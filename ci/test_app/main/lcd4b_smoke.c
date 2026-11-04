#include "driver/ledc.h"
#include "esp_board_manager.h"
#include "dev_display_lcd.h"
#include "dev_lcd_touch.h"
#include "periph_ledc.h"
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd_panel_probe.h"

static const char *TAG = "lcd4b_smoke";

static void draw_bands(esp_lcd_panel_handle_t panel, uint16_t width, uint16_t height)
{
    enum { BAND_HEIGHT = 24 };
    static uint16_t pixels[720 * BAND_HEIGHT];
    const uint16_t colors[] = {0xf800, 0x07e0, 0x001f, 0xffff, 0xffe0, 0xf81f};
    for (uint16_t y = 0; y < height; y += BAND_HEIGHT) {
        const uint16_t lines = (height - y) < BAND_HEIGHT ? (height - y) : BAND_HEIGHT;
        const uint16_t color = colors[(y / BAND_HEIGHT) % (sizeof(colors) / sizeof(colors[0]))];
        for (size_t i = 0; i < (size_t)width * lines; ++i) {
            pixels[i] = color;
        }
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel, 0, y, width, y + lines, pixels));
    }
}

static const char *panel_name(waveshare_lcd_panel_kind_t kind)
{
    switch (kind) {
    case WAVESHARE_LCD_PANEL_JD9365: return "JD9365DA";
    case WAVESHARE_LCD_PANEL_ST7703: return "ST7703";
    default: return "unknown";
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(esp_board_manager_init_device_by_name("display_lcd"));
    ESP_ERROR_CHECK(esp_board_manager_init_device_by_name("lcd_touch"));
    ESP_ERROR_CHECK(esp_board_manager_init_device_by_name("lcd_brightness"));

    dev_display_lcd_handles_t *lcd = NULL;
    dev_display_lcd_config_t *lcd_cfg = NULL;
    dev_lcd_touch_handles_t *touch = NULL;
    periph_ledc_handle_t *backlight = NULL;
    ESP_ERROR_CHECK(esp_board_manager_get_device_handle("display_lcd", (void **)&lcd));
    ESP_ERROR_CHECK(esp_board_manager_get_device_config("display_lcd", (void **)&lcd_cfg));
    ESP_ERROR_CHECK(esp_board_manager_get_device_handle("lcd_touch", (void **)&touch));
    ESP_ERROR_CHECK(esp_board_manager_get_device_handle("lcd_brightness", (void **)&backlight));

    waveshare_lcd_panel_timing_t timing = {0};
    ESP_ERROR_CHECK(waveshare_lcd_panel_get_effective_timing(&timing) ? ESP_OK : ESP_ERR_INVALID_STATE);
    ESP_LOGI(TAG, "LCD DCS profile: %s (identified=%d), %ux%u, %.3f MHz, HBP/HS/HFP=%u/%u/%u, VBP/VS/VFP=%u/%u/%u",
             panel_name(waveshare_lcd_panel_get_kind()), waveshare_lcd_panel_is_identified(),
             lcd_cfg->lcd_width, lcd_cfg->lcd_height, (double)timing.pixel_clock_mhz,
             timing.hsync_back_porch, timing.hsync_pulse_width, timing.hsync_front_porch,
             timing.vsync_back_porch, timing.vsync_pulse_width, timing.vsync_front_porch);

    uint8_t product_id[4] = {0};
    ESP_ERROR_CHECK(esp_lcd_panel_io_rx_param(touch->io_handle, 0x8140, product_id, sizeof(product_id)));
    ESP_LOGI(TAG, "GT911 product ID: %.4s", product_id);

    ESP_ERROR_CHECK(ledc_set_duty(backlight->speed_mode, backlight->channel, 1023));
    ESP_ERROR_CHECK(ledc_update_duty(backlight->speed_mode, backlight->channel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd->panel_handle, true));
    draw_bands(lcd->panel_handle, lcd_cfg->lcd_width, lcd_cfg->lcd_height);
    ESP_LOGI(TAG, "BMGR_DISPLAY_PASS: backlight=100 panel=on color-bands-drawn");

    for (;;) {
        ESP_ERROR_CHECK(esp_lcd_touch_read_data(touch->touch_handle));
        esp_lcd_touch_point_data_t points[5] = {0};
        uint8_t count = 0;
        if (esp_lcd_touch_get_data(touch->touch_handle, points, &count, 5) == ESP_OK && count) {
            ESP_LOGI(TAG, "BMGR_TOUCH_PASS: %u point(s), first=(%u,%u)", count, points[0].x, points[0].y);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
