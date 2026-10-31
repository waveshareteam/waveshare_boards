/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include <stdint.h>
#include <stdlib.h>

#include "driver/i2c_master.h"
#include "esp_board_manager_includes.h"
#include "esp_log.h"
#include "gen_board_device_custom.h"
#include "lcd_brightness.h"

static const char *TAG = "CUSTOM_LCD_BRIGHTNESS";

typedef struct {
    i2c_master_dev_handle_t i2c_device;
    uint8_t brightness_register;
    int brightness_percent;
    const char *i2c_peripheral_name;
} custom_lcd_brightness_handle_t;

esp_err_t esp_custom_lcd_brightness_set(void *device_handle, int brightness_percent)
{
    custom_lcd_brightness_handle_t *handle = (custom_lcd_brightness_handle_t *)device_handle;
    if (handle == NULL || handle->i2c_device == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    if (brightness_percent < 0) {
        brightness_percent = 0;
    }

    uint8_t data[] = {
        handle->brightness_register,
        (uint8_t)((255U * (unsigned int)brightness_percent) / 100U),
    };
    esp_err_t ret = i2c_master_transmit(handle->i2c_device, data, sizeof(data), 50);
    if (ret == ESP_OK) {
        handle->brightness_percent = brightness_percent;
    }
    return ret;
}

esp_err_t esp_custom_lcd_brightness_get(void *device_handle, int *brightness_percent)
{
    custom_lcd_brightness_handle_t *handle = (custom_lcd_brightness_handle_t *)device_handle;
    if (handle == NULL || handle->i2c_device == NULL || brightness_percent == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *brightness_percent = handle->brightness_percent;
    return ESP_OK;
}

static int custom_lcd_brightness_init(void *cfg, int cfg_size, void **device_handle)
{
    if (cfg == NULL || device_handle == NULL || cfg_size != sizeof(dev_custom_lcd_brightness_config_t)) {
        ESP_LOGE(TAG, "Invalid LCD brightness configuration");
        return ESP_ERR_INVALID_ARG;
    }

    const dev_custom_lcd_brightness_config_t *config =
        (const dev_custom_lcd_brightness_config_t *)cfg;
    if (config->peripheral_name == NULL) {
        ESP_LOGE(TAG, "No I2C peripheral configured for LCD brightness");
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_handle_t i2c_bus = NULL;
    esp_err_t ret = esp_board_periph_ref_handle(config->peripheral_name, (void **)&i2c_bus);
    if (ret != ESP_OK || i2c_bus == NULL) {
        ESP_LOGE(TAG, "Failed to get I2C peripheral handle: %s", esp_err_to_name(ret));
        return ret != ESP_OK ? ret : ESP_ERR_INVALID_STATE;
    }

    custom_lcd_brightness_handle_t *handle = calloc(1, sizeof(*handle));
    if (handle == NULL) {
        esp_board_periph_unref_handle(config->peripheral_name);
        return ESP_ERR_NO_MEM;
    }
    handle->i2c_peripheral_name = config->peripheral_name;

    const i2c_device_config_t i2c_device_config = {
        .scl_speed_hz = config->scl_speed_hz,
        .device_address = config->i2c_address,
    };
    ret = i2c_master_bus_add_device(i2c_bus, &i2c_device_config, &handle->i2c_device);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add LCD brightness I2C device: %s", esp_err_to_name(ret));
        esp_board_periph_unref_handle(handle->i2c_peripheral_name);
        free(handle);
        return ret;
    }

    handle->brightness_register = config->brightness_register;
    ret = esp_custom_lcd_brightness_set(handle, config->default_percent);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set default LCD brightness: %s", esp_err_to_name(ret));
        i2c_master_bus_rm_device(handle->i2c_device);
        esp_board_periph_unref_handle(handle->i2c_peripheral_name);
        free(handle);
        return ret;
    }

    *device_handle = handle;
    ESP_LOGI(TAG, "LCD brightness controller initialized: address=0x%02x, register=0x%02x, default=%u%%",
             (unsigned int)(uint8_t)config->i2c_address,
             (unsigned int)config->brightness_register,
             (unsigned int)config->default_percent);
    return ESP_OK;
}

static int custom_lcd_brightness_deinit(void *device_handle)
{
    custom_lcd_brightness_handle_t *handle = (custom_lcd_brightness_handle_t *)device_handle;
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    if (handle->i2c_device != NULL) {
        ret = i2c_master_bus_rm_device(handle->i2c_device);
    }
    esp_err_t periph_ret = esp_board_periph_unref_handle(handle->i2c_peripheral_name);
    free(handle);
    return ret != ESP_OK ? ret : periph_ret;
}

CUSTOM_DEVICE_IMPLEMENT(lcd_brightness, custom_lcd_brightness_init, custom_lcd_brightness_deinit);
