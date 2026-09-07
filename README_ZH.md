<div align="center">

<h1>Waveshare Boards</h1>

<p><strong>适用于 ESP Board Manager 的微雪开发板定义组件包</strong></p>

<a href="https://github.com/waveshareteam/waveshare-boards/actions/workflows/ci.yml"><img src="https://github.com/waveshareteam/waveshare-boards/actions/workflows/ci.yml/badge.svg" alt="Build"></a>
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
| [`esp32_s3_touch_lcd_7`](esp32_s3_touch_lcd_7/) | ESP32-S3 | `display_lcd`, `lcd_touch` | `i2c_master`, `uart_rs485`, `uart_external`, `adc_sensor`, `spi_sd` |
<!-- END SUPPORTED_BOARDS -->

板卡配置保留在现有目录。CI 校验并编译这些配置；电气行为和实机运行效果需要单独测试。

<a id="quick-start"></a>

## 🚀 快速开始

先激活 ESP-IDF 环境。在组件库版本发布之前，将本仓库克隆到**已有 ESP-IDF 应用**的组件目录：

```bash
mkdir -p components
git clone https://github.com/waveshareteam/waveshare-boards.git components/waveshare-boards
python -m pip install esp-bmgr-assist==0.8.3
idf.py bmgr -l
idf.py bmgr -b esp32_s3_touch_lcd_7
idf.py build
```

使用当前 IDF 环境配套的 Component Manager；各 IDF 版本线测试过的版本见 [CI 指南](docs/CI_ZH.md)。

本地目录名请使用 `waveshare-boards`，因为 ESP-IDF 使用目录名作为组件名。
组件包已经公开依赖 `espressif/esp_board_manager`；应用可以包含
`esp_board_manager.h`，并通过 `esp_board_manager_init()` 初始化所选板卡。

维护者正式发布组件后，可以通过组件库安装，替代本地克隆：

```bash
idf.py add-dependency "waveshare/waveshare-boards"
```

计划使用的组件库名称为 `waveshare/waveshare-boards`。仓库里的版本号或打包检查通过，
不代表该版本已经在组件库发布。

## 🗂️ 仓库结构

```text
esp32_s3_touch_lcd_7/    板卡信息、外设、设备与初始化代码
idf_component.yml      组件元数据与托管依赖
CMakeLists.txt         板卡包组件注册
ci/test_app/           Board Manager 集成编译测试
ci/scripts/            发现、变更路由与验证测试
ci/versions.json       明确的 ESP-IDF 与 Python 工具版本
scripts/               板卡目录生成维护
docs/                  CI 与组件库发布指南
.github/               工作流与贡献模板
```

集成测试位于 `ci/`。当前仓库没有第一方应用示例、Arduino 工程或交付固件。

## 🛠️ 维护

```bash
python scripts/update_supported_boards_table.py
python ci/scripts/board_pack.py check
python -m unittest discover -s ci/scripts -p 'test_*.py' -v
```

板卡目录最多可以嵌套三层。目录名必须与唯一的 `board` 字段一致。
生成的目录表使用实际设备和外设名称，并同步处理板卡删除或重命名。

## 📚 文档

- [CI 覆盖范围与本地验证](docs/CI_ZH.md)
- [组件库凭据与发布](docs/PUBLISHING_ZH.md)
- [贡献指南](CONTRIBUTING_ZH.md)
- [问题支持](SUPPORT_ZH.md)
- [ESP Board Manager 文档](https://docs.espressif.com/projects/esp-board-manager/zh_CN/latest/index.html)

## 📄 许可证

采用 [Apache License 2.0](LICENSE)。贡献或复用板卡定义与第三方代码时，请保留已有声明。
