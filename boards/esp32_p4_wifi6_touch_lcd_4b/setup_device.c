#include "esp_board_manager_includes.h"

#if __has_include(<esp_lcd_st7703.h>)
#include "esp_lcd_st7703.h"

__attribute__((weak)) esp_err_t lcd_dsi_panel_factory_entry_t(esp_lcd_dsi_bus_handle_t dsi_handle, dev_display_lcd_config_t *lcd_cfg, dev_display_lcd_handles_t *lcd_handles)
{
    /* init_cmds stays NULL so the driver applies vendor_specific_init_default. */
    st7703_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = dsi_handle,
            .dpi_config = &lcd_cfg->sub_cfg.dsi.dpi_config,
        },
        .flags = {
            .use_mipi_interface = 1,
        },
    };
    esp_lcd_panel_dev_config_t config = {
        .reset_gpio_num = lcd_cfg->sub_cfg.dsi.reset_gpio_num,
        .rgb_ele_order = lcd_cfg->rgb_ele_order,
        .bits_per_pixel = lcd_cfg->bits_per_pixel,
        .data_endian = lcd_cfg->data_endian,
        .flags = {
            .reset_active_high = lcd_cfg->sub_cfg.dsi.reset_active_high,
        },
        .vendor_config = &vendor_config,
    };
    return esp_lcd_new_panel_st7703(lcd_handles->io_handle, &config, &lcd_handles->panel_handle);
}
#endif

#if __has_include(<esp_lcd_touch_gt911.h>)
#include "esp_lcd_touch_gt911.h"
__attribute__((weak)) esp_err_t lcd_touch_factory_entry_t(esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *cfg, esp_lcd_touch_handle_t *touch)
{
    return esp_lcd_touch_new_i2c_gt911(io, cfg, touch);
}
#endif
