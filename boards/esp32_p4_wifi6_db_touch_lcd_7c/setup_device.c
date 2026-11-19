/*
 * Board-specific factories for the ESP32-P4-WIFI6-DB 7C board.
 *
 * The weak symbols are the extension points used by ESP Board Manager 0.5.x.
 */

#include <stddef.h>

#include "esp_log.h"
#include "esp_board_manager_includes.h"

#if __has_include(<esp_lcd_ek79007.h>)
#define has_ek79007 1
#include "esp_lcd_ek79007.h"
#endif

#if __has_include(<esp_lcd_touch_gt911.h>)
#define has_gt911 1
#include "esp_lcd_touch_gt911.h"
#include "touch_gt911_config_update.h"
#endif

static const char *tag = "P4_WIFI6_DB_7C_SETUP";

#if defined(has_ek79007)
__attribute__((weak)) esp_err_t lcd_dsi_panel_factory_entry_t(
    esp_lcd_dsi_bus_handle_t dsi_handle,
    dev_display_lcd_config_t *lcd_cfg,
    dev_display_lcd_handles_t *lcd_handles)
{
    if (dsi_handle == NULL || lcd_cfg == NULL || lcd_handles == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = dsi_handle,
            .dpi_config = &lcd_cfg->sub_cfg.dsi.dpi_config,
            .lane_num = 2,
        },
    };

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = lcd_cfg->sub_cfg.dsi.reset_gpio_num,
        .rgb_ele_order = lcd_cfg->rgb_ele_order,
        .bits_per_pixel = lcd_cfg->bits_per_pixel,
        .data_endian = lcd_cfg->data_endian,
        .flags = {
            .reset_active_high = lcd_cfg->sub_cfg.dsi.reset_active_high,
        },
        .vendor_config = &vendor_config,
    };

    esp_err_t ret = esp_lcd_new_panel_ek79007(
        lcd_handles->io_handle,
        &panel_config,
        &lcd_handles->panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to create EK79007 panel: %s", esp_err_to_name(ret));
    }
    return ret;
}
#endif

#if defined(has_gt911)
__attribute__((weak)) esp_err_t lcd_touch_factory_entry_t(
    esp_lcd_panel_io_handle_t io,
    const esp_lcd_touch_config_t *touch_config,
    esp_lcd_touch_handle_t *touch_handle)
{
    if (io == NULL || touch_config == NULL || touch_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = touch_gt911_config_update(io, touch_config->x_max, touch_config->y_max,
                                              touch_config->rst_gpio_num, touch_config->int_gpio_num);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to update GT911 configuration: %s", esp_err_to_name(ret));
        return ret;
    }

    /* The controller was reset before the configuration update. */
    esp_lcd_touch_config_t gt911_touch_config = *touch_config;
    gt911_touch_config.rst_gpio_num = GPIO_NUM_NC;

    ret = esp_lcd_touch_new_i2c_gt911(io, &gt911_touch_config, touch_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to create GT911 touch driver: %s", esp_err_to_name(ret));
    }
    return ret;
}
#endif
