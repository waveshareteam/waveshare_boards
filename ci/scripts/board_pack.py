#!/usr/bin/env python3
"""Discover board packs, route complete Git diffs, and prepare build matrices."""

from __future__ import annotations

import argparse
from collections import defaultdict
import json
import os
from pathlib import Path, PurePosixPath
import posixpath
import re
import subprocess
import sys

import yaml

ROOT = Path(__file__).resolve().parents[2]
EXCLUDED = {"ci", "scripts", "docs", "firmware", "examples", "build",
            "managed_components", "components", "third_party", "libraries", "dist"}
STABLE = re.compile(r"\d+\.\d+\.\d+(?:~\d+)?")


def load_yaml(path):
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"expected a YAML mapping: {path.name}")
    return data


def discover(root):
    boards = []
    names = set()
    for path in sorted(root.rglob("board_info.yaml")):
        relative = path.parent.relative_to(root)
        if any(p.startswith(".") or p in EXCLUDED for p in relative.parts):
            continue
        # Local components scanning starts above the pack directory. Reserve
        # that level so override, managed, and local installs all work.
        if not 1 <= len(relative.parts) <= 2:
            raise ValueError(f"board exceeds the supported two-level depth: {relative}")
        info = load_yaml(path)
        name, chip = info.get("board", ""), info.get("chip", "")
        if not isinstance(name, str) or not re.fullmatch(r"[a-z][a-z0-9_]*", name):
            raise ValueError(f"invalid board name: {relative}")
        if name != path.parent.name or name in names:
            raise ValueError(f"board name must be unique and match its directory: {relative}")
        if not isinstance(chip, str) or not re.fullmatch(r"esp32[a-z0-9]*", chip):
            raise ValueError(f"invalid chip: {relative}")
        for filename, key in (("board_devices.yaml", "devices"),
                              ("board_peripherals.yaml", "peripherals")):
            data = load_yaml(path.parent / filename)
            if not isinstance(data.get(key), list):
                raise ValueError(f"{relative}/{filename} must contain a {key} list")
        names.add(name)
        boards.append({"board": name, "target": chip, "path": relative.as_posix()})
    if not boards:
        raise ValueError("no first-party board definitions found")
    return boards


def changed_paths(root, base, head="HEAD"):
    # --no-renames includes both sides as D/A, including removed board folders.
    result = subprocess.run(
        ["git", "diff", "--name-only", "--no-renames", "-z", f"{base}...{head}", "--"],
        cwd=root, check=True, capture_output=True)
    paths = result.stdout.decode("utf-8").split("\0")[:-1]
    if not paths:
        raise ValueError("empty changed-file scope; use --all for an intentional full build")
    return paths


HEADER_SUFFIXES = {".h", ".hh", ".hpp", ".hxx", ".inc", ".inl"}


def tracked_files(root, revision):
    output = subprocess.check_output(["git", "ls-tree", "-rz", "--name-only", revision], cwd=root)
    return set(output.decode().split("\0")[:-1])


def include_users(root, revision, files):
    """Reverse literal includes at one revision, using the source and pack include directories."""
    sources = ["*." + ext for ext in ("c", "cc", "cpp", "cxx", "S")] + ["*" + ext for ext in HEADER_SUFFIXES]
    result = subprocess.run(["git", "grep", "-zI", "-E", "-e", r"^[[:space:]]*#[[:space:]]*include",
                             "-e", r"__has_include[[:space:]]*\(",
                             revision, "--", *sources], cwd=root, capture_output=True)
    if result.returncode not in (0, 1):  # git grep returns 1 for no matches.
        result.check_returncode()
    users = defaultdict(set)
    dynamic = set()
    for line in result.stdout.decode(errors="surrogateescape").splitlines():
        location, directive = line.split("\0", 1)
        source = location.split(":", 1)[1]
        for match in re.finditer(r'(?:^\s*#\s*include\b|\b__has_include\s*\()\s*(?:[<"]([^">]+)[">])?', directive):
            header = match[1]
            if header is None:
                dynamic.add(source)
                continue
            for directory in (posixpath.dirname(source), "include", ""):
                candidate = posixpath.normpath(posixpath.join(directory, header))
                if candidate in files:
                    users[candidate].add(source)
    return users, dynamic


def git_route(root, boards, base, head):
    ancestor = subprocess.check_output(["git", "merge-base", base, head], cwd=root, text=True).strip()
    paths = changed_paths(root, ancestor, head)
    all_boards = {b["board"] for b in boards}
    scopes = defaultdict(set)
    manifests = []
    for revision in (ancestor, head):
        files = tracked_files(root, revision)
        if "idf_component.yml" in paths and "idf_component.yml" in files:
            content = subprocess.check_output(["git", "show", f"{revision}:idf_component.yml"], cwd=root)
            manifest = yaml.safe_load(content)
            if isinstance(manifest, dict) and "version" in manifest:
                manifests.append({k: v for k, v in manifest.items() if k != "version"})

        directories = {p.parent.as_posix(): p.parent.name for filename in files
                       if (p := PurePosixPath(filename)).name == "board_info.yaml"
                       and 1 <= len(p.parent.parts) <= 2
                       and not any(part in EXCLUDED or part.startswith(".") for part in p.parent.parts)}

        def owner(path):
            return next((name for directory, name in directories.items() if path.startswith(directory + "/")), None)

        headers = {p for p in paths if p in files and PurePosixPath(p).suffix in HEADER_SUFFIXES
                   and (p.startswith("include/") or owner(p))}
        users, dynamic = include_users(root, revision, files) if headers else ({}, set())
        for path in set(paths) & files:
            if board := owner(path):
                scopes[path].add(board)
            if path not in headers:
                continue
            # Macro includes cannot be resolved statically. Include their consumers conservatively.
            pending = set(users.get(path, ())) | dynamic
            seen = set()
            while pending:
                source = pending.pop()
                if source in seen:
                    continue
                seen.add(source)
                if board := owner(source):
                    scopes[path].add(board)
                elif not source.startswith("include/"):
                    scopes[path].update(all_boards)
                pending.update(users.get(source, ()))

    if len(manifests) == 2 and manifests[0] == manifests[1]:
        scopes["idf_component.yml"] = set()
    # Unattributed shared inputs retain the full-build fallback in route().
    return route(boards, paths, scopes)


def route(boards, paths, scopes=None):
    if not paths:
        raise ValueError("a complete nonempty changed-file scope is required")
    selected, firmware, unknown = set(), [], []
    docs_only = True
    for value in paths:
        path = PurePosixPath(value)
        if path.is_absolute() or ".." in path.parts or not path.parts:
            raise ValueError("invalid repository-relative changed path")
        doc = path.suffix.lower() == ".md" or (
            path.parts[0] == "docs" and path.suffix.lower() in {".svg", ".png", ".jpg"})
        if not doc:
            docs_only = False
        if path.parts[0] == "firmware":
            kind = "documentation" if doc else (
                "binary" if path.suffix == ".bin" else
                "archive" if path.suffix == ".zip" else "source_or_config")
            firmware.append({"path": value, "kind": kind})
            continue
        if doc or value in {"LICENSE", ".gitignore"} or value.startswith(".github/ISSUE_TEMPLATE/"):
            continue
        if scopes is not None and value in scopes:
            selected.update(scopes[value])
            continue
        direct = [b["board"] for b in boards if value.startswith(b["path"] + "/")]
        if direct:
            selected.update(direct)
        else:
            selected.update(b["board"] for b in boards)
            if not (value.startswith(("ci/", "scripts/", ".github/workflows/", "include/"))
                    or value in {"CMakeLists.txt", "idf_component.yml"}):
                unknown.append(value)
    return {"boards": [b for b in boards if b["board"] in selected],
            "docs_only": docs_only, "firmware": firmware, "unknown": unknown}


def registry_versions(root):
    from idf_component_tools.registry.service_details import get_storage_client
    spec = load_yaml(root / "idf_component.yml")["dependencies"]["espressif/esp_board_manager"]["version"]
    releases = get_storage_client().versions("espressif/esp_board_manager", spec).versions
    versions = sorted(v.version for v in releases if STABLE.fullmatch(str(v.version)))
    if not versions:
        raise ValueError("no stable Board Manager version satisfies the manifest")
    return list(dict.fromkeys([str(versions[0]), str(versions[-1])]))


def matrix(root, boards, versions):
    config = json.loads((root / "ci/versions.json").read_text(encoding="utf-8"))
    # Targets an IDF version cannot build, such as a chip it does not know yet.
    unsupported = config.get("unsupported_targets", {})
    cells = []
    for board in boards:
        for idf in config["idf"]:
            if board["target"] in unsupported.get(idf, ()):
                continue
            for version in versions:
                cells.append(dict(board=board["board"], target=board["target"],
                                  idf=idf, bmgr=version,
                                  component_manager=config["component_manager"][idf],
                                  bmgr_assist=config["bmgr_assist"]))
    return {"include": cells}


def pin(root, version):
    if not STABLE.fullmatch(version):
        raise ValueError("an exact stable Board Manager version is required")
    path = root / "ci/test_app/main/idf_component.yml"
    data = load_yaml(path)
    data["dependencies"]["espressif/esp_board_manager"]["version"] = f"=={version}"
    path.write_text(yaml.safe_dump(data, sort_keys=False), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("check")
    pin_parser = sub.add_parser("pin")
    pin_parser.add_argument("version")
    plan = sub.add_parser("matrix")
    scope = plan.add_mutually_exclusive_group(required=True)
    scope.add_argument("--all", action="store_true")
    scope.add_argument("--base")
    plan.add_argument("--head", default="HEAD")
    plan.add_argument("--bmgr", action="append", help="exact versions for offline reproduction")
    args = parser.parse_args()
    try:
        if args.command == "pin":
            pin(args.root, args.version)
            return 0
        boards = discover(args.root)
        if args.command == "check":
            print(json.dumps(boards, indent=2))
            return 0
        routed = ({"boards": boards, "docs_only": False, "firmware": [], "unknown": []}
                  if args.all else git_route(args.root, boards, args.base, args.head))
        versions = (args.bmgr or registry_versions(args.root)) if routed["boards"] else []
        if any(not STABLE.fullmatch(v) for v in versions):
            raise ValueError("matrix versions must be exact stable releases")
        result = matrix(args.root, routed["boards"], versions)
        print(json.dumps({"matrix": result, **routed}, indent=2))
        if output := os.environ.get("GITHUB_OUTPUT"):
            with open(output, "a", encoding="utf-8") as stream:
                stream.write(f"matrix={json.dumps(result, separators=(',', ':'))}\n")
                stream.write(f"has_builds={str(bool(result['include'])).lower()}\n")
                stream.write(f"docs_only={str(routed['docs_only']).lower()}\n")
        return 0
    except Exception as error:
        print(f"board-pack validation failed: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
