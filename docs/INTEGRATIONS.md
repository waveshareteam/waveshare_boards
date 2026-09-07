# AMOLED brightness and optional integrations

[简体中文](INTEGRATIONS_ZH.md) · [Home](../README.md) · [Board layout](BOARDS.md)

## Brightness without Brookesia

The public `waveshare_amoled_set_brightness(io, percent)` helper is available in
`include/waveshare_amoled.h`. It sends the original QSPI brightness command for
the supported CO5300/SH8601 panels without a Brookesia dependency. After board
initialization, an ordinary application can use the selected display's IO handle:

```c
#include "esp_board_manager.h"
#include "dev_display_lcd.h"
#include "waveshare_amoled.h"

// After esp_board_manager_init() succeeds for a supported AMOLED board:
dev_display_lcd_handles_t *display = NULL;
ESP_ERROR_CHECK(esp_board_manager_get_device_handle("display_lcd", (void **)&display));
ESP_ERROR_CHECK(waveshare_amoled_set_brightness(display->io_handle, 75));
```

The function clamps percentages above 100 and returns the LCD IO error on failure.
It has no cached state; the optional framework adapter below retains the original
cached getter, mutex, and interface lifecycle. RGB LCD boards do not use this
QSPI AMOLED command.

## Brookesia AMOLED brightness

`integrations/brookesia_hal_custom/` provides one shared adapter for the 1.75C,
1.8 V1, and 2.16 AMOLED profiles. It replaces their three identical brightness
implementations. Board discovery and hardware initialization use `boards/` and
do not select this component. Applications opt in when they need Brookesia HAL
brightness control.

The shared adapter preserves:

- Device name `CustomDisplay`, interface implementation `CustomDisplay:Backlight`,
  and display group `display_lcd`.
- `set_brightness()` and `get_brightness()`, clamping to 0–100 percent and converting
  to the panel's 0–255 brightness command (`0x51` over QSPI) through the public helper.
- The mutex, unchanged-command suppression, error propagation, and initialization
  behavior. The plugin starts at 0 percent; the application then chooses brightness.
  `get_brightness()` returns the last successfully set value, not a hardware readback.
- The original unsupported result for separate light on/off methods.

The ordinary panel setup retains its existing initial-brightness command. The
plugin is a framework interface adapter, not a replacement panel or PMU driver.

## Explicit application setup

Use **ESP-IDF 6.1** for the optional adapter. Brookesia HAL interface **0.8.2**
and its lib_utils **0.8.2** dependency are tested. Upstream lib_utils 0.8 requires
IDF **6.0 through 6.2**; it cannot resolve on IDF 5.5. This upstream framework
requirement does not apply to the ordinary board pack, which continues to build
on IDF 5.5.5 and 6.1. See the
[upstream dependency contract](https://github.com/espressif/esp-brookesia/blob/6a087b6d76e989802b72fdb835928b273af76af8/utils/brookesia_lib_utils/idf_component.yml).

For the local clone layout in the [quick start](../README.md#quick-start), add
this dependency to the application's `main/idf_component.yml`:

```yaml
dependencies:
  brookesia_hal_custom:
    override_path: ../components/waveshare_boards/integrations/brookesia_hal_custom
```

Merge it with existing dependencies. Resolve dependencies, select the matching
AMOLED board, and build the application. Do not add this adapter dependency to
`board_devices.yaml`. Explicit component selection registers the plugin under
its original name; applications use their normal HAL interface lookup.

The adapter directly uses `brookesia_hal_interface` and Board Manager. It no
longer includes the full `brookesia_hal_adaptor` solely for the `display_lcd`
group constant. This keeps the standalone plugin independent of the adaptor's
Board Manager version constraint.

A complete Brookesia application still needs its own dependency review:
[HAL adaptor 0.8.4](https://github.com/espressif/esp-brookesia/blob/6a087b6d76e989802b72fdb835928b273af76af8/hal/brookesia_hal_adaptor/idf_component.yml)
requires Board Manager `0.5.*` when display is enabled, while this board pack
requires `>=0.7.2`. That upstream constraint is not changed here. Passing the
standalone interface-adapter test does not claim that an unmodified full
Brookesia application resolves with this pack.

## Preserved application profiles

The original Brookesia application defaults are retained separately:

```text
integrations/brookesia_hal_custom/profiles/
  esp32_s3_touch_amoled_1_75c/sdkconfig.defaults
  esp32_s3_touch_amoled_1_8/sdkconfig.defaults
  esp32_s3_touch_amoled_2_16/sdkconfig.defaults
```

They are opt-in reference configuration, not automatically appended board
settings. They enable application features and refer to `partitions_16m.csv`,
which the consuming application must provide. Choose the matching board profile
and review it against that application's Brookesia version and partition layout.
The ordinary hardware defaults remain beside each board.

## Validation

CI builds the plain board pack for all four boards on both IDF lines (eight
builds), then separately enables this adapter for the three AMOLED boards on
IDF 6.1 (three builds). This proves compilation and plugin linkage against the
HAL interfaces. Host tests cover the public command encoding, percentage scaling,
clamping, invalid handles, and IO-error propagation. CI also compiles the public
header with both IDF lines. It does not exercise physical brightness changes or a full
Brookesia application.

To compile the adapter with the repository's test app in a fresh checkout:

```bash
python ci/scripts/board_pack.py pin 0.7.2
python ci/scripts/board_pack.py integration brookesia
idf.py -C ci/test_app bmgr -b esp32_s3_touch_amoled_1_8
idf.py -C ci/test_app build
```

Use `integration none` for the ordinary test mode. Both commands only modify
the test application's manifest; do not commit generated test inputs.

The implementation and profiles retain their upstream copyright and license
notices. See [source attribution](BOARDS.md#source-licenses).
