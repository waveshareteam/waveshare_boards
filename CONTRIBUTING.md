# Contributing

[简体中文](CONTRIBUTING_ZH.md) · [Home](README.md)

Submit board definitions, fixes, or documentation through a pull request.

1. Keep each board's `board_info.yaml`, `board_peripherals.yaml`,
   `board_devices.yaml`, and optional setup source together. Names must be unique
   and match the directory; nesting is limited to three levels.
2. Base hardware changes on the matching product revision's schematic or official
   hardware reference. Include the source link or permitted reference file and
   explain what was checked. Report hardware testing separately from compilation.
3. Keep board-specific glue local and declare reusable drivers as managed
   dependencies. Explain version constraints and their validation scope.
4. Regenerate the [board catalog](README.md#board-catalog), update English and
   Simplified Chinese documentation together, and preserve existing license notices.
5. Run the commands in [CI validation](docs/CI.md) and confirm all selected checks
   on the latest PR commit. Do not commit generated components, build directories,
   credentials, personal paths, or device identifiers.

The PR description should state the board, behavioral change, tested framework
and Board Manager versions, and any remaining hardware validation limits.
Publication instructions are in [the release guide](docs/PUBLISHING.md).
