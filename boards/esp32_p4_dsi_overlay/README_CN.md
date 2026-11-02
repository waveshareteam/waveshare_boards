# Waveshare ESP32-P4 DSI 显示屏 Overlay

[English](README.md) | [板卡定义](../../README_ZH.md#board-catalog) | [Waveshare ESP32-P4 产品文档](https://docs.waveshare.net/ESP32-P4)

本目录包含 Waveshare DSI-TOUCH 显示屏的 ESP Board Manager amend 定义。每个子目录对应一个 overlay，用于更新兼容基础板的显示配置。

请使用 ESP Board Manager 0.7.1 或更高版本。仓库接入方式和基础板信息请见[根目录 README](../../README_ZH.md)。

## 🖥️ 支持的 Overlay

| Overlay | 产品 | 驱动 | 分辨率 | DSI 比特率 | 初始化配置 |
|---|---|---|---:|---:|---|
| `5_dsi_touch_a` | 5-DSI-TOUCH-A | `waveshare/esp_lcd_hx8394` | 720x1280 | 700 Mbps | 驱动默认 |
| `7_dsi_touch_c` | 7-DSI-TOUCH-C | `espressif/esp_lcd_ek79007` | 1024x600 | 1000 Mbps | 驱动默认 |
| `8_dsi_touch_a` | 8-DSI-TOUCH-A | `waveshare/esp_lcd_jd9365` | 800x1280 | 1500 Mbps | `Kconfig.projbuild` |
| `10_1_dsi_touch_a` | 10.1-DSI-TOUCH-A | `waveshare/esp_lcd_jd9365` | 800x1280 | 1500 Mbps | 驱动默认 |
| `10_1_dsi_touch_b` | 10.1-DSI-TOUCH-B | `waveshare/esp_lcd_jd9365` | 720x1280 | 1200 Mbps | `Kconfig.projbuild` |

只适用于单一基础板的屏幕随该板卡目录提供，不放在本目录。`esp32_p4_wifi6_touch_lcd_x` 自带 `lcd_7_dsi_touch_a`（ILI9881C，720x1280，1000 Mbps）和 `lcd_8_dsi_touch_a`（JD9365，800x1280，1500 Mbps）。

所有屏幕均使用 2 条 DSI 数据 lane、RGB565、3 个 DPI frame buffer，触摸芯片为 GT911。时序与 [Waveshare-ESP32-components](https://github.com/waveshareteam/Waveshare-ESP32-components) 中的 Waveshare BSP 一致。

每个 overlay 目录包含：

- `board_amend.yaml`：amend 清单，`apply:` 列出需要应用的文件；
- `panel.yaml`：DSI lane 比特率、LCD 驱动依赖、DPI 时序和触摸分辨率；
- `lcd_mipi_dsi.c`：所选面板对应的 `lcd_dsi_panel_factory_entry_t` 和 `lcd_touch_factory_entry_t` 强符号实现；
- 需要非默认 JD9365 初始化表的 overlay 会包含 `Kconfig.projbuild`。

## 🚀 应用 Overlay

选择与显示屏匹配的 overlay 和兼容基础板。例如，将 7-DSI-TOUCH-C 配置应用到 `esp32_p4_wifi6_db`：

    idf.py bmgr -b esp32_p4_wifi6_db -a ../esp32_p4_dsi_overlay/7_dsi_touch_c

如果本仓库没有作为工程组件被发现，还需要通过 `-c` 传入仓库路径：

    idf.py bmgr -c /path/to/waveshare_boards -b esp32_p4_wifi6_db -a ../esp32_p4_dsi_overlay/7_dsi_touch_c

相对 `-a` 路径会先相对于命令当前工作目录解析；如果那里没有找到 amend 清单，Board Manager 还会尝试所选板卡目录。因此任意基础板都可以通过 `../esp32_p4_dsi_overlay/<overlay>` 找到本目录，`lcd_7_dsi_touch_a` 这类板卡自带 overlay 也是这样被找到的。也可以使用绝对路径。更换板卡或 overlay 后请重新执行 `idf.py bmgr`。Board Manager 会在工程中生成 `components/gen_bmgr_codes/`，并将板卡和 overlay YAML 中声明的依赖写入生成的 `idf_component.yml`。

## ⚙️ LCD 配置

LCD 参数在 `panel.yaml` 中描述，包括 DSI lane 比特率、驱动依赖、DPI 时序和触摸分辨率。每个 overlay 的 `lcd_mipi_dsi.c` 提供 LCD 和 GT911 触摸 factory 强符号实现，并由 `board_amend.yaml` 应用。

`waveshare/esp_lcd_jd9365` 驱动在编译期通过 `CONFIG_BSP_LCD_TYPE_*` 选择厂商初始化表，内置默认表对应 10.1-DSI-TOUCH-A。因此，8-A 和 10.1-B overlay 会包含一个无 prompt、默认值为 `y` 的 `Kconfig.projbuild`，用于选择匹配的初始化表。10.1-A overlay 使用驱动默认表。

## 📝 注意事项

- overlay 只能应用到兼容的基础板；分辨率、驱动和 DSI 比特率均为面板专用。
- 不要在同一工程中同时使用本仓库定义和 Waveshare BSP 组件。BSP 中的 `BSP_LCD_TYPE_*` Kconfig 符号同名，可能覆盖 overlay 对 JD9365 初始化表的选择。
- overlay 只描述面板本身，不配置屏板上的背光控制器。
- 使用前请对照目标硬件版本核对 GPIO、外设和显示时序。
