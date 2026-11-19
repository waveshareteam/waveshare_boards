/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if __has_include(<esp_lcd_touch_gt911.h>)

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


#include "touch_gt911_config_update.h"

static const char *touch_firmware_tag = "GT911_CFG_UPDATE";

#define TOUCH_GT911_COMMAND_REG (0x8040)
#define TOUCH_GT911_STATUS_REG (0x8000)
#define TOUCH_GT911_CONFIG_REG (0x8047)
#define TOUCH_GT911_READ_XY_REG (0x814E)
#define TOUCH_GT911_CONFIG_SIZE (186U)
#define TOUCH_GT911_CHECKSUM_OFFSET (184U)
#define TOUCH_GT911_FRESH_OFFSET (185U)
#define TOUCH_GT911_X_RES_OFFSET (1U)
#define TOUCH_GT911_Y_RES_OFFSET (3U)
#define TOUCH_GT911_MAX_TOUCH_OFFSET (5U)
#define TOUCH_GT911_MODULE_SWITCH1_OFFSET (6U)
#define TOUCH_GT911_AXIS_REVERSE_MASK (0xC0U)

static const uint8_t s_gt911_config_template[TOUCH_GT911_CONFIG_SIZE] = {
    0x5B, 0x00, 0x04, 0x58, 0x02, 0x05, 0x3D, 0x00, 0x22, 0x08, 0x1E, 0x08, 0x6E, 0x50, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x18, 0x1A, 0x1E, 0x14, 0x8C, 0x2E, 0x0E, 0xB1,
    0xB3, 0xB8, 0x08, 0x00, 0x00, 0x00, 0xD9, 0x02, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0xC8, 0x9E, 0xF5, 0xF4, 0x07, 0x00, 0x00, 0x04, 0x81, 0x9A,
    0x00, 0x7F, 0xA3, 0x00, 0x7E, 0xAD, 0x00, 0x7D, 0xB7, 0x00, 0x7D, 0xC2, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18,
    0x1A, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x29, 0x28, 0x26, 0x24, 0x22, 0x21, 0x20, 0x1F, 0x1E, 0x1D, 0x1C, 0x18,
    0x16, 0x14, 0x13, 0x12, 0x10, 0x0F, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x01,
};

_Static_assert(sizeof(s_gt911_config_template) == TOUCH_GT911_CONFIG_SIZE, "GT911 configuration template must contain 186 bytes");

static uint8_t touch_gt911_calculate_checksum(const uint8_t *config)
{
    uint8_t checksum = 0;

    for (size_t i = 0; i < TOUCH_GT911_CHECKSUM_OFFSET; i++) {
        checksum += config[i];
    }
    return (uint8_t)(0U - checksum);
}

static esp_err_t touch_gt911_reset_controller(gpio_num_t rst_gpio, gpio_num_t int_gpio)
{
    if (rst_gpio == GPIO_NUM_NC) {
        return ESP_OK;
    }

    const gpio_config_t reset_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(rst_gpio),
    };
    ESP_RETURN_ON_ERROR(gpio_config(&reset_config), touch_firmware_tag, "Configure GT911 reset GPIO failed");

    ESP_RETURN_ON_ERROR(gpio_set_level(rst_gpio, 1), touch_firmware_tag, "Release GT911 reset failed");
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_RETURN_ON_ERROR(gpio_set_level(rst_gpio, 0), touch_firmware_tag, "Assert GT911 reset failed");
    vTaskDelay(pdMS_TO_TICKS(100));

    if (int_gpio != GPIO_NUM_NC) {
        const gpio_config_t int_config = {
            .mode = GPIO_MODE_OUTPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pin_bit_mask = BIT64(int_gpio),
        };
        ESP_RETURN_ON_ERROR(gpio_config(&int_config), touch_firmware_tag, "Configure GT911 interrupt GPIO failed");
        ESP_RETURN_ON_ERROR(gpio_set_level(int_gpio, 0), touch_firmware_tag, "Assert GT911 interrupt failed");
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    ESP_RETURN_ON_ERROR(gpio_set_level(rst_gpio, 1), touch_firmware_tag, "Release GT911 reset failed");
    vTaskDelay(pdMS_TO_TICKS(200));

    if (int_gpio != GPIO_NUM_NC) {
        ESP_RETURN_ON_ERROR(gpio_reset_pin(int_gpio), touch_firmware_tag, "Release GT911 interrupt GPIO failed");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return ESP_OK;
}

static uint16_t touch_gt911_read_u16(const uint8_t *config, size_t offset)
{
    return (uint16_t)config[offset] | ((uint16_t)config[offset + 1] << 8);
}

static bool touch_gt911_config_checksum_valid(const uint8_t *config)
{
    return config[TOUCH_GT911_CHECKSUM_OFFSET] == touch_gt911_calculate_checksum(config);
}

static void touch_gt911_prepare_config(uint8_t *config, uint16_t x_resolution, uint16_t y_resolution)
{

    memcpy(config, s_gt911_config_template, sizeof(s_gt911_config_template));
    config[TOUCH_GT911_X_RES_OFFSET] = (uint8_t)(x_resolution & 0xFFU);
    config[TOUCH_GT911_X_RES_OFFSET + 1] = (uint8_t)(x_resolution >> 8);
    config[TOUCH_GT911_Y_RES_OFFSET] = (uint8_t)(y_resolution & 0xFFU);
    config[TOUCH_GT911_Y_RES_OFFSET + 1] = (uint8_t)(y_resolution >> 8);
    config[TOUCH_GT911_MODULE_SWITCH1_OFFSET] |= TOUCH_GT911_AXIS_REVERSE_MASK;
    config[TOUCH_GT911_FRESH_OFFSET] = 1;
    config[TOUCH_GT911_CHECKSUM_OFFSET] = touch_gt911_calculate_checksum(config);
}

static bool touch_gt911_config_matches(const uint8_t *current_config, const uint8_t *target_config)
{
    if (!touch_gt911_config_checksum_valid(current_config)) {
        return false;
    }
    return current_config[TOUCH_GT911_X_RES_OFFSET] == target_config[TOUCH_GT911_X_RES_OFFSET] && current_config[TOUCH_GT911_X_RES_OFFSET + 1] == target_config[TOUCH_GT911_X_RES_OFFSET + 1] &&
           current_config[TOUCH_GT911_Y_RES_OFFSET] == target_config[TOUCH_GT911_Y_RES_OFFSET] && current_config[TOUCH_GT911_Y_RES_OFFSET + 1] == target_config[TOUCH_GT911_Y_RES_OFFSET + 1] &&
           current_config[TOUCH_GT911_MAX_TOUCH_OFFSET] == target_config[TOUCH_GT911_MAX_TOUCH_OFFSET] &&
           (current_config[TOUCH_GT911_MODULE_SWITCH1_OFFSET] & TOUCH_GT911_AXIS_REVERSE_MASK) == (target_config[TOUCH_GT911_MODULE_SWITCH1_OFFSET] & TOUCH_GT911_AXIS_REVERSE_MASK);
}

static esp_err_t touch_gt911_update_config(esp_lcd_panel_io_handle_t io, uint16_t x_res, uint16_t y_res)
{
    uint8_t current_config[TOUCH_GT911_CONFIG_SIZE] = {0};
    uint8_t target_config[TOUCH_GT911_CONFIG_SIZE] = {0};
    uint8_t command = 0x02;
    uint8_t clear_status = 0;
    uint8_t status = 0;

    ESP_RETURN_ON_FALSE(io != NULL, ESP_ERR_INVALID_ARG, touch_firmware_tag, "GT911 IO handle is NULL");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_rx_param(io, TOUCH_GT911_CONFIG_REG, current_config, sizeof(current_config)), touch_firmware_tag, "Read GT911 configuration failed");

    touch_gt911_prepare_config(target_config, x_res, y_res);
    const bool current_checksum_valid = touch_gt911_config_checksum_valid(current_config);
    ESP_LOGI(touch_firmware_tag,
             "GT911 current configuration: version=0x%02X, resolution=%ux%u, max_touch=%u, "
             "axis_reverse=0x%02X, checksum=%s",
             current_config[0], touch_gt911_read_u16(current_config, TOUCH_GT911_X_RES_OFFSET), touch_gt911_read_u16(current_config, TOUCH_GT911_Y_RES_OFFSET),
             current_config[TOUCH_GT911_MAX_TOUCH_OFFSET], current_config[TOUCH_GT911_MODULE_SWITCH1_OFFSET] & TOUCH_GT911_AXIS_REVERSE_MASK, current_checksum_valid ? "valid" : "invalid");
    if (touch_gt911_config_matches(current_config, target_config)) {
        ESP_LOGI(touch_firmware_tag, "GT911 configuration already matches the LCD panel");
        return ESP_OK;
    }

    if (current_checksum_valid) {
        ESP_LOGI(touch_firmware_tag, "Updating GT911 configuration from %ux%u to %ux%u, axis_reverse=0x%02X", touch_gt911_read_u16(current_config, TOUCH_GT911_X_RES_OFFSET),
                 touch_gt911_read_u16(current_config, TOUCH_GT911_Y_RES_OFFSET), touch_gt911_read_u16(target_config, TOUCH_GT911_X_RES_OFFSET),
                 touch_gt911_read_u16(target_config, TOUCH_GT911_Y_RES_OFFSET), target_config[TOUCH_GT911_MODULE_SWITCH1_OFFSET] & TOUCH_GT911_AXIS_REVERSE_MASK);
    } else {
        ESP_LOGW(touch_firmware_tag, "GT911 configuration checksum is invalid; writing the board configuration");
    }

    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, TOUCH_GT911_COMMAND_REG, &command, sizeof(command)), touch_firmware_tag, "Prepare GT911 configuration update failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, TOUCH_GT911_READ_XY_REG, &clear_status, sizeof(clear_status)), touch_firmware_tag, "Clear GT911 touch status failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_rx_param(io, TOUCH_GT911_STATUS_REG, &status, sizeof(status)), touch_firmware_tag, "Read GT911 update status failed");
    ESP_LOGD(touch_firmware_tag, "GT911 status before configuration update: 0x%02X", status);
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, TOUCH_GT911_CONFIG_REG, target_config, sizeof(target_config)), touch_firmware_tag, "Write GT911 configuration failed");

    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_rx_param(io, TOUCH_GT911_CONFIG_REG, current_config, sizeof(current_config)), touch_firmware_tag, "Read back GT911 configuration failed");
    ESP_RETURN_ON_FALSE(touch_gt911_config_matches(current_config, target_config), ESP_ERR_INVALID_CRC, touch_firmware_tag, "GT911 configuration verification failed");

    ESP_LOGI(touch_firmware_tag, "GT911 configuration update completed");
    vTaskDelay(pdMS_TO_TICKS(100));
    return ESP_OK;
}

esp_err_t touch_gt911_config_update(esp_lcd_panel_io_handle_t io, uint16_t x_res, uint16_t y_res, gpio_num_t rst_gpio, gpio_num_t int_gpio)
{
    ESP_RETURN_ON_FALSE(io != NULL, ESP_ERR_INVALID_ARG, touch_firmware_tag, "GT911 IO handle is NULL");
    ESP_RETURN_ON_ERROR(touch_gt911_reset_controller(rst_gpio, int_gpio), touch_firmware_tag, "Reset GT911 before configuration update failed");
    return touch_gt911_update_config(io, x_res, y_res);
}
#endif
