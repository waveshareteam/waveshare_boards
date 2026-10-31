# Board organization and migration

[简体中文](BOARDS_ZH.md) · [Home](../README.md)

## Catalog layout

The repository and component are named `waveshare_boards`; the registry identity
is `waveshare/waveshare_boards`. Keep all definitions in:

```text
boards/
  esp32_s31_touch_lcd_4_3c/
  esp32_s3_lr1121_epaper_2_9/
  esp32_s3_lr1121_oled_1_54_xf/
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

## S31 Touch LCD 4.3C definition scope

`esp32_s31_touch_lcd_4_3c` targets **ESP32-S31 (`esp32s31`)**, not ESP32-S3.
It defines an 800 × 480 RGB565 display with two PSRAM frame buffers, GT911 touch,
LEDC backlight (60% default), 4-bit SDMMC with active-low card power, ES8389
audio input/output, RS485 and custom RTC, IMU, battery-gauge and CAN-FD devices.
The included `setup_device.c` validates the selected GT911 address before
calling the managed touch driver.

The declared connections agree with the product's
[hardware reference](https://github.com/waveshareteam/ESP32-S31-Touch-LCD-4.3C/blob/8007c28a43f19fea878a61df5ad34ee61cecb5e4/HARDWARE_REFERENCE.md);
the [S31 schematic](https://github.com/waveshareteam/ESP32-S31-Touch-LCD-4.3C/blob/8007c28a43f19fea878a61df5ad34ee61cecb5e4/hardware/schematics/ESP32-S31-Touch-LCD-4.3C-Schematics.pdf)
is available for further review. This is not a full schematic audit or a
physical-board test of this import.

| Interface | Configured GPIOs / parameters |
| --- | --- |
| I2C0 | SDA 0, SCL 1 |
| RGB display | D0–D15: 8–19, 33–36; PCLK 40 at 26 MHz; DE 43, HSYNC 44, VSYNC 45 |
| Touch / backlight | GT911 reset 46, interrupt 2; backlight 47 at 5 kHz |
| SDMMC | CLK 24, CMD 25, D0–D3: 20–23; power enable 56, active low |
| I2S0 / amplifier | BCLK 37, WS 38, DOUT 39, DIN 42; amplifier enable 57 |
| RS485 | UART1 TX 6, RX 7, RTS/DE 61; 115200 baud, half-duplex |
| CAN-FD | TX 4, RX 5; 500 kbit/s arbitration, 2 Mbit/s data |
| RTC / IMU / battery | PCF85063ATL `0x51`, IRQ 3; QMI8658 `0x6B`/`0x6A`, IRQ 54; BQ27220 `0x55` |

The sensor addresses above are 7-bit. GT911 uses 7-bit `0x5D`/`0x14`, and
ES8389 uses `0x10`; their YAML entries use the Board Manager 8-bit forms
`0xBA`/`0x28` and `0x20`. Audio defaults to 48 kHz, 16-bit stereo with
no external MCLK configured; this does not prove the hardware lacks an MCLK net.
The battery design capacity of 650 mAh is a software default.

Display, touch, backlight, SD power/mount and audio do not skip initialization.
In particular, an absent SD card can make automatic board initialization fail;
mount failure never automatically formats the card. Applications must configure
PSRAM for the display frame buffers. No board-specific sdkconfig defaults are
included here. `rtc_pcf85063`, `imu_qmi8658`, `bat_bq27220` and `canfd`
use `init_skip: true`; using these custom devices through Board Manager requires
application-supplied adapters and explicit initialization. The RS485 entry
describes a UART, and no USB device or host-stack initialization is defined here.

The product's [toolchain configuration](https://github.com/waveshareteam/ESP32-S31-Touch-LCD-4.3C/blob/8007c28a43f19fea878a61df5ad34ee61cecb5e4/releases/config.json)
pins ESP-IDF `6.2.0-dev-08e0d30a74ad` (commit
`08e0d30a74ad0bfd5a34933142b80f45619ee410`), Board Manager `0.7.5`,
Component Manager `3.1.2` and codec `2.0.0-beta5`.
These are product-side references, not new validation results for this pack.
The existing IDF 5.5.5/6.1 CI matrix is not an established S31 build matrix;
see [CI limitations](CI.md#esp32-s31-coverage-gap).

## LR1121 ePaper 2.9 definition scope

`esp32_s3_lr1121_epaper_2_9` imports the three YAML files from the product's
original example board profile. The published [product examples](https://github.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/blob/4aa6c326ca3a23317b180c86bbaea15d3327386f/examples/esp-idf/README.md#board-manager-integration)
consume this profile through `waveshare/waveshare_boards`. The profile describes
an ESP32-S3, an SSD16xx-compatible 2.9-inch e-paper device, LR1121 radio,
PCF85063A RTC, AXP2602 battery gauge, one WS2812 status LED and display power control.

The following values are configuration references, not verified schematic results:

| Interface | Configured GPIOs / parameters |
| --- | --- |
| I2C | SDA 13, SCL 14; RTC address `0x51`, battery gauge address `0x62` |
| Shared SPI2 | SCLK 40, MOSI 45, MISO 46; display 10 MHz, radio 8 MHz |
| E-paper | BUSY 8, reset 9, DC 17, CS 18; power GPIO 38, active high |
| LR1121 | DIO1 6, NSS 42, reset 39, BUSY 41 |
| Other | RTC interrupt 7; WS2812 GPIO 21, one GRB LED; BOOT input GPIO 0 |

`status_led` uses Board Manager's native RMT `led_strip` device.
It and the four custom devices (`epaper_ssd16xx`, `rtc_pcf85063a`,
`bat_axp2602`, `radio_lr1121`) retain `init_skip: true` and require explicit
initialization. Applications must provide and register initialization/deinitialization
adapters and their driver dependencies for the custom devices. These adapters are
not included in this pack; the YAML alone does not implement display refresh or
radio communication. Both devices share `spi_shared` and use separate chip selects.

`epaper_power_ctrl` uses the native GPIO power-control device and does **not** skip
initialization; its GPIO 38 default is high. The display references it through
`power_ctrl_device`. Applications must account for this power behavior during
startup and sleep. BOOT is only a GPIO declaration; no button device or SD card
device is defined. The AXP2602 sense-resistor value of 10 mΩ is a configuration
default that still needs checking against the installed hardware.

The published [hardware reference](https://github.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/blob/4aa6c326ca3a23317b180c86bbaea15d3327386f/docs/hardware-facts.zh-CN.md)
and [Rev1.1 schematic](https://github.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/blob/4aa6c326ca3a23317b180c86bbaea15d3327386f/hardware/schematics/ESP32-S3-LR1121-ePaper-2.9-XF-Schematics.pdf)
are available in the product repository. The GPIO assignments above agree with
that hardware reference; this import has not undergone a full schematic audit
or physical-board validation. Verify the exact PCB revision, display/power timing,
radio configuration and battery measurement separately. GPIO 38 controls a shared
power domain for the display, radio, WS2812 and microSD, despite the device name
`epaper_power_ctrl`.
The README uses the published [product image](https://raw.githubusercontent.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/4aa6c326ca3a23317b180c86bbaea15d3327386f/docs/images/ESP32-S3-LR1121-ePaper-2.9-details-5-2.webp),
pinned to product-repository commit `4aa6c326ca3a23317b180c86bbaea15d3327386f`.

## LR1121 OLED 1.54 XF definition scope

`esp32_s3_lr1121_oled_1_54_xf` imports the three YAML files from the
[product's factory-source profile](https://github.com/waveshareteam/esp32-s3-lr1121-oled-1.54/tree/a0bf73932ef06d047810b17888cc7e33f064680e/firmware/12_factory_demo/components/esp32_s3_lr1121_oled_1_54_support/boards/esp32_s3_lr1121_oled_1_54_xf) without changing its hardware values.
The hardware reference is the product repository's [schematic](https://github.com/waveshareteam/esp32-s3-lr1121-oled-1.54/blob/a0bf73932ef06d047810b17888cc7e33f064680e/hardware/schematics/esp32-s3-lr1121-oled-1_54_schematics.pdf).
The profile describes ESP32-S3 GPIOs, shared I2C, LR1121 SPI, speaker/microphone
I2S, buttons and one-bit SDMMC.

This is a hardware-definition profile. All devices retain `init_skip: true` for
application-controlled initialization. `radio`, `oled`, `rtc`, `battery` and
`audio` are custom devices; this pack does not include their initialization
callbacks, product adapters or managed driver dependencies. Applications must
supply and register those adapters before initializing the devices by name.
Calling `esp_board_manager_init()` alone does not make these devices usable.
Buttons and SD use built-in Board Manager device types and also require explicit
initialization. SD mount failure does not automatically format the card.

The radio adapter must also initialize the separately declared `gpio_radio_busy`;
it is not in the custom radio's four-entry peripheral list. The audio defaults
are 16 kHz with STD speaker output and four-slot TDM microphone input. Application
profiles such as the walkie-talkie's 8 kHz capture need their own configuration.
When adopting this published profile, remove duplicate application-local board
YAML definitions while retaining the required product adapters.

Generation and compilation checks do not validate device initialization, shared
I2S operation, RF settings, power behavior or physical wiring. Hardware testing
has not been performed for this import; verify the exact PCB revision and those
functions separately before claiming complete product support.

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
