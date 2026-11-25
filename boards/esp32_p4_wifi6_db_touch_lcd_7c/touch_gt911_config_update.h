/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"

#ifdef __cplusplus
extern "C" {
#endif
#if __has_include(<esp_lcd_touch_gt911.h>)

/**
 * @brief Reset the GT911 and make sure its configuration matches the LCD panel
 *
 * The configuration is only written when the resolution, touch count, axis
 * reverse bits or checksum differ from the board configuration.
 *
 * @param io       Panel IO handle of the GT911 (16-bit register address)
 * @param x_res    Panel horizontal resolution
 * @param y_res    Panel vertical resolution
 * @param rst_gpio GT911 reset GPIO, GPIO_NUM_NC to skip the reset
 * @param int_gpio GT911 interrupt GPIO, GPIO_NUM_NC if not connected
 */
esp_err_t touch_gt911_config_update(esp_lcd_panel_io_handle_t io, uint16_t x_res, uint16_t y_res,
                                    gpio_num_t rst_gpio, gpio_num_t int_gpio);
#endif

#ifdef __cplusplus
}
#endif
