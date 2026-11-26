/* SPDX-License-Identifier: Apache-2.0 */

#include "../../include/waveshare_amoled_2_06_display.h"
#include "../../include/waveshare_amoled_2_06_touch.h"

esp_err_t esp_board_runtime_pre_init(void)
{
    return waveshare_amoled_2_06_prepare_touch();
}

esp_err_t lcd_panel_factory_entry_t(esp_lcd_panel_io_handle_t io,
                                    const esp_lcd_panel_dev_config_t *config,
                                    esp_lcd_panel_handle_t *ret_panel)
{
    return waveshare_amoled_2_06_new_panel(io, config, ret_panel);
}

esp_err_t lcd_touch_factory_entry_t(esp_lcd_panel_io_handle_t io,
                                    const esp_lcd_touch_config_t *config,
                                    esp_lcd_touch_handle_t *ret_touch)
{
    return waveshare_amoled_2_06_new_touch(io, config, ret_touch);
}
