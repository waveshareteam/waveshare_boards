<div align="center">

<h1>Waveshare Boards</h1>

<p><strong>Waveshare board definitions for ESP Board Manager</strong></p>

<a href="https://github.com/waveshareteam/waveshare_boards/actions/workflows/ci.yml"><img src="https://github.com/waveshareteam/waveshare_boards/actions/workflows/ci.yml/badge.svg" alt="Build"></a>
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

<table>
  <tr>
    <td align="center" valign="middle" width="180">
      <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.75C">
        <img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.75C/6d19f7e16fb9a3be219e9eed43ca9eb56c88d01c/docs/images/esp32-s3-touch-amoled-1.75c.jpg" alt="ESP32-S3-Touch-AMOLED-1.75C development board" width="160">
      </a>
    </td>
    <td valign="middle" width="600">
      <p><strong>ESP32-S3-Touch-AMOLED-1.75C</strong></p>
      <a href="boards/esp32_s3_touch_amoled_1_75c/">Board definition</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.75C">Product documentation</a>
    </td>
  </tr>
  <tr>
    <td align="center" valign="middle" width="180">
      <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.8">
        <img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.8/7ab8f957e22ea1ab811256359f4eddcaaf49ee91/assets/ESP32-S3-Touch-AMOLED-1.8-hero.webp" alt="ESP32-S3-Touch-AMOLED-1.8 development board" width="160">
      </a>
    </td>
    <td valign="middle" width="600">
      <p><strong>ESP32-S3-Touch-AMOLED-1.8</strong></p>
      <a href="boards/esp32_s3_touch_amoled_1_8/">Board definition</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.8">Product documentation</a>
    </td>
  </tr>
  <tr>
    <td align="center" valign="middle" width="180">
      <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-2.16">
        <img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-AMOLED-2.16/225a62bff11b5d0a0b607873860d39485a9a9685/docs/assets/ESP32-S3-Touch-AMOLED-2.16-details-1.jpg" alt="ESP32-S3-Touch-AMOLED-2.16 development board" width="160">
      </a>
    </td>
    <td valign="middle" width="600">
      <p><strong>ESP32-S3-Touch-AMOLED-2.16</strong></p>
      <a href="boards/esp32_s3_touch_amoled_2_16/">Board definition</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-2.16">Product documentation</a>
    </td>
  </tr>
  <tr>
    <td align="center" valign="middle" width="180">
      <a href="https://docs.waveshare.com/ESP32-S3-Touch-LCD-7">
        <img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-LCD-7/bc51a84b30c0cea018fee6e0d7427ec10102060e/assets/Product-1.webp" alt="ESP32-S3-Touch-LCD-7 development board" width="160">
      </a>
    </td>
    <td valign="middle" width="600">
      <p><strong>ESP32-S3-Touch-LCD-7</strong></p>
      <a href="boards/esp32_s3_touch_lcd_7/">Board definition</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-LCD-7">Product documentation</a>
    </td>
  </tr>
</table>

<details>
<summary>Device and peripheral definitions</summary>

The table lists definitions present in this repository. Peripheral declarations
such as `spi_sd` describe a bus; they do not imply a complete storage application.

<!-- BEGIN SUPPORTED_BOARDS -->
| Board | Chip | Device definitions | Peripheral definitions |
| --- | --- | --- | --- |
| [`esp32_s3_touch_amoled_1_75c`](boards/esp32_s3_touch_amoled_1_75c/) | ESP32-S3 | `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_amoled_1_8`](boards/esp32_s3_touch_amoled_1_8/) | ESP32-S3 | `gpio_expander`, `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_amoled_2_16`](boards/esp32_s3_touch_amoled_2_16/) | ESP32-S3 | `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_lcd_7`](boards/esp32_s3_touch_lcd_7/) | ESP32-S3 | `display_lcd`, `lcd_touch` | `i2c_master`, `uart_rs485`, `uart_external`, `adc_sensor`, `spi_sd` |
<!-- END SUPPORTED_BOARDS -->

</details>

Definitions live under `boards/<full_model>/`. The three AMOLED profiles come
from Espressif Brookesia; see [board revisions and migration notes](docs/BOARDS.md).
CI validates generation and compilation; physical operation requires board testing.

<a id="quick-start"></a>

## 🚀 Quick start

Activate ESP-IDF first. For a local checkout, clone this pack into an
**existing ESP-IDF application's** component directory:

```bash
mkdir -p components
git clone https://github.com/waveshareteam/waveshare_boards.git components/waveshare_boards
python -m pip install esp-bmgr-assist==0.8.3
idf.py set-target esp32s3
idf.py bmgr -l
idf.py bmgr -b esp32_s3_touch_lcd_7
idf.py build
```

Use the Component Manager version from the active IDF environment; the tested
versions for each IDF line are listed in [the CI guide](docs/CI.md).

`set-target` resolves the pack dependencies before discovery. In an already
configured ESP32-S3 application, use `idf.py reconfigure` instead to preserve
its configuration.

Keep the local directory name `waveshare_boards`: ESP-IDF uses the directory name as
the component name. The pack declares `espressif/esp_board_manager` as a public
dependency. Applications can include `esp_board_manager.h` and initialize the
selected board using `esp_board_manager_init()`.

Published versions can be installed from the registry:

```bash
idf.py add-dependency "waveshare/waveshare_boards"
idf.py reconfigure
idf.py bmgr -l
```

Find available versions in the
[Component Registry](https://components.espressif.com/components/waveshare/waveshare_boards).

## 🗂️ Repository layout

```text
boards/<full_model>/   Board metadata, peripherals, devices, and setup code
include/               Framework-independent AMOLED brightness helper
idf_component.yml      Component metadata and managed dependencies
CMakeLists.txt         Board-pack component registration
ci/test_app/           Board Manager integration compile test
ci/scripts/            Discovery, change routing, and validation tests
ci/versions.json       Exact ESP-IDF and Python tooling versions
scripts/               Generated board catalog maintenance
docs/                  Board revisions, CI, and registry publication guides
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

Use one `boards/` container and keep the chip prefix in each model name. Do not
add another chip/category directory: the layout must fit Board Manager scanning
from an application's `components/` root. Each model directory must match its
unique `board` value. The catalog is regenerated from the definitions.

## 📚 Documentation

- [Board organization, hardware revisions, and migration](docs/BOARDS.md)
- [AMOLED brightness control](docs/INTEGRATIONS.md)
- [CI coverage and local validation](docs/CI.md)
- [Registry credentials and publication](docs/PUBLISHING.md)
- [Contributing](CONTRIBUTING.md)
- [Support](SUPPORT.md)
- [ESP Board Manager documentation](https://docs.espressif.com/projects/esp-board-manager/en/latest/index.html)

## 📄 License

[Apache License 2.0](LICENSE), with imported files retaining their original
SPDX notices, including CC0-1.0. See [source attribution](docs/BOARDS.md).
