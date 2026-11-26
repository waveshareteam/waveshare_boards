/* SPDX-License-Identifier: Apache-2.0 */
#pragma once

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_sh8601.h"

/* The 2.06-inch CO5300 panels use the SH8601 driver and the product BSP initialization sequence. */
static const sh8601_lcd_init_cmd_t waveshare_amoled_2_06_init_cmds[] = {
    {0x11, NULL, 0, 120},
    {0xC4, (const uint8_t[]){0x80}, 1, 0},
    {0x44, (const uint8_t[]){0x01, 0xD1}, 2, 0},
    {0x35, (const uint8_t[]){0x00}, 1, 0},
    {0x53, (const uint8_t[]){0x20}, 1, 10},
    {0x63, (const uint8_t[]){0xFF}, 1, 10},
    {0x51, (const uint8_t[]){0x00}, 1, 10},
    {0x2A, (const uint8_t[]){0x00, 0x16, 0x01, 0xAF}, 4, 0},
    {0x2B, (const uint8_t[]){0x00, 0x00, 0x01, 0xF5}, 4, 0},
    {0x29, NULL, 0, 10},
    {0x51, (const uint8_t[]){0xFF}, 1, 0},
};

static inline esp_err_t waveshare_amoled_2_06_new_panel(
    esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *config,
    esp_lcd_panel_handle_t *ret_panel)
{
    sh8601_vendor_config_t vendor = {
        .init_cmds = waveshare_amoled_2_06_init_cmds,
        .init_cmds_size = sizeof(waveshare_amoled_2_06_init_cmds) / sizeof(waveshare_amoled_2_06_init_cmds[0]),
        .flags.use_qspi_interface = 1,
    };
    esp_lcd_panel_dev_config_t panel_config = *config;
    panel_config.vendor_config = &vendor;
    esp_lcd_panel_handle_t panel;
    esp_err_t ret = esp_lcd_new_panel_sh8601(io, &panel_config, &panel);
    if (ret != ESP_OK) {
        return ret;
    }
    /* The visible columns start at column 22 in panel RAM. */
    ret = esp_lcd_panel_set_gap(panel, 22, 0);
    if (ret != ESP_OK) {
        esp_lcd_panel_del(panel);
        return ret;
    }
    *ret_panel = panel;
    return ESP_OK;
}
