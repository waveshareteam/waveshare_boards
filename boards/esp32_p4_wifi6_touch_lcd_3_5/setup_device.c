#include "esp_board_manager_includes.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static esp_err_t board_touch_prepare(void)
{
    const gpio_config_t reset_config = {
        .pin_bit_mask = BIT64(29),
        .mode = GPIO_MODE_OUTPUT,
    };
    esp_err_t ret = gpio_config(&reset_config);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = gpio_set_level(29, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    ret = gpio_set_level(29, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    return ESP_OK;
}

#if __has_include(<esp_lcd_st7796.h>)
#include "esp_lcd_st7796.h"
__attribute__((weak)) esp_err_t lcd_panel_factory_entry_t(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
    // Display initialization precedes the Board Manager touch I2C probe.
    esp_err_t ret = board_touch_prepare();
    if (ret != ESP_OK) {
        return ret;
    }
    // The ST7796 driver's built-in init sequence matches this panel.
    return esp_lcd_new_panel_st7796(io, panel_dev_config, ret_panel);
}
#endif

#if __has_include(<esp_lcd_touch_ft5x06.h>)
#include "esp_lcd_touch_ft5x06.h"
__attribute__((weak)) esp_err_t lcd_touch_factory_entry_t(esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *cfg, esp_lcd_touch_handle_t *touch)
{
    return esp_lcd_touch_new_i2c_ft5x06(io, cfg, touch);
}
#endif
