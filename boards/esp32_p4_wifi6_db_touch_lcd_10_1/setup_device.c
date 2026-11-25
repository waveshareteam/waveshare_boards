/*
 * ESP32-P4-WIFI6-DB-Touch-10.1 JD9365 MIPI-DSI panel factory.
 *
 * This is intentionally kept aligned with the existing
 * esp32_p4_wifi6_db board factory. Hardware-specific GPIOs and timings are
 * described by board_devices.yaml.
 */

#include "esp_log.h"
#include "esp_board_manager_includes.h"
#if __has_include(<esp_lcd_jd9365_10_1.h>)
#define HAS_JD9365 1
#include "esp_lcd_jd9365_10_1.h"
#endif
#include "esp_lcd_touch_gt911.h"
#include <sys/cdefs.h>

static const char *TAG = "ESP32_P4_WIFI6_DB_TOUCH_10_1_LCD";

__attribute__((weak)) esp_err_t lcd_touch_factory_entry_t(
    const esp_lcd_panel_io_handle_t io,
    const esp_lcd_touch_config_t *config,
    esp_lcd_touch_handle_t *touch_handle)
{
    if (io == NULL || config == NULL || touch_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_lcd_touch_new_i2c_gt911(io, config, touch_handle);
}

#if defined(HAS_JD9365)
__attribute__((weak)) esp_err_t lcd_dsi_panel_factory_entry_t(
    esp_lcd_dsi_bus_handle_t dsi_handle,
    dev_display_lcd_config_t *lcd_cfg,
    dev_display_lcd_handles_t *lcd_handles)
{
    if (dsi_handle == NULL || lcd_cfg == NULL || lcd_handles == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    jd9365_vendor_config_t vendor_config = {
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

    // The JD9365 driver programs the 0x45 backlight controller over I2C1
    // through i2c_bus, which reuses an existing bus but would otherwise create
    // its own and make a later i2c_master peripheral init fail. Hold a
    // reference while the panel is created so the Board Manager bus exists
    // first; the driver only touches I2C during creation.
    void *i2c_bus = NULL;
    esp_err_t ret = esp_board_periph_ref_handle(ESP_BOARD_PERIPH_NAME_I2C_MASTER, &i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get i2c_master peripheral handle: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_lcd_new_panel_jd9365(
        lcd_handles->io_handle,
        &panel_config,
        &lcd_handles->panel_handle);
    esp_board_periph_unref_handle(ESP_BOARD_PERIPH_NAME_I2C_MASTER);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create JD9365 panel: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}
#endif
