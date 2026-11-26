/*
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "dev_lcd_touch.h"
#include "driver/gpio.h"
#include "esp_board_device.h"
#include "esp_board_manager.h"
#include "esp_check.h"
#include "esp_lcd_touch_cst816s.h"
#include "esp_lcd_touch_ft5x06.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static inline esp_err_t waveshare_amoled_2_06_reset_touch(const esp_lcd_touch_config_t *config)
{
    const gpio_num_t pin = config->rst_gpio_num;
    ESP_RETURN_ON_ERROR(gpio_set_direction(pin, GPIO_MODE_OUTPUT), "ws_touch", "Configure touch reset");
    ESP_RETURN_ON_ERROR(gpio_set_level(pin, config->levels.reset), "ws_touch", "Assert touch reset");
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_RETURN_ON_ERROR(gpio_set_level(pin, !config->levels.reset), "ws_touch", "Release touch reset");
    vTaskDelay(pdMS_TO_TICKS(120));
    return ESP_OK;
}

static inline int waveshare_amoled_2_06_init_touch(void *cfg, int cfg_size, void **handle)
{
    if (!cfg || cfg_size != sizeof(dev_lcd_touch_config_t) || !handle) {
        return -1;
    }
    const dev_lcd_touch_config_t *device = cfg;
    if (waveshare_amoled_2_06_reset_touch(&device->touch_config) == ESP_OK &&
            dev_lcd_touch_init(cfg, cfg_size, handle) == 0) {
        return 0;
    }
    gpio_reset_pin(device->touch_config.rst_gpio_num);
    return -1;
}

static inline int waveshare_amoled_2_06_deinit_touch(void *handle)
{
    dev_lcd_touch_config_t *device = NULL;
    if (esp_board_device_get_config_by_handle(handle, (void **)&device) != ESP_OK || !device) {
        return -1;
    }
    const gpio_num_t pin = device->touch_config.rst_gpio_num;
    int ret = dev_lcd_touch_deinit(handle);
    if (ret == 0) {
        gpio_reset_pin(pin);
    }
    return ret;
}

static inline esp_err_t waveshare_amoled_2_06_prepare_touch(void)
{
    // Wake the controller immediately before each probe, including late or repeated device initialization.
    return esp_board_device_set_ops("lcd_touch", waveshare_amoled_2_06_init_touch, waveshare_amoled_2_06_deinit_touch);
}

static inline esp_err_t waveshare_amoled_2_06_new_touch(esp_lcd_panel_io_handle_t io,
                                                        const esp_lcd_touch_config_t *config,
                                                        esp_lcd_touch_handle_t *ret_touch)
{
    uint16_t address;
    ESP_RETURN_ON_ERROR(esp_board_device_get_i2c_effective_addr("lcd_touch", &address), "ws_touch",
                        "Get probed touch address");
    esp_lcd_touch_config_t driver_config = *config;
    driver_config.rst_gpio_num = GPIO_NUM_NC;  // The board already reset the controller before probing.
    switch (address) {
    case ESP_LCD_TOUCH_IO_I2C_CST816S_ADDRESS << 1:
        ESP_LOGI("ws_touch", "CST820B detected at 0x%02x", address >> 1);
        return esp_lcd_touch_new_i2c_cst816s(io, &driver_config, ret_touch);
    case ESP_LCD_TOUCH_IO_I2C_FT5x06_ADDRESS << 1:
        ESP_LOGI("ws_touch", "FT6146 detected at 0x%02x", address >> 1);
        return esp_lcd_touch_new_i2c_ft5x06(io, &driver_config, ret_touch);
    default:
        ESP_LOGE("ws_touch", "Unsupported touch address 0x%02x", address >> 1);
        return ESP_ERR_NOT_SUPPORTED;
    }
}
