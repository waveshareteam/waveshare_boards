# AMOLED brightness control

[简体中文](INTEGRATIONS_ZH.md) · [Home](../README.md) · [Board layout](BOARDS.md)

## Set brightness

The public `waveshare_amoled_set_brightness(io, percent)` helper in
`include/waveshare_amoled.h` controls the supported CO5300/SH8601 QSPI AMOLED
panels. It is available directly from the board pack. After board initialization,
an application can use the selected display's IO handle:

```c
#include "esp_board_manager.h"
#include "dev_display_lcd.h"
#include "waveshare_amoled.h"

// After esp_board_manager_init() succeeds for a supported AMOLED board:
dev_display_lcd_handles_t *display = NULL;
ESP_ERROR_CHECK(esp_board_manager_get_device_handle("display_lcd", (void **)&display));
ESP_ERROR_CHECK(waveshare_amoled_set_brightness(display->io_handle, 75));
```

The helper accepts 0–100 percent, clamps values above 100, and sends the original
QSPI brightness command (`0x51`). It returns `ESP_ERR_INVALID_ARG` for a null IO
handle and propagates LCD IO errors. Zero sets brightness to zero.

The helper has no cached state or mutex. Applications that need a remembered
brightness value or coordinate concurrent brightness changes manage that state
and synchronization themselves. The panel's existing initial-brightness command
and board power initialization are unchanged. RGB LCD boards do not use this
QSPI AMOLED command.

## Supported profiles and validation

This helper applies to the 1.75C, 1.8 V1, and 2.16 AMOLED profiles. See
[board revisions](BOARDS.md) before selecting a hardware revision.

Host tests check the command encoding, percentage scaling, clamping, invalid
handles, and IO-error propagation. CI compiles all four board profiles on
ESP-IDF 5.5.5 and 6.1, including the public header, for eight builds in total.
Physical brightness changes still require testing on the selected board.

The implementation retains its original copyright and license notice. See
[source attribution](BOARDS.md#source-licenses).
