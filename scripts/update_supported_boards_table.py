#!/usr/bin/env python3
"""Regenerate bilingual tables from the definitions; --check never writes."""

import argparse
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "ci/scripts"))
from board_pack import discover, load_yaml

BEGIN = "<!-- BEGIN SUPPORTED_BOARDS -->"
END = "<!-- END SUPPORTED_BOARDS -->"


def render(root, chinese):
    headings = ["开发板", "芯片", "设备定义", "外设定义"] if chinese else [
        "Board", "Chip", "Device definitions", "Peripheral definitions"]
    rows = ["| " + " | ".join(headings) + " |", "| --- | --- | --- | --- |"]
    for board in discover(root):
        directory = root / board["path"]
        devices = load_yaml(directory / "board_devices.yaml")["devices"]
        peripherals = load_yaml(directory / "board_peripherals.yaml")["peripherals"]
        chip = board["target"].upper().replace("ESP32S", "ESP32-S").replace("ESP32P", "ESP32-P").replace("ESP32C", "ESP32-C").replace("ESP32H", "ESP32-H")
        rows.append("| " + " | ".join([
            f"[`{board['board']}`]({board['path']}/)", chip,
            ", ".join(f"`{d['name']}`" for d in devices) or "—",
            ", ".join(f"`{p['name']}`" for p in peripherals) or "—"]) + " |")
    return "\n".join(rows)


def update(root, check=False):
    stale = []
    for filename, chinese in (("README.md", False), ("README_ZH.md", True)):
        path = root / filename
        text = path.read_text(encoding="utf-8")
        if text.count(BEGIN) != 1 or text.count(END) != 1:
            raise ValueError(f"{filename}: missing or duplicate table markers")
        start, end = text.index(BEGIN) + len(BEGIN), text.index(END)
        if end < start:
            raise ValueError("table markers are out of order")
        result = text[:start] + "\n" + render(root, chinese) + "\n" + text[end:]
        if result != text:
            stale.append(filename)
            if not check:
                path.write_text(result, encoding="utf-8")
    return stale


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    stale = update(ROOT, args.check)
    if stale and args.check:
        print("Regenerate board tables: " + ", ".join(stale), file=sys.stderr)
        sys.exit(1)
