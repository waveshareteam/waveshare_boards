#include "esp_board_manager.h"
#include "waveshare_amoled.h"
#include "esp_err.h"

void app_main(void)
{
    ESP_ERROR_CHECK(esp_board_manager_init());
    (void)esp_board_manager_print_board_info();
    ESP_ERROR_CHECK(esp_board_manager_deinit());
}
