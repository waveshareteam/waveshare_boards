# Board pack CI

[简体中文](CI_ZH.md) · [Home](../README.md)

## Build contract

[The workflow](https://github.com/waveshareteam/waveshare_boards/blob/main/.github/workflows/ci.yml)
discovers first-party board definitions and builds `ci/test_app` for each selected
board. It does not scan upstream board packs or publish firmware artifacts.

`ci/versions.json` selects ESP-IDF **v5.5.5** and **v6.1**, the stable releases
selected for the two supported lines. Board Manager versions are resolved once
per run from the registry using the root manifest: the oldest and newest stable
published versions satisfying the range are tested, with duplicates removed.
Registry lookup failures fail the job. Each matrix entry records an exact version.

The initial Board Manager floor is **0.7.2**, matching the reference board-pack
template. The current floor and latest resolve to the same version, giving eight
board builds across the two IDF lines. Keep the floor until a deliberate
compatibility change is tested. Unsupported catalog profiles or generation failures fail CI; they are
not counted as successful builds.

For IDF 5.5, Component Manager **2.5.0** is selected because the reference template documents
incorrect Board Manager Kconfig dependency handling in **2.4.8**. The helper is
**esp-bmgr-assist 0.8.3**. IDF 6.1 uses Component Manager **3.0.3**,
consistent with its 3.0.x constraints and required CMake interface version 5;
2.5.0 cannot serve that interface. Revisit these tooling pins when upgrading the matrix and
rerun generation and compilation on both IDF lines.

Board Manager 0.7.2 recognizes `waveshare_boards` in managed installs, local
`components/` clones, and manifest `override_path` dependencies. The integration
test uses the override form and invokes discovery without `-c`.
Use `boards/<full_model>/` inside the pack: local scanning begins at the
application's `components/` root, so adding another chip directory exceeds its
three-level search limit. See [layout and source evidence](BOARDS.md).

## Change routing

The lightweight metadata, unit-test, navigation, and packaging checks run on every
PR. `Board pack checks` is the stable aggregate status suitable for branch protection.

| Change | Board builds |
| --- | --- |
| Root or nested Markdown, documentation images | None |
| Issue forms, license, ignore rules | None |
| Files inside one board directory | That board |
| CI scripts, tests, workflows, public headers, root manifest or CMake | All boards |
| Deleted or renamed board paths | Both sides are considered; removed paths can select all remaining boards |
| Firmware documentation, source, binary, archive | Reported separately; no board build |
| Unrecognized non-documentation input | All boards; path reported for review |
| Empty or unavailable Git diff | Error; no silent fallback |
| Manual or publication validation | All boards |

`ci/scripts/board_pack.py matrix` writes the `matrix` and `has_builds` outputs
consumed by the build job. New commits cancel obsolete PR runs. Publication runs
are serialized and are not cancelled by later PR commits.

`check_docs.py` is a limited check of root/docs bilingual pairs, local navigation
targets, and homepage section symmetry. It does not claim full Markdown ownership,
fragment, language-routing, or private-data auditing. Review those items when
changing public documentation.

## Local reproduction

Clone into a directory named `waveshare_boards`. With the desired ESP-IDF environment
active, install Component Manager 2.5.0 for IDF 5.5 or 3.0.3 for IDF 6.1,
then run from that directory:

```bash
python -m pip install PyYAML==6.0.3 esp-bmgr-assist==0.8.3
python -m unittest discover -s ci/scripts -p 'test_*.py' -v
python ci/scripts/board_pack.py check
python scripts/update_supported_boards_table.py --check
python ci/scripts/check_docs.py
python ci/scripts/board_pack.py matrix --all
python ci/scripts/board_pack.py pin 0.7.2
idf.py -C ci/test_app bmgr -l
idf.py -C ci/test_app bmgr -b esp32_s3_touch_lcd_7
idf.py -C ci/test_app build
compote component pack --name waveshare_boards
```

Use a fresh checkout for each IDF/Board Manager combination so generated
components, dependency locks, and sdkconfig files do not cross environments.
For an offline matrix reproduction, append `--bmgr 0.7.2` to the matrix command.
For a PR range, use `matrix --base origin/main` with the complete Git history.
The integration manifest pin is temporary build input; do not commit a changed
pin unless intentionally changing the baseline.

## Evidence and boundaries

Board YAML and `setup_device.c` are board-specific source, not copied generic
drivers. The GT911 driver remains a managed dependency at the board's existing
**1.2.1** constraint; retain that baseline until another driver version is checked
with both framework lines and the hardware. The setup function validates the
Board Manager-selected I2C address and delegates to the managed driver.

Successful compilation proves API and generated-code compatibility, not wiring,
PSRAM provisioning, touch reset sequencing, or physical operation. Official schematic links and the AMOLED migration checks are recorded in
[board notes](BOARDS.md). Physical operation and application-level integration
remain separate validation steps.

Primary references: [IDF v5.5.5](https://github.com/espressif/esp-idf/releases/tag/v5.5.5),
[IDF v6.1](https://github.com/espressif/esp-idf/releases/tag/v6.1),
[5.5 to 6.0 migration](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32s3/migration-guides/release-6.x/6.0/index.html),
[6.0 to 6.1 migration](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32s3/migration-guides/release-6.x/6.1/index.html),
and [Board Manager](https://components.espressif.com/components/espressif/esp_board_manager).
