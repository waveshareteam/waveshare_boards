"""Build bilingual, commit-specific evidence from GitHub files and check runs."""

from collections import Counter
import re
import subprocess

from codex_review_source import get_json, get_pages


SUMMARY_JOBS = {"Identify Codex review receipt", "Publish bilingual review summary"}
UNSUCCESSFUL = {"action_required", "cancelled", "failure", "stale", "startup_failure", "timed_out"}


def code_span(value: str) -> str:
    value = " ".join(value.split())[:200]
    fence = "`" * (1 + max((len(run) for run in re.findall(r"`+", value)), default=0))
    return f"{fence} {value} {fence}"


def file_scope(repo, number, pr, sha):
    """Pin the comparison to immutable commits, including for superseded PR heads."""
    base = pr["base"]["sha"]
    comparison = f"{base}...{sha}"
    diff_url = f"https://github.com/{repo}/compare/{comparison}"
    # GitHub includes at most 300 files on the first comparison page.
    files = get_json(f"repos/{repo}/compare/{comparison}?per_page=1")["files"]
    partial = len(files) >= 300
    if (partial or pr["merged"]) and pr["head"]["sha"] == sha:
        pr_path = f"repos/{repo}/pulls/{number}"
        all_files = get_pages(f"{pr_path}/files?per_page=100")
        current = get_json(pr_path)
        if current["head"]["sha"] == sha and current["base"]["sha"] == base:
            files = all_files
            partial = len(files) < current["changed_files"]
            diff_url = f"https://github.com/{repo}/pull/{number}/files"
    paths = [item["filename"] for item in files]
    count = f"{len(paths)}+" if partial else str(len(paths))
    examples = ", ".join(code_span(path) for path in paths[:6])
    if len(paths) > 6 or partial:
        examples += ", …"
    line = f"- [变更范围 / Diff scope]({diff_url}) · {count} 个文件 / files"
    if examples:
        line += " · " + examples
    boards = {parts[1] for path in paths if len(parts := path.split("/")) > 2 and parts[0] == "boards"}
    if boards:
        shown = ", ".join(code_span(board) for board in sorted(boards)[:5])
        line += f"\n  - 涉及板卡 / Changed boards · {shown}" + (" …" if len(boards) > 5 else "")
    return line, boards


def checks_at(repo, sha):
    checks = get_pages(f"repos/{repo}/commits/{sha}/check-runs?filter=latest&per_page=100", key="check_runs")
    return [check for check in checks if not (
        (check.get("app") or {}).get("slug") == "github-actions" and check["name"] in SUMMARY_JOBS
    )]


def scope_lines(repo: str, number: int, sha: str) -> tuple[list[str], str]:
    pr = get_json(f"repos/{repo}/pulls/{number}")
    diff, boards = file_scope(repo, number, pr, sha)
    check_sha = sha
    checks = checks_at(repo, sha)
    merge = pr.get("merge_commit_sha")
    if not checks and merge and sha == pr["head"]["sha"]:
        # The PR's transient merge ref can change between API requests. Check its parents.
        try:
            parents = get_json(f"repos/{repo}/commits/{merge}")["parents"]
        except subprocess.CalledProcessError:
            parents = []
        if len(parents) == 2 and parents[1]["sha"] == sha:
            check_sha = merge
            checks = checks_at(repo, merge)
    checks_url = f"https://github.com/{repo}/commit/{check_sha}/checks"
    label = "合并提交检查 / Merge-commit checks" if check_sha != sha else "提交检查 / Commit checks"
    if not checks:
        return [diff, f"- [{label}]({checks_url}) · 未找到检查记录 / No check runs reported."], "unknown"

    counts = Counter("pending" if check["status"] != "completed" else check["conclusion"] for check in checks)
    failed = sum(counts[key] for key in UNSUCCESSFUL)
    pending, passed = counts["pending"], counts["success"]
    other = len(checks) - failed - pending - passed
    line = (f"- [{label}]({checks_url}) · 通过 / passed {passed} · "
            f"失败或中止 / unsuccessful {failed} · 等待中 / pending {pending}")
    if other:
        line += f" · 跳过或其他 / skipped or other {other}"
    state = "failed" if failed else "pending" if pending else "passed" if passed else "unknown"

    def order(check):
        name = check["name"]
        relevant = name.split(" / ", 1)[0] in boards
        return (check["conclusion"] not in UNSUCCESSFUL, not relevant, " / " in name, name)

    links = []
    for check in sorted(checks, key=order)[:6]:
        text = code_span(check["name"])
        url = check.get("html_url") or ""
        if re.fullmatch(re.escape(f"https://github.com/{repo}") + r"/(?:actions/runs/\d+/job/\d+|runs/\d+)", url):
            text = f"[{text}]({url})"
        links.append(f"{text} ({check['conclusion'] or check['status']})")
    return [diff, line, "  - 作业记录 / Check records · " + ", ".join(links)], state
