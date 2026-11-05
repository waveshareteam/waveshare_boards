/*
 * SPDX-FileCopyrightText: 2026 Waveshare
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_board_manager_includes.h"
#include "esp_log.h"
#include "esp_lcd_jd9365.h"
#include "esp_lcd_st7703.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "jd9365_720_720_init.h"
#include "lcd_panel_probe.h"

static const char *TAG = "ESP32_P4_4B_LCD";
static waveshare_lcd_panel_kind_t s_panel_kind = WAVESHARE_LCD_PANEL_UNKNOWN;
static bool s_panel_identified;
static esp_lcd_dpi_panel_config_t s_effective_dpi_config;
static waveshare_lcd_panel_timing_t s_effective_timing;
static bool s_effective_timing_valid;

waveshare_lcd_panel_kind_t waveshare_lcd_panel_get_kind(void)
{
    return s_panel_kind;
}

bool waveshare_lcd_panel_is_identified(void)
{
    return s_panel_identified;
}

bool waveshare_lcd_panel_get_effective_timing(waveshare_lcd_panel_timing_t *timing)
{
    if (timing == NULL || !s_effective_timing_valid) {
        return false;
    }
    *timing = s_effective_timing;
    return true;
}

/* The Board Manager creates the DBI IO before calling this hook, while it
 * resets the resulting panel only after the hook returns. A controller ID
 * cannot be read reliably until the physical reset has completed. */
static esp_err_t reset_panel_for_probe(const dev_display_lcd_config_t *lcd_cfg)
{
    const int reset_gpio = lcd_cfg->sub_cfg.dsi.reset_gpio_num;
    if (reset_gpio < 0) {
        return ESP_ERR_INVALID_ARG;
    }
    const int active = lcd_cfg->sub_cfg.dsi.reset_active_high ? 1 : 0;
    gpio_config_t reset_config = {
        .pin_bit_mask = 1ULL << reset_gpio,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_BOARD_RETURN_ON_ERROR(gpio_config(&reset_config), TAG, "Configure panel reset GPIO failed");
    ESP_BOARD_RETURN_ON_ERROR(gpio_set_level(reset_gpio, !active), TAG, "Deassert panel reset failed");
    vTaskDelay(pdMS_TO_TICKS(5));
    ESP_BOARD_RETURN_ON_ERROR(gpio_set_level(reset_gpio, active), TAG, "Assert panel reset failed");
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_BOARD_RETURN_ON_ERROR(gpio_set_level(reset_gpio, !active), TAG, "Release panel reset failed");
    vTaskDelay(pdMS_TO_TICKS(120));
    return ESP_OK;
}

static void set_jd9365_720_square_timing(esp_lcd_dpi_panel_config_t *dpi_config)
{
    /* 4-inch 720x720 JD9365DA panel used by the local factory BSP. */
    /* ESP32-P4 can generate 240/7 MHz exactly; requesting 36 MHz causes the
     * IDF DSI timing compensation to make the front porch invalid. */
    dpi_config->dpi_clock_freq_mhz = 240.0f / 7.0f;
    dpi_config->video_timing.hsync_back_porch = 30;
    dpi_config->video_timing.hsync_pulse_width = 24;
    dpi_config->video_timing.hsync_front_porch = 30;
    dpi_config->video_timing.vsync_back_porch = 16;
    dpi_config->video_timing.vsync_pulse_width = 2;
    dpi_config->video_timing.vsync_front_porch = 12;
}

static void set_effective_timing(const esp_lcd_dpi_panel_config_t *dpi_config)
{
    s_effective_timing = (waveshare_lcd_panel_timing_t){
        .pixel_clock_mhz = dpi_config->dpi_clock_freq_mhz,
        .hsync_back_porch = dpi_config->video_timing.hsync_back_porch,
        .hsync_pulse_width = dpi_config->video_timing.hsync_pulse_width,
        .hsync_front_porch = dpi_config->video_timing.hsync_front_porch,
        .vsync_back_porch = dpi_config->video_timing.vsync_back_porch,
        .vsync_pulse_width = dpi_config->video_timing.vsync_pulse_width,
        .vsync_front_porch = dpi_config->video_timing.vsync_front_porch,
    };
    s_effective_timing_valid = true;
}

static esp_lcd_panel_dev_config_t panel_config_from_lcd_cfg(const dev_display_lcd_config_t *lcd_cfg,
                                                             void *vendor_config)
{
    return (esp_lcd_panel_dev_config_t){
        .reset_gpio_num = lcd_cfg->sub_cfg.dsi.reset_gpio_num,
        .rgb_ele_order = lcd_cfg->rgb_ele_order,
        .bits_per_pixel = lcd_cfg->bits_per_pixel,
        .data_endian = lcd_cfg->data_endian,
        .flags = {
            .reset_active_high = lcd_cfg->sub_cfg.dsi.reset_active_high,
        },
        .vendor_config = vendor_config,
    };
}

/* Board Manager calls this hook after creating the DSI bus and DBI IO. */
__attribute__((weak)) esp_err_t lcd_dsi_panel_factory_entry_t(
    esp_lcd_dsi_bus_handle_t dsi_handle,
    dev_display_lcd_config_t *lcd_cfg,
    dev_display_lcd_handles_t *lcd_handles)
{
    if (dsi_handle == NULL || lcd_cfg == NULL || lcd_handles == NULL ||
        lcd_handles->io_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BOARD_RETURN_ON_ERROR(reset_panel_for_probe(lcd_cfg), TAG, "Reset panel for probe failed");
    s_panel_kind = WAVESHARE_LCD_PANEL_UNKNOWN;
    s_panel_identified = false;
    esp_err_t ret = waveshare_lcd_panel_probe(lcd_handles->io_handle, &s_panel_kind);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Panel identification failed: %s", esp_err_to_name(ret));
        return ret;
    }
    s_panel_identified = true;
    s_effective_dpi_config = lcd_cfg->sub_cfg.dsi.dpi_config;

    if (s_panel_kind == WAVESHARE_LCD_PANEL_JD9365) {
        set_jd9365_720_square_timing(&s_effective_dpi_config);
        set_effective_timing(&s_effective_dpi_config);
        jd9365_vendor_config_t vendor_config = {
            .init_cmds = waveshare_jd9365_720_720_init_cmds,
            .init_cmds_size = sizeof(waveshare_jd9365_720_720_init_cmds) /
                              sizeof(waveshare_jd9365_720_720_init_cmds[0]),
            .mipi_config = {
                .dsi_bus = dsi_handle,
                .dpi_config = &s_effective_dpi_config,
                .lane_num = 2,
            },
        };
        esp_lcd_panel_dev_config_t config = panel_config_from_lcd_cfg(lcd_cfg, &vendor_config);
        ESP_LOGI(TAG, "Detected JD9365DA; selecting 8904ANT 720x720 initialization (%u commands)",
                 (unsigned)vendor_config.init_cmds_size);
        return esp_lcd_new_panel_jd9365(lcd_handles->io_handle, &config,
                                        &lcd_handles->panel_handle);
    }

    if (s_panel_kind == WAVESHARE_LCD_PANEL_ST7703) {
        set_effective_timing(&s_effective_dpi_config);
        st7703_vendor_config_t vendor_config = {
            .mipi_config = {
                .dsi_bus = dsi_handle,
                .dpi_config = &s_effective_dpi_config,
            },
            .flags = {
                .use_mipi_interface = 1,
            },
        };
        esp_lcd_panel_dev_config_t config = panel_config_from_lcd_cfg(lcd_cfg, &vendor_config);
        ESP_LOGI(TAG, "Detected ST7703; selecting 720x720 initialization");
        return esp_lcd_new_panel_st7703(lcd_handles->io_handle, &config,
                                        &lcd_handles->panel_handle);
    }

    return ESP_ERR_NOT_SUPPORTED;
}

#if __has_include(<esp_lcd_touch_gt911.h>)
#include "esp_lcd_touch_gt911.h"

__attribute__((weak)) esp_err_t lcd_touch_factory_entry_t(
    esp_lcd_panel_io_handle_t io,
    const esp_lcd_touch_config_t *cfg,
    esp_lcd_touch_handle_t *touch)
{
    return esp_lcd_touch_new_i2c_gt911(io, cfg, touch);
}
#endif
