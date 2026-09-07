<div align="center">

<h1>Waveshare Boards</h1>

<p><strong>Waveshare board definitions for ESP Board Manager</strong></p>

<a href="https://github.com/waveshareteam/waveshare-boards/actions/workflows/ci.yml"><img src="https://github.com/waveshareteam/waveshare-boards/actions/workflows/ci.yml/badge.svg" alt="Build"></a>
<a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache--2.0-blue.svg" alt="License"></a>

English | [简体中文](README_ZH.md)

<a href="docs/CI.md">📚 Documentation</a> · <a href="#quick-start">🚀 Quick start</a>

</div>

---

This board pack describes Waveshare hardware using Board Manager YAML files and
board-specific setup code. It follows the component-pack workflow in
[demo_boards](https://github.com/LiuCodee/demo_boards), with Waveshare definitions,
bilingual documentation, and generation/build checks.

<a id="board-catalog"></a>

## 📋 Board catalog

The table lists definitions present in this repository. Peripheral declarations
such as `spi_sd` describe a bus; they do not imply a complete storage application.

<!-- BEGIN SUPPORTED_BOARDS -->
| Board | Chip | Device definitions | Peripheral definitions |
| --- | --- | --- | --- |
| [`esp32_s3_touch_lcd_7`](esp32_s3_touch_lcd_7/) | ESP32-S3 | `display_lcd`, `lcd_touch` | `i2c_master`, `uart_rs485`, `uart_external`, `adc_sensor`, `spi_sd` |
<!-- END SUPPORTED_BOARDS -->

Board configurations remain in their existing directories. CI validates and
compiles them; electrical behavior and operation on physical hardware require
separate testing.

<a id="quick-start"></a>

## 🚀 Quick start

Activate ESP-IDF first. Until a registry version is published, clone this pack
into an **existing ESP-IDF application's** component directory:

```bash
mkdir -p components
git clone https://github.com/waveshareteam/waveshare-boards.git components/waveshare-boards
python -m pip install esp-bmgr-assist==0.8.3
idf.py bmgr -l
idf.py bmgr -b esp32_s3_touch_lcd_7
idf.py build
```

Use the Component Manager version from the active IDF environment; the tested
versions for each IDF line are listed in [the CI guide](docs/CI.md).

Keep the local directory name `waveshare-boards`: ESP-IDF uses the directory name as
the component name. The pack declares `espressif/esp_board_manager` as a public
dependency. Applications can include `esp_board_manager.h` and initialize the
selected board using `esp_board_manager_init()`.

After maintainers publish the component, install it from the registry instead
of keeping the local clone:

```bash
idf.py add-dependency "waveshare/waveshare-boards"
```

The intended registry identity is `waveshare/waveshare-boards`; a repository version
or a passing packaging check does not by itself mean that version is published.

## 🗂️ Repository layout

```text
esp32_s3_touch_lcd_7/    Board metadata, peripherals, devices, and setup code
idf_component.yml      Component metadata and managed dependencies
CMakeLists.txt         Board-pack component registration
ci/test_app/           Board Manager integration compile test
ci/scripts/            Discovery, change routing, and validation tests
ci/versions.json       Exact ESP-IDF and Python tooling versions
scripts/               Generated board catalog maintenance
docs/                  CI and registry publication guides
.github/               Workflows and contribution templates
```

The integration test is maintained under `ci/`; this repository currently has
no first-party application examples, Arduino sketches, or delivery firmware.

## 🛠️ Maintenance

```bash
python scripts/update_supported_boards_table.py
python ci/scripts/board_pack.py check
python -m unittest discover -s ci/scripts -p 'test_*.py' -v
```

Board directories may be nested up to three levels. Each directory name must
match its unique `board` value. The generated catalog lists the actual device
and peripheral names, including board removals and renames.

## 📚 Documentation

- [CI coverage and local validation](docs/CI.md)
- [Registry credentials and publication](docs/PUBLISHING.md)
- [Contributing](CONTRIBUTING.md)
- [Support](SUPPORT.md)
- [ESP Board Manager documentation](https://docs.espressif.com/projects/esp-board-manager/en/latest/index.html)

## 📄 License

[Apache License 2.0](LICENSE). Preserve existing notices when contributing or
reusing board definitions and third-party code.
