"""Offline regression tests for the two GitHub Codex review result formats."""

import json
import unittest
from unittest.mock import patch

import codex_review_scope as scope
import codex_review_source as source
import codex_review_summary as summary


REPO = "sample/boards"
SHA = "a" * 40


def receipt_comment():
    return {
        "user": {"login": source.BOT}, "id": 9, "created_at": "2026-01-01T01:00:01Z",
        "html_url": "https://github.com/sample/boards/pull/1#issuecomment-9",
        "body": f"{source.RECEIPT}\nCodex Review: Didn't find any major issues.\n**Reviewed commit:** `{SHA}`",
    }


def event(comment):
    return {"issue": {"number": 1, "pull_request": {}}, "sender": {"login": source.BOT}, "comment": comment}


class ReviewSummaryTests(unittest.TestCase):
    def test_clean_receipt_does_not_override_failing_ci(self):
        review = source.load_report_input(REPO, 1, event(receipt_comment()), None)
        self.assertEqual("comment:9", review["key"])
        review.update(items=[], scope=["build-board: failure"], ci="failed")
        body = summary.render(REPO, review)
        for evidence in ("未报告重大问题", "no major issues", "失败", "failed checks", "build-board: failure", review["url"]):
            self.assertIn(evidence, body)

    def test_two_reviews_on_one_commit_use_their_own_completion_time(self):
        def status(ended):
            state = {"code_review": {"commit_sha": SHA, "status": "completed", "has_any_findings": True}}
            return (source.SUMMARY + "\n<!-- codex-review-state:v1 " + json.dumps(state) + " -->\n"
                    f'| **Code Review** | Completed <relative-time datetime="{ended}"> |')

        items = [
            {"key": "review:2", "sha": SHA, "completed_at": "2026-01-01T02:00:02Z"},
            {"key": "review:1", "sha": SHA, "completed_at": "2026-01-01T01:00:02Z"},
        ]
        with patch.object(source, "list_receipts", return_value=items):
            for hour, key in (("01", "review:1"), ("02", "review:2")):
                with self.subTest(hour=hour):
                    comment = dict(receipt_comment(), body=status(f"2026-01-01T{hour}:00:04Z"))
                    selected = source.select_event("repos/" + REPO, 1, event(comment))
                    self.assertEqual(key, selected["key"])

    def test_other_accounts_cannot_supply_or_edit_a_codex_receipt(self):
        for actor in ("sender", "author"):
            with self.subTest(actor=actor):
                webhook = event(receipt_comment())
                account = webhook["sender"] if actor == "sender" else webhook["comment"]["user"]
                account["login"] = "contributor"
                self.assertIsNone(source.load_report_input(REPO, 1, webhook, None))

    def test_translate_only_missing_language_and_keep_both_sources(self):
        mono = summary.findings_text({"body": "[P2] 缺少电源配置", "url": "https://example.org/finding-1"})
        both = summary.findings_text({"body": "中文：引脚冲突\nEnglish: Pin conflict", "url": "https://example.org/finding-2"})
        review = {"sha": SHA, "url": "https://example.org/review", "items": [mono, both], "ci": "passed", "scope": []}
        with patch("codex_review_translate.translate", return_value="Power configuration is missing.") as translator:
            body = summary.render(REPO, review)
            translator.assert_called_once_with(mono["texts"]["zh"], "zh", "en")
        for value in ("P2", "缺少电源配置", "Power configuration is missing.", "引脚冲突", "Pin conflict",
                      mono["url"], both["url"], "建议修改后合并", "before merging"):
            self.assertIn(value, body)

    def test_prior_commit_does_not_inherit_new_head_checks(self):
        current = {"head": {"sha": "b" * 40}, "base": {"sha": "c" * 40},
                   "merged": False, "merge_commit_sha": "d" * 40}
        with patch.object(scope, "get_json", side_effect=[current, {"files": [{"filename": "boards/demo/io.yaml"}]}]), \
             patch.object(scope, "checks_at", return_value=[]) as checks:
            lines, state = scope.scope_lines(REPO, 1, SHA)
        self.assertEqual("unknown", state)
        checks.assert_called_once_with(REPO, SHA)
        self.assertIn("boards/demo/io.yaml", "\n".join(lines))
        self.assertIn(SHA, "\n".join(lines))


if __name__ == "__main__":
    unittest.main()
