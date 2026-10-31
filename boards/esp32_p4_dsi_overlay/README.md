# Waveshare ESP32-P4 DSI Display Overlays

[中文](README_CN.md) | [Board definitions](../../README.md#board-catalog) | [Waveshare ESP32-P4 documentation](https://docs.waveshare.net/ESP32-P4)

This directory contains ESP Board Manager amend definitions for Waveshare DSI-TOUCH displays. Each subdirectory is one amend that updates a compatible base board's display configuration.

Use ESP Board Manager 0.7.1 or newer. For repository setup and base-board information, see the [root README](../../README.md).

## 🖥️ Supported Overlays

| Overlay | Product | Driver | Resolution | DSI bitrate | Initialization |
|---|---|---|---:|---:|---|
| `5_dsi_touch_a` | 5-DSI-TOUCH-A | `waveshare/esp_lcd_hx8394` | 720x1280 | 700 Mbps | Driver default |
| `7_dsi_touch_c` | 7-DSI-TOUCH-C | `espressif/esp_lcd_ek79007` | 1024x600 | 1000 Mbps | Driver default |
| `8_dsi_touch_a` | 8-DSI-TOUCH-A | `waveshare/esp_lcd_jd9365` | 800x1280 | 1500 Mbps | `Kconfig.projbuild` |
| `10_1_dsi_touch_a` | 10.1-DSI-TOUCH-A | `waveshare/esp_lcd_jd9365` | 800x1280 | 1500 Mbps | Driver default |
| `10_1_dsi_touch_b` | 10.1-DSI-TOUCH-B | `waveshare/esp_lcd_jd9365` | 720x1280 | 1200 Mbps | `Kconfig.projbuild` |

Panels that only fit one base board ship with that board instead of this directory. `esp32_p4_wifi6_touch_lcd_x` carries `lcd_7_dsi_touch_a` (ILI9881C, 720x1280, 1000 Mbps) and `lcd_8_dsi_touch_a` (JD9365, 800x1280, 1500 Mbps).

All panels use two DSI data lanes, RGB565, three DPI frame buffers, and a GT911 touch controller. Timing values follow the Waveshare BSPs in [Waveshare-ESP32-components](https://github.com/waveshareteam/Waveshare-ESP32-components).

Each overlay directory contains:

- `board_amend.yaml`: the amend manifest; its `apply:` list names the files to apply;
- `panel.yaml`: DSI lane bitrate, LCD driver dependency, DPI timing, and touch resolution;
- `lcd_mipi_dsi.c`: strong `lcd_dsi_panel_factory_entry_t` and `lcd_touch_factory_entry_t` implementations for the selected panel;
- `Kconfig.projbuild` on the JD9365 overlays that need a non-default initialization table.

## 🚀 Apply an Overlay

Choose an overlay that matches the display and a compatible base board. For example, apply the 7-DSI-TOUCH-C configuration to `esp32_p4_wifi6_db`:

    idf.py bmgr -b esp32_p4_wifi6_db -a ../esp32_p4_dsi_overlay/7_dsi_touch_c

If this repository is not discoverable as a project component, also pass its path with `-c`:

    idf.py bmgr -c /path/to/waveshare_boards -b esp32_p4_wifi6_db -a ../esp32_p4_dsi_overlay/7_dsi_touch_c

Relative `-a` paths are first resolved from the command's current working directory; if no amend manifest is found there, Board Manager also tries the selected board directory. That is how `../esp32_p4_dsi_overlay/<overlay>` reaches this directory from any base board, and how a board-local overlay such as `lcd_7_dsi_touch_a` is found. An absolute amend path also works. Re-run `idf.py bmgr` after changing the board or overlay. Board Manager generates `components/gen_bmgr_codes/` in the project and adds dependencies declared by the board and overlay YAML files to its generated `idf_component.yml`.

## ⚙️ LCD Configuration

LCD parameters are described in `panel.yaml`, including the DSI lane bitrate, driver dependency, DPI timing, and touch resolution. Each overlay's `lcd_mipi_dsi.c` supplies LCD and GT911 touch factory overrides, and `board_amend.yaml` applies that source.

The `waveshare/esp_lcd_jd9365` driver selects its vendor initialization table at compile time with `CONFIG_BSP_LCD_TYPE_*`. Its built-in default is the 10.1-DSI-TOUCH-A sequence. Overlays for 8-A and 10.1-B include a prompt-less `Kconfig.projbuild` that defaults the matching symbol to `y`. The 10.1-A overlay uses the driver's default table.

## 📝 Notes

- Apply an overlay only to a compatible base board; resolution, driver, and DSI bitrate are panel-specific.
- Do not use these definitions together with a Waveshare BSP component in the same project. The BSPs define the same `BSP_LCD_TYPE_*` Kconfig symbols and can override the JD9365 table selected by an overlay.
- These overlays describe the panel only. They do not configure the panel board's backlight controller.
- Check GPIOs, peripherals, and display timings against the target hardware revision before relying on them.
