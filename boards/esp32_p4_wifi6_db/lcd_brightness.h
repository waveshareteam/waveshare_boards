/*
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set LCD backlight brightness through the custom Board Manager device handle.
 *
 * @param[in] device_handle Handle returned for "lcd_brightness".
 * @param[in] brightness_percent Brightness percentage from 0 to 100.
 *
 * @return ESP_OK on success, otherwise an error code.
 */
esp_err_t esp_custom_lcd_brightness_set(void *device_handle, int brightness_percent);

/** Get the last brightness percentage successfully written by this device. */
esp_err_t esp_custom_lcd_brightness_get(void *device_handle, int *brightness_percent);

#ifdef __cplusplus
}
#endif
