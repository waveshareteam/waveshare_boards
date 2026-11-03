#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_board_manager_includes.h"
#include "esp_lcd_touch_gt911.h"

#include "esp_lcd_hx8394.h"

static const char *TAG = "WAVESHARE_DISPLAY_DSI";

esp_err_t lcd_dsi_panel_factory_entry_t(
    esp_lcd_dsi_bus_handle_t dsi_handle,
    dev_display_lcd_config_t *lcd_cfg,
    dev_display_lcd_handles_t *lcd_handles)
{
    ESP_LOGI(TAG, "Strong LCD factory override is active (waveshare_lcd)");

    if (dsi_handle == NULL || lcd_cfg == NULL || lcd_handles == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_lcd_dpi_panel_config_t dpi_config = lcd_cfg->sub_cfg.dsi.dpi_config;
    hx8394_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = dsi_handle,
            .dpi_config = &dpi_config,
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

    // The panel driver programs the 0x45 backlight controller over I2C1 through
    // i2c_bus, which reuses an existing bus but would otherwise create its own
    // and make a later i2c_master peripheral init fail. Hold a reference while
    // the panel is created so the Board Manager bus exists first; the driver
    // only touches I2C during creation.
    void *i2c_bus = NULL;
    esp_err_t ret = esp_board_periph_ref_handle(ESP_BOARD_PERIPH_NAME_I2C_MASTER, &i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get i2c_master peripheral handle: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_lcd_new_panel_hx8394(
        lcd_handles->io_handle,
        &panel_config,
        &lcd_handles->panel_handle);
    esp_board_periph_unref_handle(ESP_BOARD_PERIPH_NAME_I2C_MASTER);
    return ret;
}

// Strong override for the GT911 touch configuration used by this amend.
esp_err_t lcd_touch_factory_entry_t(
    const esp_lcd_panel_io_handle_t io,
    const esp_lcd_touch_config_t *config,
    esp_lcd_touch_handle_t *touch_handle)
{
    if (io == NULL || config == NULL || touch_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_lcd_touch_new_i2c_gt911(io, config, touch_handle);
}
