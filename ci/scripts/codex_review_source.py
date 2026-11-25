"""Read one completed Codex code review from the GitHub pull request timeline."""

import json
import re
import subprocess
import time
from datetime import datetime, timedelta


BOT = "chatgpt-codex-connector[bot]"
RECEIPT = "<!-- codex-review-receipt:v1 -->"
SUMMARY = "<!-- codex-pull-request-review-summary -->"
NO_FINDINGS = re.compile(r"^Codex Review: Didn['’]t find any major issues\b", re.MULTILINE)
REVIEWED_SHA = re.compile(r"\*\*Reviewed commit:\*\*\s*`([0-9a-f]{7,40})`")
REVIEW_HEADING = re.compile(r"^#+ [^\n]*\bCodex Review\s*$", re.MULTILINE)


def get_json(path):
    return json.loads(subprocess.check_output(["gh", "api", path]))


def get_pages(path, *, key=None):
    query = f".{key}[] | @json" if key else ".[] | @json"
    output = subprocess.check_output(["gh", "api", "--paginate", path, "--jq", query])
    return [json.loads(line) for line in output.splitlines()]


def timestamp(value):
    return datetime.fromisoformat(value.replace("Z", "+00:00"))


def code_state(body):
    state = re.search(r"<!-- codex-review-state:v1 (.*?) -->", body)
    if not body.startswith(SUMMARY) or not state:
        return None
    code = json.loads(state[1]).get("code_review") or {}
    # The top-level comment also changes on security reviews and thread resolutions.
    row = next((line for line in body.splitlines() if re.match(r"^\|[^|]*\*\*Code Review\*\*\s*\|", line)), "")
    when = re.search(r'<relative-time datetime="([^"]+)"', row)
    if not when or not code.get("commit_sha"):
        return None
    return (code.get("status"), timestamp(when[1]), code["commit_sha"], code.get("has_any_findings"))


def receipt(kind, entry):
    body = entry.get("body") or ""
    if entry["user"]["login"] != BOT or not body.startswith(RECEIPT):
        return None
    note = ""
    if kind == "comment":
        match = REVIEWED_SHA.search(body)
        if not match or not NO_FINDINGS.search(body):
            return None
        sha, when = match[1], entry["created_at"]
    else:
        heading = REVIEW_HEADING.search(body)
        if not heading:
            return None
        sha, when = entry["commit_id"], entry.get("submitted_at")
        if not when:
            return None
        note = body[heading.end():].split("<details>", 1)[0]
        note = REVIEWED_SHA.sub("", note).replace("Here are some automated review suggestions for this pull request.", "").strip()
    return {
        "key": f"{kind}:{entry['id']}", "sha": sha, "url": entry["html_url"],
        "has_findings": kind == "review", "completed_at": when,
        "findings": [], "review_note": note,
    }


def list_receipts(prefix, number, kind):
    endpoint = f"pulls/{number}/reviews" if kind == "review" else f"issues/{number}/comments"
    records = get_pages(f"{prefix}/{endpoint}?per_page=100")
    return [item for record in records if (item := receipt(kind, record)) is not None]


def clean_summary(entry):
    """Automatic clean reviews can update only the bot's rolling status comment."""
    if entry["user"]["login"] != BOT:
        return None
    state = code_state(entry.get("body") or "")
    if not state or state[0] != "completed" or state[3] is not False:
        return None
    _, end, sha, _ = state
    if not re.fullmatch(r"[0-9a-f]{40}", sha):
        return None
    return {
        "key": f"summary:{entry['id']}:{sha}:{end.strftime('%Y%m%dT%H%M%S%f')}",
        "sha": sha, "url": entry["html_url"], "completed_at": end.isoformat(),
        "has_findings": False, "findings": [], "review_note": "",
    }


def latest_matching(items, sha, start, end):
    matches = (item for item in items if sha.startswith(item["sha"])
               and start <= timestamp(item["completed_at"]) <= end)
    return max(matches, key=lambda item: timestamp(item["completed_at"]), default=None)


def select_manual(repo, number, selector):
    prefix = f"repos/{repo}"
    if selector:
        match = re.fullmatch(r"(review|comment):([1-9][0-9]*)", selector)
        if not match:
            raise ValueError("Expected review:<id> or comment:<id>")
        kind, identifier = match.groups()
        if kind == "review":
            entry = get_json(f"{prefix}/pulls/{number}/reviews/{identifier}")
        else:
            entry = get_json(f"{prefix}/issues/comments/{identifier}")
            if not entry["issue_url"].endswith(f"/{repo}/issues/{number}"):
                raise ValueError("Receipt belongs to a different pull request")
        selected = receipt(kind, entry)
    else:
        comments = get_pages(f"{prefix}/issues/{number}/comments?per_page=100")
        standalone = [item for c in comments if (item := receipt("comment", c))]
        items = list_receipts(prefix, number, "review") + standalone
        for comment in comments:
            summary = clean_summary(comment)
            if summary:
                end = timestamp(summary["completed_at"])
                # Keep a manual clean review's original comment ID when both exist.
                item = latest_matching(standalone, summary["sha"], end - timedelta(minutes=2), end)
                items.append(item or summary)
        selected = max(items, key=lambda item: timestamp(item["completed_at"]), default=None)
    if selected is None:
        raise ValueError("No supported Codex code review receipt found")
    return selected


def select_event(prefix, number, event):
    comment = event["comment"]
    issue = event["issue"]
    if "pull_request" not in issue or issue["number"] != number:
        return None
    if event.get("sender", {}).get("login") != BOT or comment["user"]["login"] != BOT:
        return None
    direct_receipt = receipt("comment", comment)
    if direct_receipt:
        return direct_receipt
    state = code_state(comment["body"])
    previous = code_state(event.get("changes", {}).get("body", {}).get("from", ""))
    if not state or state[0] != "completed" or not isinstance(state[3], bool):
        return None
    if previous and state[:3] == previous[:3]:
        return None
    _, end, sha, found = state
    # GitHub rounds receipt timestamps to seconds. Never use SHA alone: multiple
    # independently requested reviews can share one commit.
    start = end - timedelta(minutes=2)
    if previous and previous[0] == "running" and previous[2] == sha:
        start = previous[1].replace(microsecond=0)
    for delay in (0, 3, 8, 20):
        if delay:
            time.sleep(delay)
        items = list_receipts(prefix, number, "review" if found else "comment")
        match = latest_matching(items, sha, start, end)
        if match:
            return match
    if not found:
        summary = clean_summary(comment)
        if summary:
            return summary
    raise RuntimeError("Completed Codex review has no matching receipt yet")


def load_report_input(repo: str, number: int, event: dict, receipt_selector: str | None) -> dict | None:
    prefix = f"repos/{repo}"
    if "comment" in event:
        selected = select_event(prefix, number, event)
    else:
        selected = select_manual(repo, number, receipt_selector)
    if selected is None:
        return None
    if len(selected["sha"]) < 40:
        selected["sha"] = get_json(f"{prefix}/commits/{selected['sha']}")["sha"]
    if selected["has_findings"]:
        identifier = selected["key"].split(":")[1]
        path = f"{prefix}/pulls/{number}/reviews/{identifier}/comments?per_page=100"
        for delay in (0, 3, 8, 20):
            if delay:
                time.sleep(delay)
            rows = [row for row in get_pages(path) if row["user"]["login"] == BOT and not row.get("in_reply_to_id")]
            if rows:
                selected["findings"] = [
                    {"body": row["body"], "url": row["html_url"], "path": row["path"],
                     "line": row.get("line") or row.get("original_line")}
                    for row in rows
                ]
                break
        if not selected["findings"] and not selected["review_note"]:
            raise RuntimeError("Codex review is missing its inline findings; refusing to publish a clean result")
    return selected
