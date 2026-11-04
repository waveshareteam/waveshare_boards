option(BOARD_PACK_LCD4B_SMOKE "Build the LCD-4B hardware smoke app" OFF)

set(TEST_APP_SOURCE "main.c")
if(BOARD_PACK_LCD4B_SMOKE)
    set(TEST_APP_SOURCE "lcd4b_smoke.c")
endif()

idf_component_register(SRCS "${TEST_APP_SOURCE}"
                       REQUIRES esp_board_manager)
