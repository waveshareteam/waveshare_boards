/*
 * SPDX-FileCopyrightText: 2026 Waveshare
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_panel_io.h"

typedef enum {
    WAVESHARE_LCD_PANEL_UNKNOWN = 0,
    WAVESHARE_LCD_PANEL_ST7703,
    WAVESHARE_LCD_PANEL_JD9365,
} waveshare_lcd_panel_kind_t;

typedef struct {
    float pixel_clock_mhz;
    uint16_t hsync_back_porch;
    uint16_t hsync_pulse_width;
    uint16_t hsync_front_porch;
    uint16_t vsync_back_porch;
    uint16_t vsync_pulse_width;
    uint16_t vsync_front_porch;
} waveshare_lcd_panel_timing_t;

/** Read the panel identity over DSI DBI with bounded FIFO waits. */
esp_err_t waveshare_lcd_panel_probe(esp_lcd_panel_io_handle_t io,
                                    waveshare_lcd_panel_kind_t *kind);

/** Runtime result of the Board Manager DSI factory hook. */
waveshare_lcd_panel_kind_t waveshare_lcd_panel_get_kind(void);
bool waveshare_lcd_panel_is_identified(void);
bool waveshare_lcd_panel_get_effective_timing(waveshare_lcd_panel_timing_t *timing);
