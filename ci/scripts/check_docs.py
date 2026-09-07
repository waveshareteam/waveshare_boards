#!/usr/bin/env python3
"""Limited documentation check: pairs, navigation targets, and H2 symmetry.

This is not a complete Markdown, fragment, privacy, or ownership audit.
"""

from pathlib import Path
import re
import sys
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[2]


def check(root):
    findings = []
    paths = list(root.glob("*.md")) + list((root / "docs").rglob("*.md"))
    for path in paths:
        text = path.read_text(encoding="utf-8")
        chinese = path.stem.endswith("_ZH")
        peer = path.with_name(path.stem[:-3] + ".md" if chinese else path.stem + "_ZH.md")
        if not peer.is_file() or f"]({peer.name})" not in text[:1500]:
            findings.append(f"{path.name}: missing companion or language navigation")
        visible = re.sub(r"```.*?```", "", text, flags=re.S)
        for match in re.finditer(r"\]\(([^)]+)\)", visible):
            url = urlsplit(match.group(1))
            if url.scheme or url.netloc or not url.path:
                continue
            target = (path.parent / unquote(url.path)).resolve()
            if not target.is_relative_to(root.resolve()) or not target.exists():
                findings.append(f"{path.name}: missing or escaping local link")
    english, chinese = root / "README.md", root / "README_ZH.md"
    if english.is_file() and chinese.is_file():
        icons = lambda p: re.findall(r"^## (\S+)", p.read_text(encoding="utf-8"), re.M)
        if icons(english) != icons(chinese):
            findings.append("homepage section icons must match")
    return findings


if __name__ == "__main__":
    findings = check(ROOT)
    print("\n".join(findings) if findings else "Documentation navigation passed")
    sys.exit(bool(findings))
