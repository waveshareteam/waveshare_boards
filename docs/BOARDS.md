# Board organization and migration

[简体中文](BOARDS_ZH.md) · [Home](../README.md)

## Catalog layout

The repository and component are named `waveshare_boards`; the registry identity
is `waveshare/waveshare_boards`. Keep all definitions in:

```text
boards/
  esp32_s3_touch_lcd_7/
  esp32_s3_touch_amoled_1_75c/
  esp32_s3_touch_amoled_1_8/
  esp32_s3_touch_amoled_2_16/
```

Each model keeps `board_info.yaml`, `board_devices.yaml`,
`board_peripherals.yaml`, setup source, and any board-local components together.
The model directory must equal the unique `board` field. Retain the chip prefix;
use the generated catalog's chip column to group products. Add a revision suffix
only when hardware revisions need independently selectable definitions.

Do not add `boards/esp32s3/<model>/`. Board Manager 0.7.2 scans local packs from
an application's `components/` root, so the package name itself consumes one of
its three search levels. `components/waveshare_boards/boards/<model>/` fits;
an extra chip folder does not. Managed and override scans start at the package,
but the layout must also work for local clones. The underscore suffix `_boards`
passes this version's override-dependency name filter. No upstream patch or
customer-path argument is needed. See the pinned
[discovery implementation](https://github.com/espressif/esp-board-manager/blob/2beb9b22b0892b343bd555a1ebc9929a7edce8fc/generators/config_generator.py).

## Imported source and adaptations

The three AMOLED definitions originate from
[Espressif Brookesia, commit 6a087b6](https://github.com/espressif/esp-brookesia/tree/6a087b6d76e989802b72fdb835928b273af76af8/hal/brookesia_hal_boards/boards/waveshare).
The existing LCD 7 definition is moved without hardware changes. This pack keeps
board-specific setup and AXP2101 glue, while display, touch, audio, and GPIO
expander drivers remain registry dependencies.

The migration makes these changes to the imported profiles:

- Include `dev_custom.h` explicitly for custom-device registration on Board Manager 0.7.2.
- Extract the unchanged QSPI brightness command into the public
  `waveshare_amoled_set_brightness()` helper for applications to control panel brightness.
  See [brightness control](INTEGRATIONS.md).
- Keep only hardware defaults in `sdkconfig.defaults.board`. Applications choose
  their own feature settings and partition layout.
- Disable automatic SD formatting after mount failure on the 1.8 and 2.16 boards.
  Mount failure is reported without erasing an existing card.
- Remove duplicate YAML keys with identical values and correct stale pin comments.

The imported PMU rail and charging initialization remains unchanged. Display,
touch, audio, SD, peripheral configuration, and panel initialization are retained
in each board directory. Runtime AMOLED brightness control remains available
through the public helper, with the original command format and percentage scaling.
The AXP2101 implementation remains board-local because the 1.8 setup also uses
its GPIO expander; merging those paths is a separate hardware change.

## Hardware reference checks

The following pins were checked against the official board schematics. Values
are ESP32 GPIO numbers unless marked as TCA9554 expander pins. Shared audio pins
are BCLK 9, WS 45, DOUT 8, DIN 10, and amplifier enable 46.

| Model | I2C SDA / SCL | QSPI clock / CS / data 0–3 | LCD reset | Touch reset / interrupt | Audio MCLK | SD CLK / CMD / D0 |
| --- | --- | --- | --- | --- | --- | --- |
| 1.75C | 15 / 14 | 38 / 12 / 4, 5, 6, 7 | 1 | 2 / 11 | 16 | Not declared |
| 1.8 V1 | 15 / 14 | 11 / 12 / 4, 5, 6, 7 | Expander 0 | Expander 2 / 21 | 16 | 2 / 1 / 3 |
| 2.16 | 15 / 14 | 38 / 12 / 4, 5, 6, 7 | 39 | 40 / 11 | 42 | 2 / 1 / 3 |

The 1.8 display power enable is TCA9554 pin 1. All three profiles use the AXP2101
at 7-bit address `0x34`. The 1.75C and 2.16 use CO5300 displays, CST9217 touch,
ES8311 output, and ES7210 input; the original 1.8 profile uses SH8601, FT3168
through the FT5x06 driver, and ES8311 for input/output. Display dimensions are
466×466, 368×448, and 480×480 respectively.

Primary references:

- [1.75C product documentation](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.75C)
  and [schematic](https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.75C/ESP32-S3-Touch-AMOLED-1.75C-schematic.pdf).
- [1.8 product documentation](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.8)
  and [schematic](https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8/ESP32-S3-Touch-AMOLED-1.8.pdf).
- [2.16 product documentation](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-2.16)
  and [schematic](https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-2.16/ESP32-S3-Touch-AMOLED-2.16-Schematic.pdf).

These checks cover the declared pin mapping and controller selection. They do
not validate analog behavior, charging characteristics, power/reset timing,
PSRAM operation, display colors, touch coordinates, audio, or SD operation on
physical hardware. Test those functions on each matching board revision.

## 1.8 hardware revision compatibility

The imported `esp32_s3_touch_amoled_1_8` profile currently targets **V1
(SH8601 / FT3168)**. V2 uses CO5300 / CST820 and must not be assumed compatible
with the unchanged profile. Identify the revision from the product label.

A single model with runtime V1/V2 detection is feasible. Waveshare's
[managed BSP implementation](https://github.com/waveshareteam/Waveshare-ESP32-components/blob/d081959d3841e0b370c2957c122bf8604ab42bc8/bsp/esp32_s3_touch_amoled_1_8/esp32_s3_touch_amoled_1_8.c)
probes the touch controller, selects FT5x06 or CST816S-family driver code, and
sets a 16-pixel X gap for the CO5300 panel. CST816S is the compatible driver API
name; the fitted V2 controller is CST820.

Adapting that approach here requires both touch dependencies, reset before
Board Manager's I2C probe, consistent address/driver selection, and the panel
offset. The shared PMU, audio, and SD definitions can remain together. This is
a compatibility assessment, not implemented V2 support; each revision needs a
physical display/touch test after implementation.

## Source licenses

Brookesia's board package carries Apache-2.0; individual imported files retain
SPDX notices, including CC0-1.0 on setup code. Preserve the original copyright
headers.
Managed drivers retain their respective licenses; this pack does not relicense
them. See [the upstream package license](https://github.com/espressif/esp-brookesia/blob/6a087b6d76e989802b72fdb835928b273af76af8/hal/brookesia_hal_boards/license.txt)
and [CC0-1.0 terms](https://creativecommons.org/publicdomain/zero/1.0/legalcode.en).
