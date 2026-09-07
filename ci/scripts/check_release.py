#!/usr/bin/env python3
"""Validate the manifest and restrict real releases to main or its version tag."""

import os
from pathlib import Path
import re
import subprocess
import sys

import yaml

ROOT = Path(__file__).resolve().parents[2]


def check(root, dry_run, ref):
    if dry_run not in {"true", "false"}:
        raise ValueError("DRY_RUN must be true or false")
    version = yaml.safe_load((root / "idf_component.yml").read_text())["version"]
    if not isinstance(version, str) or not re.fullmatch(r"(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)", version):
        raise ValueError("release version must be a stable major.minor.patch string")
    if dry_run == "true":
        return version
    if ref not in {"refs/heads/main", f"refs/tags/v{version}"}:
        raise ValueError("publish from main or its v<manifest-version> tag")
    subprocess.run(["git", "merge-base", "--is-ancestor", "HEAD", "origin/main"],
                   cwd=root, check=True)
    return version


if __name__ == "__main__":
    try:
        version = check(ROOT, os.environ.get("DRY_RUN"), os.environ.get("GITHUB_REF"))
        if os.environ.get("GITHUB_OUTPUT"):
            with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as output:
                output.write(f"version={version}\ntag=v{version}\n")
    except Exception as error:
        print(f"release check failed: {error}", file=sys.stderr)
        sys.exit(1)
