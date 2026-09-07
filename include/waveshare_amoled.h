/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 *
 * Extracted from the Waveshare AMOLED brightness adapter in ESP-Brookesia.
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_lcd_panel_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set brightness on the supported CO5300/SH8601 QSPI AMOLED panels.
 *
 * The caller supplies the initialized display's panel IO handle. Values above
 * 100 are clamped; zero turns brightness down to zero. This helper has no
 * Brookesia dependency or cached state and returns the panel IO result.
 */
static inline esp_err_t waveshare_amoled_set_brightness(esp_lcd_panel_io_handle_t io, uint8_t percent)
{
    if (!io) {
        return ESP_ERR_INVALID_ARG;
    }
    if (percent > 100) {
        percent = 100;
    }
    const uint8_t level = (uint8_t)((255U * percent) / 100U);
    const int command = (0x02U << 24) | (0x51U << 8);
    return esp_lcd_panel_io_tx_param(io, command, &level, sizeof(level));
}

#ifdef __cplusplus
}
#endif
