"""Publish a bilingual PR comment for one completed GitHub Codex review."""

import argparse
import html
import json
import os
import re
import subprocess
from pathlib import Path

from codex_review_scope import scope_lines
from codex_review_source import get_pages, load_report_input

LANGUAGE_LABEL = re.compile(r"(?im)^\s*(?:[-*#]+\s*)?(中文|English)\s*[*:：\s]+")
FOOTER = re.compile(r"(?im)^\s*(?:Useful\? React|<!-- codex-code-review-finding:|<details>)")
BADGE = re.compile(r"!\[[^\]]*\]\([^)]*\)")
LINK = re.compile(r"\[([^\]\n]+)\]\((https?://[^)\s]+)\)")
PAGE_LIMIT = 52000


def findings_text(finding):
    """Prefer the two authored language blocks; otherwise translate the original once."""
    body = FOOTER.split(finding["body"], maxsplit=1)[0].strip()
    priority = re.search(r"\bP([0-3])\b", body.partition("\n")[0])
    body = re.sub(r"</?sub>|\*\*", "", BADGE.sub("", body)).strip()
    links = list(dict.fromkeys(url for _, url in LINK.findall(body) if url.startswith("https://")))
    body = LINK.sub(r"\1", body)
    labels = list(LANGUAGE_LABEL.finditer(body))
    texts = {}
    for index, label in enumerate(labels):
        end = labels[index + 1].start() if index + 1 < len(labels) else len(body)
        lang = "zh" if label[1] == "中文" else "en"
        content = body[label.end():end].strip()
        if content:
            texts[lang] = "\n\n".join(filter(None, (texts.get(lang), content)))
    if not texts:
        texts["zh" if re.search(r"[\u3400-\u9fff]", body) else "en"] = body
    if not any(texts.values()):
        raise ValueError(f"Empty finding at {finding['url']}")
    return {"priority": priority[0] if priority else "", "url": finding["url"], "links": links, "texts": texts}


def existing_pages(repo, number, key):
    tag = re.compile(rf"^<!-- codex-bilingual:v1 {re.escape(key)} (\d+)/(\d+) -->")
    pages = {}
    for c in get_pages(f"repos/{repo}/issues/{number}/comments?per_page=100"):
        match = tag.match(c.get("body") or "")
        if c["user"]["login"] == "github-actions[bot]" and match:
            pages[int(match[1])] = (c["id"], int(match[2]))
    return pages


def output(name, value):
    with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as stream:
        print(f"{name}={value}", file=stream)


def identify(repo, event):
    manual = os.environ["GITHUB_EVENT_NAME"] == "workflow_dispatch"
    number = int(event["inputs"]["pr"] if manual else event["issue"]["number"])
    selector = event["inputs"].get("receipt") if manual else None
    review = load_report_input(repo, number, event, selector)
    if review:
        output("pr", number)
        output("receipt", review["key"])
        if review["key"].startswith("summary:"):
            # Preserve the verified snapshot even if another push edits the comment
            # while the publishing job is queued.
            output("clean_snapshot", json.dumps(review, separators=(",", ":")))


def prepare(repo, number, key, folder):
    pages = existing_pages(repo, number, key)
    if pages and all(total == len(pages) for _, total in pages.values()) and set(pages) == set(range(1, len(pages) + 1)):
        print(f"Summary already published for {key}.")
        return
    snapshot = os.environ.get("CLEAN_SNAPSHOT")
    if snapshot:
        review = json.loads(snapshot)
        if review["key"] != key or review["has_findings"] is not False:
            raise ValueError("Clean review snapshot does not match the identified round")
    else:
        review = load_report_input(repo, number, {}, key)
    results = list(review["findings"])
    if review["review_note"]:
        results.append({"body": review["review_note"], "url": review["url"]})
    review["items"] = [findings_text(item) for item in results]
    if review["has_findings"] and not review["items"]:
        raise ValueError("Review reports findings but supplies no evidence")
    review["scope"], review["ci"] = scope_lines(repo, number, review["sha"])
    (folder / "input.json").write_text(json.dumps(review, ensure_ascii=False), encoding="utf-8")
    output("needed", "yes")
    output("translate", "yes" if any(len(x["texts"]) == 1 for x in review["items"]) else "no")


def merge_advice(review):
    if review["ci"] == "failed":
        return "建议先处理失败的检查再合并。", "Resolve the failed checks before merging."
    if any(i["priority"] != "P3" for i in review["items"]):
        return "建议修改后合并，或在 PR 中说明如何处理下列发现。", "Address the findings or document their disposition before merging."
    if review["ci"] == "pending":
        return "建议等正在运行的检查完成后再决定是否合并。", "Wait for the outstanding checks before deciding whether to merge."
    if review["ci"] == "unknown":
        return "建议先核实该提交的验证情况，再决定是否合并。", "Confirm validation for this commit before deciding whether to merge."
    if review["items"]:
        return "建议合并前确认以下低优先级意见。", "Acknowledge the low-priority findings before merging."
    return "本轮未报告阻塞项，已记录的检查通过，可由维护者决定合并。", "No blocker was reported and the recorded checks passed. Ready for a maintainer’s merge decision."


def quote_review(text):
    # Treat generated text as a quotation, and don't create new user notifications.
    safe = html.escape(text, quote=False).replace("@", "@\u200b")
    return "\n".join("> " + line for line in safe.splitlines())


def render(repo, review):
    zh, en = merge_advice(review)
    receipt_url = review["url"]
    sha = review["sha"]
    source_label = "原始审查 / Original review"
    if review.get("key", "").startswith("summary:"):
        source_label = "机器人滚动摘要 / Bot status (updates in place)"
    lines = ["### Codex 审查总评 / Review summary",
             f"[`{sha[:10]}`](https://github.com/{repo}/commit/{sha}) · [{source_label}]({receipt_url})",
             f"**合并建议 / Merge recommendation**\n\n{zh}\n\n{en}",
             "#### 发现与依据 / Findings and evidence"]
    translated = False
    if not review["items"]:
        lines.append("Codex 在本轮回执中未报告重大问题。 / Codex reported no major issues in this review.")
    for index, item in enumerate(review["items"], start=1):
        texts = item["texts"].copy()
        machine_language = None
        if len(texts) == 1:
            from codex_review_translate import translate
            source = next(iter(texts))
            target = "en" if source == "zh" else "zh"
            texts[target] = translate(texts[source], source, target)
            machine_language = target
            translated = True
        label = " · ".join(filter(None, (str(index), item["priority"], "原评与证据 / Source")))
        headings = {"zh": "中文（机译）" if machine_language == "zh" else "中文",
                    "en": "English (machine translation)" if machine_language == "en" else "English"}
        lines.append(f"**[{label}]({item['url']})**")
        for lang in ("zh", "en"):
            lines.append(f"**{headings[lang]}**\n\n" + quote_review(texts[lang]))
        if item["links"]:
            links = [f"[{i}](<{url}>)" for i, url in enumerate(item["links"], start=1) if "<" not in url and ">" not in url]
            lines.append("原评引用 / Cited evidence · " + " · ".join(links))
    lines.extend(["#### 核查范围 / Verification scope", *review["scope"]])
    lines.append("以上范围由工作流核对提交差异和 CI 记录得出。原评中的测试说明见回执或行内链接。 / "
                 "The workflow verified the commit diff and CI records above. See the linked review for tests reported by Codex.")
    if translated:
        lines.append("单语原评的另一语种由运行器本地翻译，技术细节请对照原评。 / "
                     "Single-language findings include a local machine translation. Refer to the original for technical wording.")
    return "\n\n".join(lines)


def split_comment(body):
    """Stay below GitHub's comment size limit, preserving UTF-8 and paragraph breaks."""
    pages = []
    while len(body.encode("utf-8")) > PAGE_LIMIT:
        prefix = body.encode("utf-8")[:PAGE_LIMIT].decode("utf-8", errors="ignore")
        cut = prefix.rfind("\n\n")
        if cut < len(prefix) // 2:
            cut = len(prefix)
        pages.append(body[:cut].rstrip())
        body = body[cut:].lstrip()
    pages.append(body)
    return pages


def publish(repo, number, key, folder):
    pages = json.loads((folder / "pages.json").read_text(encoding="utf-8"))
    previous = existing_pages(repo, number, key)
    for index, page in enumerate(pages, start=1):
        total = len(pages)
        body = f"<!-- codex-bilingual:v1 {key} {index}/{total} -->\n\n{page}"
        if total > 1:
            body += f"\n\n_{index}/{total}_"
        if index in previous:
            endpoint = f"repos/{repo}/issues/comments/{previous[index][0]}"
            method = "PATCH"
        else:
            endpoint = f"repos/{repo}/issues/{number}/comments"
            method = "POST"
        result = subprocess.check_output(["gh", "api", "--method", method, endpoint, "--input", "-", "--jq", ".html_url"],
                                         input=json.dumps({"body": body}).encode())
        print(result.decode().strip())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("step", choices=["identify", "prepare", "render", "publish"])
    step = parser.parse_args().step
    repo = os.environ["GITHUB_REPOSITORY"]
    if step == "identify":
        identify(repo, json.loads(Path(os.environ["GITHUB_EVENT_PATH"]).read_text()))
        return
    folder = Path(os.environ["RUNNER_TEMP"]) / "codex-summary"
    if step == "prepare":
        folder.mkdir(exist_ok=True)
        prepare(repo, int(os.environ["PR_NUMBER"]), os.environ["RECEIPT_KEY"], folder)
    elif step == "render":
        review = json.loads((folder / "input.json").read_text(encoding="utf-8"))
        (folder / "pages.json").write_text(json.dumps(split_comment(render(repo, review)), ensure_ascii=False), encoding="utf-8")
    else:
        publish(repo, int(os.environ["PR_NUMBER"]), os.environ["RECEIPT_KEY"], folder)


if __name__ == "__main__":
    main()
