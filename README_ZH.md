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

下表列出仓库中实际存在的定义。`spi_sd` 等外设声明描述的是总线，
不代表已经提供完整的存储应用。

<!-- BEGIN SUPPORTED_BOARDS -->
| 开发板 | 芯片 | 设备定义 | 外设定义 |
| --- | --- | --- | --- |
| [`esp32_s3_touch_amoled_1_75c`](boards/esp32_s3_touch_amoled_1_75c/) | ESP32-S3 | `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_amoled_1_8`](boards/esp32_s3_touch_amoled_1_8/) | ESP32-S3 | `gpio_expander`, `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_amoled_2_16`](boards/esp32_s3_touch_amoled_2_16/) | ESP32-S3 | `axp2101_power_manager`, `audio_dac`, `audio_adc`, `display_lcd`, `lcd_touch`, `fs_sdcard` | `i2c_master`, `i2s_audio_out`, `i2s_audio_in`, `gpio_pa_control`, `spi_display` |
| [`esp32_s3_touch_lcd_7`](boards/esp32_s3_touch_lcd_7/) | ESP32-S3 | `display_lcd`, `lcd_touch` | `i2c_master`, `uart_rs485`, `uart_external`, `adc_sensor`, `spi_sd` |
<!-- END SUPPORTED_BOARDS -->

板卡定义统一放在 `boards/<完整型号>/`。新增的三个 AMOLED 型号来自 Espressif Brookesia，
详见[硬件版本与迁移说明](docs/BOARDS_ZH.md)。CI 校验生成与编译，实机运行效果需要单独测试。

<a id="quick-start"></a>

## 🚀 快速开始

先激活 ESP-IDF 环境。在组件库版本发布之前，将本仓库克隆到**已有 ESP-IDF 应用**的组件目录：

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

维护者正式发布组件后，可以通过组件库安装，替代本地克隆：

```bash
idf.py add-dependency "waveshare/waveshare_boards"
idf.py reconfigure
idf.py bmgr -l
```

计划使用的组件库名称为 `waveshare/waveshare_boards`。仓库里的版本号或打包检查通过，
不代表该版本已经在组件库发布。

## 🗂️ 仓库结构

```text
boards/<完整型号>/      板卡信息、外设、设备与初始化代码
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
- [CI 覆盖范围与本地验证](docs/CI_ZH.md)
- [组件库凭据与发布](docs/PUBLISHING_ZH.md)
- [贡献指南](CONTRIBUTING_ZH.md)
- [问题支持](SUPPORT_ZH.md)
- [ESP Board Manager 文档](https://docs.espressif.com/projects/esp-board-manager/zh_CN/latest/index.html)

## 📄 许可证

采用 [Apache License 2.0](LICENSE)，导入文件保留原有 SPDX 声明，包括 CC0-1.0。
详见[来源说明](docs/BOARDS_ZH.md)。
