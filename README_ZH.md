<div align="center">

<h1>Waveshare Boards</h1>

<p><strong>适用于 ESP Board Manager 的微雪开发板定义组件包</strong></p>

<a href="https://github.com/waveshareteam/waveshare_boards/actions/workflows/ci.yml"><img src="https://github.com/waveshareteam/waveshare_boards/actions/workflows/ci.yml/badge.svg" alt="Build"></a>
<a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache--2.0-blue.svg" alt="License"></a>

[English](README.md) | 简体中文

<a href="docs/CI_ZH.md">📚 文档</a> · <a href="#quick-start">🚀 快速开始</a>

</div>

---

本组件包通过 Board Manager YAML 文件和板级初始化代码描述微雪硬件。
仓库参考 [demo_boards](https://github.com/LiuCodee/demo_boards) 的组件包工作方式，
提供微雪板卡定义、双语文档以及生成和编译检查。

<a id="board-catalog"></a>

## 📋 板卡目录

### ESP32-S3

<table>
  <tr>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.75C"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.75C/6d19f7e16fb9a3be219e9eed43ca9eb56c88d01c/docs/images/esp32-s3-touch-amoled-1.75c.jpg" alt="ESP32-S3-Touch-AMOLED-1.75C 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-S3-Touch-AMOLED-1.75C</strong></p>
      <a href="boards/esp32_s3_touch_amoled_1_75c/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.75C">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.8"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.8/7ab8f957e22ea1ab811256359f4eddcaaf49ee91/assets/ESP32-S3-Touch-AMOLED-1.8-hero.webp" alt="ESP32-S3-Touch-AMOLED-1.8 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-S3-Touch-AMOLED-1.8</strong></p>
      <a href="boards/esp32_s3_touch_amoled_1_8/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.8">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-2.16"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-AMOLED-2.16/225a62bff11b5d0a0b607873860d39485a9a9685/docs/assets/ESP32-S3-Touch-AMOLED-2.16-details-1.jpg" alt="ESP32-S3-Touch-AMOLED-2.16 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-S3-Touch-AMOLED-2.16</strong></p>
      <a href="boards/esp32_s3_touch_amoled_2_16/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-2.16">产品文档</a>
    </td>
  </tr>
  <tr>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-S3-Touch-LCD-7"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-S3-Touch-LCD-7/bc51a84b30c0cea018fee6e0d7427ec10102060e/assets/Product-1.webp" alt="ESP32-S3-Touch-LCD-7 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-S3-Touch-LCD-7</strong></p>
      <a href="boards/esp32_s3_touch_lcd_7/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-S3-Touch-LCD-7">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-S3-LR1121-OLED-1.54"><img src="https://raw.githubusercontent.com/waveshareteam/esp32-s3-lr1121-oled-1.54/main/assets/ESP32-S3-LR1121-OLED-1.54-DocHeader-1.webp" alt="ESP32-S3-LR1121-OLED-1.54-XF 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-S3-LR1121-OLED-1.54-XF</strong></p>
      <a href="boards/esp32_s3_lr1121_oled_1_54_xf/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-S3-LR1121-OLED-1.54">产品文档</a>
    </td>
  </tr>
</table>

### ESP32-P4

<table>
  <tr>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-NANO-WIFI6-DB"><img src="https://www.waveshare.net/photo/development-board/ESP32-P4-NANO-WIFI6-DB/ESP32-P4-NANO-WIFI6-DB-details-1.jpg" alt="ESP32-P4-NANO-WIFI6-DB 开发板" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-NANO-WIFI6-DB</strong></p>
      <a href="boards/esp32_p4_nano_wifi6_db/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-NANO-WIFI6-DB">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6-DB"><img src="https://www.waveshare.net/photo/development-board/ESP32-P4-WIFI6-DB/ESP32-P4-WIFI6-DB-details-1.jpg" alt="ESP32-P4-WIFI6-DB 开发板" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6-DB</strong></p>
      <a href="boards/esp32_p4_wifi6_db/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6-DB">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-NANO"><img src="https://www.waveshare.net/photo/development-board/ESP32-P4-NANO/ESP32-P4-NANO-details-1.jpg" alt="ESP32-P4-NANO 开发板" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-NANO</strong></p>
      <a href="boards/esp32_p4_nano/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-NANO">产品文档</a>
    </td>
  </tr>
  <tr>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6"><img src="https://www.waveshare.net/photo/development-board/ESP32-P4-WIFI6/ESP32-P4-WIFI6-details-1.jpg" alt="ESP32-P4-WIFI6 开发板" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6</strong></p>
      <a href="boards/esp32_p4_wifi6/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-4B"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-P4-WIFI6-Touch-LCD-4B/main/docs/assets/esp32-p4-wifi6-touch-lcd-4b.jpg" alt="ESP32-P4-WIFI6-Touch-LCD-4B 开发板" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6-Touch-LCD-4B</strong></p>
      <a href="boards/esp32_p4_wifi6_touch_lcd_4b/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-4B">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-7B"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-P4-WIFI6-Touch-LCD-7B/main/assets/ESP32-P4-WIFI6-Touch-LCD-7B-details-1.jpg" alt="ESP32-P4-WIFI6-Touch-LCD-7B 开发板" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6-Touch-LCD-7B</strong></p>
      <a href="boards/esp32_p4_wifi6_touch_lcd_7b/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-7B">产品文档</a>
    </td>
  </tr>
  <tr>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-3.5"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-P4-WIFI6-Touch-LCD-3.5/870588e62daaf723f3c52f0060d8dc053665ed30/assets/ESP32-P4-WIFI6-Touch-LCD-3.5-details-1.jpg" alt="ESP32-P4-WIFI6-Touch-LCD-3.5 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6-Touch-LCD-3.5</strong></p>
      <a href="boards/esp32_p4_wifi6_touch_lcd_3_5/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-3.5">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-4.3"><img src="https://www.waveshare.net/photo/development-board/ESP32-P4-WIFI6-Touch-LCD-4.3/ESP32-P4-WIFI6-Touch-LCD-4.3-1.jpg?v=26020602" alt="ESP32-P4-WIFI6-Touch-LCD-4.3 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6-Touch-LCD-4.3</strong></p>
      <a href="boards/esp32_p4_wifi6_touch_lcd_4_3/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-4.3">产品文档</a>
    </td>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-X"><img src="https://github.com/ycorez/ESP32-P4-WIFI6-Touch-LCD-X/raw/main/assets/ESP32-P4-WIFI6-Touch-LCD-X.jpg" alt="ESP32-P4-WIFI6-Touch-LCD-X 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6-Touch-LCD-X</strong></p>
      <a href="boards/esp32_p4_wifi6_touch_lcd_x/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-X">产品文档</a>
    </td>
  </tr>
  <tr>
    <td align="center" valign="middle" width="100"><a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-XC"><img src="https://raw.githubusercontent.com/waveshareteam/ESP32-P4-WIFI6-Touch-LCD-XC/6948676d64515b6273998236d98a643ead640735/assets/ESP32-P4-WIFI6-Touch-LCD-XC.jpg" alt="ESP32-P4-WIFI6-Touch-LCD-XC 开发板产品图" width="90"></a></td>
    <td valign="middle" width="230">
      <p><strong>ESP32-P4-WIFI6-Touch-LCD-XC</strong></p>
      <a href="boards/esp32_p4_wifi6_touch_lcd_xc/">板卡定义</a> · <a href="https://docs.waveshare.com/ESP32-P4-WIFI6-Touch-LCD-XC">产品文档</a>
    </td>
  </tr>
</table>

部分板卡可通过 `-a` 选择[屏幕配置](#屏幕配置)。

<details>
<summary>设备与外设定义</summary>

下表列出仓库中实际存在的定义。`spi_sd` 等外设声明描述的是总线，
不代表已经提供完整的存储应用。

<!-- BEGIN SUPPORTED_BOARDS -->
| 开发板 | 芯片 | 设备定义 | 外设定义 |
| --- | --- | --- | --- |
| [`esp32_p4_nano`](boards/esp32_p4_nano/) | ESP32-P4 | `audio_dac`, `audio_adc`, `sdcard_power_ctrl`, `fs_sdcard`, `display_lcd`, `lcd_touch`, `camera` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `gpio_sd_power`, `ldo_mipi`, `dsi_display` |
| [`esp32_p4_nano_wifi6_db`](boards/esp32_p4_nano_wifi6_db/) | ESP32-P4 | `audio_dac`, `audio_adc`, `camera`, `display_lcd`, `sdcard_power_ctrl`, `fs_sdcard`, `lcd_touch` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `gpio_sd_power`, `ldo_mipi`, `dsi_display` |
| [`esp32_p4_wifi6`](boards/esp32_p4_wifi6/) | ESP32-P4 | `camera`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard`, `sdcard_power_ctrl` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `gpio_sd_power`, `ldo_mipi`, `dsi_display` |
| [`esp32_p4_wifi6_db`](boards/esp32_p4_wifi6_db/) | ESP32-P4 | `camera`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard`, `sdcard_power_ctrl` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `gpio_sd_power`, `ldo_mipi`, `dsi_display` |
| [`esp32_p4_wifi6_touch_lcd_3_5`](boards/esp32_p4_wifi6_touch_lcd_3_5/) | ESP32-P4 | `audio_dac`, `audio_adc`, `fs_spiffs`, `fs_sdcard`, `display_lcd`, `lcd_touch`, `lcd_brightness`, `camera` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `ldo_mipi`, `spi_lcd`, `ledc_backlight` |
| [`esp32_p4_wifi6_touch_lcd_4_3`](boards/esp32_p4_wifi6_touch_lcd_4_3/) | ESP32-P4 | `audio_dac`, `audio_adc`, `fs_spiffs`, `sdcard_power_ctrl`, `fs_sdcard`, `display_lcd`, `lcd_touch`, `lcd_brightness`, `camera` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `gpio_sd_power`, `ldo_mipi`, `dsi_display`, `ledc_backlight` |
| [`esp32_p4_wifi6_touch_lcd_4b`](boards/esp32_p4_wifi6_touch_lcd_4b/) | ESP32-P4 | `audio_dac`, `audio_adc`, `fs_spiffs`, `sdcard_power_ctrl`, `fs_sdcard`, `display_lcd`, `lcd_touch`, `lcd_brightness`, `camera` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `gpio_sd_power`, `ldo_mipi`, `dsi_display`, `ledc_backlight` |
| [`esp32_p4_wifi6_touch_lcd_7b`](boards/esp32_p4_wifi6_touch_lcd_7b/) | ESP32-P4 | `fs_sdcard`, `sdcard_power_ctrl`, `audio_adc`, `audio_dac`, `camera`, `display_lcd`, `lcd_touch`, `lcd_brightness`, `fs_spiffs` | `gpio_sd_power`, `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `ldo_mipi`, `dsi_display`, `ledc_backlight` |
| [`esp32_p4_wifi6_touch_lcd_x`](boards/esp32_p4_wifi6_touch_lcd_x/) | ESP32-P4 | `audio_dac`, `audio_adc`, `fs_spiffs`, `fs_sdcard`, `display_lcd`, `lcd_touch`, `lcd_brightness`, `camera` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `ldo_mipi`, `dsi_display`, `ledc_backlight` |
| [`esp32_p4_wifi6_touch_lcd_xc`](boards/esp32_p4_wifi6_touch_lcd_xc/) | ESP32-P4 | `audio_dac`, `audio_adc`, `fs_spiffs`, `sdcard_power_ctrl`, `fs_sdcard`, `display_lcd`, `lcd_touch`, `backlight_power_ctrl`, `lcd_brightness`, `camera` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `gpio_sd_power`, `ldo_mipi`, `dsi_display`, `ledc_backlight`, `gpio_backlight_control` |
| [`esp32_s3_lr1121_oled_1_54_xf`](boards/esp32_s3_lr1121_oled_1_54_xf/) | ESP32-S3 | `button_boot`, `button_power`, `button_menu`, `button_ptt`, `button_rtc_irq`, `radio`, `oled`, `rtc`, `battery`, `audio`, `sdcard` | `i2c_main`, `spi_radio`, `gpio_boot`, `gpio_power`, `gpio_menu`, `gpio_ptt`, `gpio_rtc_irq`, `gpio_amplifier`, `gpio_radio_cs`, `gpio_radio_irq`, `gpio_radio_reset`, `gpio_radio_busy`, `i2s_speaker`, `i2s_microphone` |
| [`esp32_s3_touch_amoled_1_75c`](boards/esp32_s3_touch_amoled_1_75c/) | ESP32-S3 | `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_amoled_1_8`](boards/esp32_s3_touch_amoled_1_8/) | ESP32-S3 | `gpio_expander`, `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_amoled_2_16`](boards/esp32_s3_touch_amoled_2_16/) | ESP32-S3 | `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_lcd_7`](boards/esp32_s3_touch_lcd_7/) | ESP32-S3 | `display_lcd`, `lcd_touch` | `i2c_master`, `uart_rs485`, `uart_external`, `adc_sensor`, `spi_sd` |
<!-- END SUPPORTED_BOARDS -->

</details>

板卡定义统一放在 `boards/<完整型号>/`。新增的三个 AMOLED 型号来自 Espressif Brookesia，
详见[硬件版本与迁移说明](docs/BOARDS_ZH.md)。CI 校验生成与编译，实机运行效果需要单独测试。

LR1121 OLED XF 配置提供硬件定义，自定义设备需要应用侧适配代码，详见
[支持范围](docs/BOARDS_ZH.md#lr1121-oled-154-xf-定义范围)。

<a id="quick-start"></a>

## 🚀 快速开始

先激活 ESP-IDF 环境。使用本地源码时，将本仓库克隆到**已有 ESP-IDF 应用**的组件目录：

```bash
mkdir -p components
git clone https://github.com/waveshareteam/waveshare_boards.git components/waveshare_boards
python -m pip install esp-bmgr-assist==0.8.3
idf.py set-target esp32s3
idf.py bmgr -l
idf.py bmgr -b esp32_s3_touch_lcd_7
idf.py build
```

使用当前 IDF 环境配套的 Component Manager；各 IDF 版本线测试过的版本见 [CI 指南](docs/CI_ZH.md)。

`set-target` 会先解析组件依赖，再进行板卡发现。已有配置的 ESP32-S3 应用可以改用
`idf.py reconfigure`，保留原配置。

本地目录名请使用 `waveshare_boards`，因为 ESP-IDF 使用目录名作为组件名。
组件包已经公开依赖 `espressif/esp_board_manager`；应用可以包含
`esp_board_manager.h`，并通过 `esp_board_manager_init()` 初始化所选板卡。

也可以通过组件库安装已发布版本：

```bash
idf.py add-dependency "waveshare/waveshare_boards"
idf.py reconfigure
idf.py bmgr -l
```

可用版本见[在线组件库](https://components.espressif.com/components/waveshare/waveshare_boards)。

### 屏幕配置

部分板卡提供可通过 `-a` 选择的屏幕配置：

| 板卡 | 屏幕配置 |
| --- | --- |
| `esp32_p4_wifi6_touch_lcd_x` | `lcd_7_dsi_touch_a`、`lcd_8_dsi_touch_a` |
| `esp32_p4_wifi6_touch_lcd_xc` | `lcd_3_4_dsi_touch_c` |

例如，在 ESP32-P4 应用中选择 7 英寸屏幕配置：

```bash
idf.py bmgr -b esp32_p4_wifi6_touch_lcd_x -a lcd_7_dsi_touch_a
```

更换板卡或屏幕配置后，需要重新运行对应命令。未指定屏幕配置时，
`esp32_p4_wifi6_touch_lcd_x` 使用 10.1 英寸基础配置。
更多用法见 [Board Manager amend 文档](https://docs.espressif.com/projects/esp-board-manager/zh_CN/latest/create-board/amend.html)。

## 🗂️ 仓库结构

```text
boards/<完整型号>/      板卡信息、外设、设备与初始化代码
include/               不依赖应用框架的 AMOLED 调光函数
idf_component.yml      组件元数据与托管依赖
CMakeLists.txt         板卡包组件注册
ci/test_app/           Board Manager 集成编译测试
ci/scripts/            发现、变更路由与验证测试
ci/versions.json       明确的 ESP-IDF 与 Python 工具版本
scripts/               板卡目录生成维护
docs/                  硬件版本、CI 与组件库发布指南
.github/               工作流与贡献模板
```

集成测试位于 `ci/`。当前仓库没有第一方应用示例、Arduino 工程或交付固件。

## 🛠️ 维护

```bash
python scripts/update_supported_boards_table.py
python ci/scripts/board_pack.py check
python -m unittest discover -s ci/scripts -p 'test_*.py' -v
```

使用一层 `boards/` 容器，型号名称保留芯片前缀，不再增加芯片或类别目录，
确保从应用 `components/` 根目录扫描时仍在 Board Manager 深度限制内。
型号目录名必须与唯一的 `board` 字段一致，目录表根据实际定义自动生成。

## 📚 文档

- [板卡组织、硬件版本与迁移说明](docs/BOARDS_ZH.md)
- [AMOLED 亮度控制](docs/INTEGRATIONS_ZH.md)
- [CI 覆盖范围与本地验证](docs/CI_ZH.md)
- [组件库凭据与发布](docs/PUBLISHING_ZH.md)
- [贡献指南](CONTRIBUTING_ZH.md)
- [问题支持](SUPPORT_ZH.md)
- [ESP Board Manager 文档](https://docs.espressif.com/projects/esp-board-manager/zh_CN/latest/index.html)

## 📄 许可证

采用 [Apache License 2.0](LICENSE)，导入文件保留原有 SPDX 声明，包括 CC0-1.0。
详见[来源说明](docs/BOARDS_ZH.md)。
