#include "esp_log.h"
#include "esp_err.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_board_manager_includes.h"

#include "esp_lcd_ili9881c.h"

static const char *TAG = "WAVESHARE_DISPLAY_DSI";

static const ili9881c_lcd_init_cmd_t lcd_init_cmds[] = {
    // {cmd, { data }, data_size, delay_ms}
    /**** CMD_Page 3 ****/
    {0xFF, (uint8_t[]){0x98, 0x81, 0x03}, 3, 0},
    // {0x01, (uint8_t []){0x00}, 1, 0},
    {0x01, (uint8_t[]){0x00}, 1, 0},
    {0x02, (uint8_t[]){0x00}, 1, 0},
    {0x03, (uint8_t[]){0x73}, 1, 0},
    {0x04, (uint8_t[]){0x00}, 1, 0},
    {0x05, (uint8_t[]){0x00}, 1, 0},
    {0x06, (uint8_t[]){0x0A}, 1, 0},
    {0x07, (uint8_t[]){0x00}, 1, 0},
    {0x08, (uint8_t[]){0x00}, 1, 0},
    {0x09, (uint8_t[]){0x61}, 1, 0},
    {0x0A, (uint8_t[]){0x00}, 1, 0},
    {0x0B, (uint8_t[]){0x00}, 1, 0},
    {0x0C, (uint8_t[]){0x01}, 1, 0},
    {0x0D, (uint8_t[]){0x00}, 1, 0},
    {0x0E, (uint8_t[]){0x00}, 1, 0},
    {0x0F, (uint8_t[]){0x61}, 1, 0},
    {0x10, (uint8_t[]){0x61}, 1, 0},
    {0x11, (uint8_t[]){0x00}, 1, 0},
    {0x12, (uint8_t[]){0x00}, 1, 0},
    {0x13, (uint8_t[]){0x00}, 1, 0},
    {0x14, (uint8_t[]){0x00}, 1, 0},
    {0x15, (uint8_t[]){0x00}, 1, 0},
    {0x16, (uint8_t[]){0x00}, 1, 0},
    {0x17, (uint8_t[]){0x00}, 1, 0},
    {0x18, (uint8_t[]){0x00}, 1, 0},
    {0x19, (uint8_t[]){0x00}, 1, 0},
    {0x1A, (uint8_t[]){0x00}, 1, 0},
    {0x1B, (uint8_t[]){0x00}, 1, 0},
    {0x1C, (uint8_t[]){0x00}, 1, 0},
    {0x1D, (uint8_t[]){0x00}, 1, 0},
    {0x1E, (uint8_t[]){0x40}, 1, 0},
    {0x1F, (uint8_t[]){0x80}, 1, 0},
    {0x20, (uint8_t[]){0x06}, 1, 0},
    {0x21, (uint8_t[]){0x01}, 1, 0},
    {0x22, (uint8_t[]){0x00}, 1, 0},
    {0x23, (uint8_t[]){0x00}, 1, 0},
    {0x24, (uint8_t[]){0x00}, 1, 0},
    {0x25, (uint8_t[]){0x00}, 1, 0},
    {0x26, (uint8_t[]){0x00}, 1, 0},
    {0x27, (uint8_t[]){0x00}, 1, 0},
    {0x28, (uint8_t[]){0x33}, 1, 0},
    {0x29, (uint8_t[]){0x03}, 1, 0},
    {0x2A, (uint8_t[]){0x00}, 1, 0},
    {0x2B, (uint8_t[]){0x00}, 1, 0},
    {0x2C, (uint8_t[]){0x00}, 1, 0},
    {0x2D, (uint8_t[]){0x00}, 1, 0},
    {0x2E, (uint8_t[]){0x00}, 1, 0},
    {0x2F, (uint8_t[]){0x00}, 1, 0},
    {0x30, (uint8_t[]){0x00}, 1, 0},
    {0x31, (uint8_t[]){0x00}, 1, 0},
    {0x32, (uint8_t[]){0x00}, 1, 0},
    {0x33, (uint8_t[]){0x00}, 1, 0},
    {0x34, (uint8_t[]){0x04}, 1, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x36, (uint8_t[]){0x00}, 1, 0},
    {0x37, (uint8_t[]){0x00}, 1, 0},
    {0x38, (uint8_t[]){0x3C}, 1, 0},
    {0x39, (uint8_t[]){0x00}, 1, 0},
    {0x3A, (uint8_t[]){0x00}, 1, 0},
    {0x3B, (uint8_t[]){0x00}, 1, 0},
    {0x3C, (uint8_t[]){0x00}, 1, 0},
    {0x3D, (uint8_t[]){0x00}, 1, 0},
    {0x3E, (uint8_t[]){0x00}, 1, 0},
    {0x3F, (uint8_t[]){0x00}, 1, 0},
    {0x40, (uint8_t[]){0x00}, 1, 0},
    {0x41, (uint8_t[]){0x00}, 1, 0},
    {0x42, (uint8_t[]){0x00}, 1, 0},
    {0x43, (uint8_t[]){0x00}, 1, 0},
    {0x44, (uint8_t[]){0x00}, 1, 0},
    {0x50, (uint8_t[]){0x10}, 1, 0},
    {0x51, (uint8_t[]){0x32}, 1, 0},
    {0x52, (uint8_t[]){0x54}, 1, 0},
    {0x53, (uint8_t[]){0x76}, 1, 0},
    {0x54, (uint8_t[]){0x98}, 1, 0},
    {0x55, (uint8_t[]){0xBA}, 1, 0},
    {0x56, (uint8_t[]){0x10}, 1, 0},
    {0x57, (uint8_t[]){0x32}, 1, 0},
    {0x58, (uint8_t[]){0x54}, 1, 0},
    {0x59, (uint8_t[]){0x76}, 1, 0},
    {0x5A, (uint8_t[]){0x98}, 1, 0},
    {0x5B, (uint8_t[]){0xBA}, 1, 0},
    {0x5C, (uint8_t[]){0xDC}, 1, 0},
    {0x5D, (uint8_t[]){0xFE}, 1, 0},
    {0x5E, (uint8_t[]){0x00}, 1, 0},
    {0x5F, (uint8_t[]){0x0E}, 1, 0},
    {0x60, (uint8_t[]){0x0F}, 1, 0},
    {0x61, (uint8_t[]){0x0C}, 1, 0},
    {0x62, (uint8_t[]){0x0D}, 1, 0},
    {0x63, (uint8_t[]){0x06}, 1, 0},
    {0x64, (uint8_t[]){0x07}, 1, 0},
    {0x65, (uint8_t[]){0x02}, 1, 0},
    {0x66, (uint8_t[]){0x02}, 1, 0},
    {0x67, (uint8_t[]){0x02}, 1, 0},
    {0x68, (uint8_t[]){0x02}, 1, 0},
    {0x69, (uint8_t[]){0x01}, 1, 0},
    {0x6A, (uint8_t[]){0x00}, 1, 0},
    {0x6B, (uint8_t[]){0x02}, 1, 0},
    {0x6C, (uint8_t[]){0x15}, 1, 0},
    {0x6D, (uint8_t[]){0x14}, 1, 0},
    {0x6E, (uint8_t[]){0x02}, 1, 0},
    {0x6F, (uint8_t[]){0x02}, 1, 0},
    {0x70, (uint8_t[]){0x02}, 1, 0},
    {0x71, (uint8_t[]){0x02}, 1, 0},
    {0x72, (uint8_t[]){0x02}, 1, 0},
    {0x73, (uint8_t[]){0x02}, 1, 0},
    {0x74, (uint8_t[]){0x02}, 1, 0},
    {0x75, (uint8_t[]){0x0E}, 1, 0},
    {0x76, (uint8_t[]){0x0F}, 1, 0},
    {0x77, (uint8_t[]){0x0C}, 1, 0},
    {0x78, (uint8_t[]){0x0D}, 1, 0},
    {0x79, (uint8_t[]){0x06}, 1, 0},
    {0x7A, (uint8_t[]){0x07}, 1, 0},
    {0x7B, (uint8_t[]){0x02}, 1, 0},
    {0x7C, (uint8_t[]){0x02}, 1, 0},
    {0x7D, (uint8_t[]){0x02}, 1, 0},
    {0x7E, (uint8_t[]){0x02}, 1, 0},
    {0x7F, (uint8_t[]){0x01}, 1, 0},
    {0x80, (uint8_t[]){0x00}, 1, 0},
    {0x81, (uint8_t[]){0x02}, 1, 0},
    {0x82, (uint8_t[]){0x14}, 1, 0},
    {0x83, (uint8_t[]){0x15}, 1, 0},
    {0x84, (uint8_t[]){0x02}, 1, 0},
    {0x85, (uint8_t[]){0x02}, 1, 0},
    {0x86, (uint8_t[]){0x02}, 1, 0},
    {0x87, (uint8_t[]){0x02}, 1, 0},
    {0x88, (uint8_t[]){0x02}, 1, 0},
    {0x89, (uint8_t[]){0x02}, 1, 0},
    {0x8A, (uint8_t[]){0x02}, 1, 0},

    {0xFF, (uint8_t[]){0x98, 0x81, 0x04}, 3, 0},
    {0x38, (uint8_t[]){0x01}, 1, 0},
    {0x39, (uint8_t[]){0x00}, 1, 0},
    {0x6C, (uint8_t[]){0x15}, 1, 0},
    {0x6E, (uint8_t[]){0x2A}, 1, 0},
    {0x6F, (uint8_t[]){0x33}, 1, 0},
    {0x3A, (uint8_t[]){0x94}, 1, 0},
    {0x8D, (uint8_t[]){0x14}, 1, 0},
    {0x87, (uint8_t[]){0xBA}, 1, 0},
    {0x26, (uint8_t[]){0x76}, 1, 0},
    {0xB2, (uint8_t[]){0xD1}, 1, 0},
    {0xB5, (uint8_t[]){0x06}, 1, 0},
    {0X3B, (uint8_t[]){0X98}, 1, 0},
    {0xFF, (uint8_t[]){0x98, 0x81, 0x01}, 3, 0},
    {0x22, (uint8_t[]){0x0A}, 1, 0},
    {0x31, (uint8_t[]){0x00}, 1, 0},
    {0x53, (uint8_t[]){0x71}, 1, 0},
    {0x55, (uint8_t[]){0x8F}, 1, 0},
    {0x40, (uint8_t[]){0x33}, 1, 0},
    {0x50, (uint8_t[]){0x96}, 1, 0},
    {0x51, (uint8_t[]){0x96}, 1, 0},
    {0x60, (uint8_t[]){0x23}, 1, 0},
    {0xA0, (uint8_t[]){0x08}, 1, 0},
    {0xA1, (uint8_t[]){0x1D}, 1, 0},
    {0xA2, (uint8_t[]){0x2A}, 1, 0},
    {0xA3, (uint8_t[]){0x10}, 1, 0},
    {0xA4, (uint8_t[]){0x15}, 1, 0},
    {0xA5, (uint8_t[]){0x28}, 1, 0},
    {0xA6, (uint8_t[]){0x1C}, 1, 0},
    {0xA7, (uint8_t[]){0x1D}, 1, 0},
    {0xA8, (uint8_t[]){0x7E}, 1, 0},
    {0xA9, (uint8_t[]){0x1D}, 1, 0},
    {0xAA, (uint8_t[]){0x29}, 1, 0},
    {0xAB, (uint8_t[]){0x6B}, 1, 0},
    {0xAC, (uint8_t[]){0x1A}, 1, 0},
    {0xAD, (uint8_t[]){0x18}, 1, 0},
    {0xAE, (uint8_t[]){0x4B}, 1, 0},
    {0xAF, (uint8_t[]){0x20}, 1, 0},
    {0xB0, (uint8_t[]){0x27}, 1, 0},
    {0xB1, (uint8_t[]){0x50}, 1, 0},
    {0xB2, (uint8_t[]){0x64}, 1, 0},
    {0xB3, (uint8_t[]){0x39}, 1, 0},
    {0xC0, (uint8_t[]){0x08}, 1, 0},
    {0xC1, (uint8_t[]){0x1D}, 1, 0},
    {0xC2, (uint8_t[]){0x2A}, 1, 0},
    {0xC3, (uint8_t[]){0x10}, 1, 0},
    {0xC4, (uint8_t[]){0x15}, 1, 0},
    {0xC5, (uint8_t[]){0x28}, 1, 0},
    {0xC6, (uint8_t[]){0x1C}, 1, 0},
    {0xC7, (uint8_t[]){0x1D}, 1, 0},
    {0xC8, (uint8_t[]){0x7E}, 1, 0},
    {0xC9, (uint8_t[]){0x1D}, 1, 0},
    {0xCA, (uint8_t[]){0x29}, 1, 0},
    {0xCB, (uint8_t[]){0x6B}, 1, 0},
    {0xCC, (uint8_t[]){0x1A}, 1, 0},
    {0xCD, (uint8_t[]){0x18}, 1, 0},
    {0xCE, (uint8_t[]){0x4B}, 1, 0},
    {0xCF, (uint8_t[]){0x20}, 1, 0},
    {0xD0, (uint8_t[]){0x27}, 1, 0},
    {0xD1, (uint8_t[]){0x50}, 1, 0},
    {0xD2, (uint8_t[]){0x64}, 1, 0},
    {0xD3, (uint8_t[]){0x39}, 1, 0},

    {0xFF, (uint8_t[]){0x98, 0x81, 0x00}, 3, 0},
    {0x3A, (uint8_t[]){0x77}, 1, 0},
    {0x36, (uint8_t[]){0x00}, 1, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x11, (uint8_t[]){0x00}, 0, 150},

    {0x29, (uint8_t[]){0x00}, 0, 20},

    //============ Gamma END===========
};

// Strong override of the weak 10.1-DSI-TOUCH-A factory in setup_device.c.
esp_err_t lcd_dsi_panel_factory_entry_t(
    esp_lcd_dsi_bus_handle_t dsi_handle,
    dev_display_lcd_config_t *lcd_cfg,
    dev_display_lcd_handles_t *lcd_handles)
{
    ESP_LOGI(TAG, "Install Waveshare 7-DSI-TOUCH-A panel (ILI9881C)");

    if (dsi_handle == NULL || lcd_cfg == NULL || lcd_handles == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ili9881c_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
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

    /*
     * esp_lcd_new_panel_ili9881c() talks to the panel MCU at 0x45 through i2c_bus_create(I2C_NUM_1) and never
     * frees that bus, so the board's i2c_master on the same port would fail to start later. Bring i2c_master
     * up first so i2c_bus reuses its handle; keep the reference for good, as board_display_start() does.
     */
    void *i2c_master = NULL;
    esp_err_t ret = esp_board_periph_ref_handle(ESP_BOARD_PERIPH_NAME_I2C_MASTER, &i2c_master);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Initialize i2c_master failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_lcd_new_panel_ili9881c(lcd_handles->io_handle, &panel_config, &lcd_handles->panel_handle);
    if (ret != ESP_OK) {
        esp_board_periph_unref_handle(ESP_BOARD_PERIPH_NAME_I2C_MASTER);
    }
    return ret;
}
