#include "esp_board_device.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_log.h"

#define GT911_ADDR_0 0xba
#define GT911_ADDR_1 0x28

esp_err_t lcd_touch_factory_entry_t(esp_lcd_panel_io_handle_t io,
                                    const esp_lcd_touch_config_t *touch_cfg,
                                    esp_lcd_touch_handle_t *ret_touch)
{
    uint16_t address = 0;
    esp_err_t err = esp_board_device_get_i2c_effective_addr("lcd_touch", &address);
    if (err != ESP_OK || (address != GT911_ADDR_0 && address != GT911_ADDR_1)) {
        ESP_LOGE("ws_touch", "Unsupported GT911 address: 0x%02x", address);
        return err == ESP_OK ? ESP_ERR_NOT_SUPPORTED : err;
    }
    return esp_lcd_touch_new_i2c_gt911(io, touch_cfg, ret_touch);
}
