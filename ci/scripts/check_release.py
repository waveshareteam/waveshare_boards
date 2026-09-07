#!/usr/bin/env python3
"""Require a version-matched tag on main before a real registry upload."""

import os
from pathlib import Path
import subprocess
import sys

import yaml

ROOT = Path(__file__).resolve().parents[2]


def check(root, dry_run, ref):
    if dry_run not in {"true", "false"}:
        raise ValueError("DRY_RUN must be true or false")
    if dry_run == "true":
        return
    version = yaml.safe_load((root / "idf_component.yml").read_text())["version"]
    if ref != f"refs/tags/v{version}":
        raise ValueError("publish from the v<manifest-version> tag after merging to main")
    subprocess.run(["git", "merge-base", "--is-ancestor", "HEAD", "origin/main"],
                   cwd=root, check=True)


if __name__ == "__main__":
    try:
        check(ROOT, os.environ.get("DRY_RUN"), os.environ.get("GITHUB_REF"))
    except Exception as error:
        print(f"release check failed: {error}", file=sys.stderr)
        sys.exit(1)
