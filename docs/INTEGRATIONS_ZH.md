# AMOLED 亮度控制

[English](INTEGRATIONS.md) · [首页](../README_ZH.md) · [板卡结构](BOARDS_ZH.md)

## 设置亮度

公共函数 `waveshare_amoled_set_brightness(io, percent)` 位于
`include/waveshare_amoled.h`，控制所支持的 CO5300 / SH8601 QSPI AMOLED 面板，
由板卡包直接提供。应用初始化板卡后，可以使用显示设备的 IO 句柄调光：

```c
#include "esp_board_manager.h"
#include "dev_display_lcd.h"
#include "waveshare_amoled.h"

// 在受支持的 AMOLED 板卡上，esp_board_manager_init() 成功之后：
dev_display_lcd_handles_t *display = NULL;
ESP_ERROR_CHECK(esp_board_manager_get_device_handle("display_lcd", (void **)&display));
ESP_ERROR_CHECK(waveshare_amoled_set_brightness(display->io_handle, 75));
```

函数接受 0–100 的亮度百分比，将超过 100 的值限制为 100，发送原有 QSPI 亮度命令 `0x51`。
IO 句柄为空时返回 `ESP_ERR_INVALID_ARG`，发送失败时返回 LCD IO 错误。
设置为 0 时将亮度降至零。

函数不缓存状态，也不持有互斥锁。需要记忆亮度值或协调多个任务调光的应用，
应自行管理状态和同步。面板原有的初始亮度命令和板卡电源初始化保持不变。
RGB LCD 板卡不使用这条 QSPI AMOLED 命令。

## 支持配置与验证

函数适用于 1.75C、1.8 V1 和 2.16 AMOLED 配置。选择板卡前请核对[硬件版本](BOARDS_ZH.md)。

主机测试覆盖命令编码、百分比换算、范围限制、无效句柄和 IO 错误返回。
CI 对四块板卡在 ESP-IDF 5.5.5 和 6.1 上进行编译，并包含公共头文件，共八项编译。
实际亮度变化仍需在对应板卡上测试。

实现保留原有版权及许可证声明，详见[来源说明](BOARDS_ZH.md#来源许可证)。
