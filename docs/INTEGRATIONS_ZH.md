# AMOLED 调光与可选集成

[English](INTEGRATIONS.md) · [首页](../README_ZH.md) · [板卡结构](BOARDS_ZH.md)

## 不依赖 Brookesia 的调光

公共函数 `waveshare_amoled_set_brightness(io, percent)` 位于
`include/waveshare_amoled.h`，不依赖 Brookesia，使用原有 QSPI 亮度命令控制所支持的
CO5300 / SH8601 面板。普通应用初始化板卡后，可以使用显示设备的 IO 句柄调光：

```c
#include "esp_board_manager.h"
#include "dev_display_lcd.h"
#include "waveshare_amoled.h"

// 在受支持的 AMOLED 板卡上，esp_board_manager_init() 成功之后：
dev_display_lcd_handles_t *display = NULL;
ESP_ERROR_CHECK(esp_board_manager_get_device_handle("display_lcd", (void **)&display));
ESP_ERROR_CHECK(waveshare_amoled_set_brightness(display->io_handle, 75));
```

函数将超过 100 的百分比限制为 100，发送失败时返回 LCD IO 错误。
它不缓存状态；下面的可选框架适配继续保留原有缓存读取、线程互斥与接口生命周期。
RGB LCD 板卡不使用这条 QSPI AMOLED 命令。

## Brookesia AMOLED 调光

`integrations/brookesia_hal_custom/` 为 1.75C、1.8 V1 和 2.16 三个 AMOLED 型号
提供一份共用适配，替代原来重复的三份亮度实现。板卡发现和硬件初始化使用 `boards/`，
不会自动选择该组件。需要 Brookesia HAL 调光接口的应用可以显式接入。

共用适配保留以下行为：

- 设备名 `CustomDisplay`、接口实现名 `CustomDisplay:Backlight` 和显示分组 `display_lcd`。
- `set_brightness()`、`get_brightness()`，百分比限制在 0–100，转换成面板的
  0–255 亮度值，通过公共函数发送原有 QSPI `0x51` 命令。
- 线程互斥、相同值不重复发送、错误返回与初始化行为。插件初始化亮度仍为 0%，
  后续由应用设置亮度。`get_brightness()` 返回最近成功设置的值，不是从硬件读回。
- 独立灯光开关接口仍返回不支持，与原实现一致。

普通屏幕初始化保留原有的初始亮度命令。该插件负责框架接口适配，不替代面板或 PMU 驱动。

## 应用显式接入

可选适配使用 **ESP-IDF 6.1**。测试的 Brookesia HAL interface 和 lib_utils 均为 **0.8.2**。
上游 lib_utils 0.8 要求 IDF **6.0 至 6.2**，无法在 IDF 5.5 下解析依赖。
这一上游框架限制不影响普通板卡包，普通模式继续覆盖 IDF 5.5.5 和 6.1。
依据见[上游依赖约定](https://github.com/espressif/esp-brookesia/blob/6a087b6d76e989802b72fdb835928b273af76af8/utils/brookesia_lib_utils/idf_component.yml)。

采用[快速开始](../README_ZH.md#quick-start)中的本地克隆结构时，在应用的
`main/idf_component.yml` 中增加：

```yaml
dependencies:
  brookesia_hal_custom:
    override_path: ../components/waveshare_boards/integrations/brookesia_hal_custom
```

与已有依赖合并，解析依赖后选择对应 AMOLED 板卡，再编译应用。
不要把这条依赖加回 `board_devices.yaml`。显式选择组件后，插件以原有名称注册，
应用仍通过正常的 HAL 接口查询方式使用它。

适配组件直接依赖 `brookesia_hal_interface` 和 Board Manager，不再仅为引用
`display_lcd` 分组常量而包含完整的 `brookesia_hal_adaptor`，因此独立插件不受
adaptor 的 Board Manager 版本约束牵连。

完整 Brookesia 应用仍需单独核对依赖：
[HAL adaptor 0.8.4](https://github.com/espressif/esp-brookesia/blob/6a087b6d76e989802b72fdb835928b273af76af8/hal/brookesia_hal_adaptor/idf_component.yml)
启用显示时要求 Board Manager `0.5.*`，而本板卡包要求 `>=0.7.2`。
本次没有修改该上游约束。独立接口适配编译通过，不代表未修改的完整 Brookesia 应用
能与本组件包一起解析依赖。

## 保留的应用配置

原 Brookesia 应用默认配置按型号分别保留：

```text
integrations/brookesia_hal_custom/profiles/
  esp32_s3_touch_amoled_1_75c/sdkconfig.defaults
  esp32_s3_touch_amoled_1_8/sdkconfig.defaults
  esp32_s3_touch_amoled_2_16/sdkconfig.defaults
```

这些是按需采用的参考配置，不会自动追加到板卡配置中。
它们会启用应用功能，并引用由应用提供的 `partitions_16m.csv`。
选择对应板卡配置后，需要结合应用的 Brookesia 版本与分区表核对。
普通硬件默认配置仍保存在各板卡目录。

## 验证范围

CI 对四块板卡在两个 IDF 版本线上运行普通模式编译，共八项；
另外在 IDF 6.1 上为三个 AMOLED 型号启用独立适配，共三项。
它验证 HAL 接口的编译与插件链接。主机测试覆盖公共函数的命令编码、百分比换算、
范围限制、无效句柄和 IO 错误返回；CI 也会在两个 IDF 版本线上编译公共头文件。
这些检查不替代实机调光或完整 Brookesia 应用测试。

在干净副本中，通过仓库测试应用编译该适配：

```bash
python ci/scripts/board_pack.py pin 0.7.2
python ci/scripts/board_pack.py integration brookesia
idf.py -C ci/test_app bmgr -b esp32_s3_touch_amoled_1_8
idf.py -C ci/test_app build
```

普通测试模式使用 `integration none`。命令只修改测试应用的清单，生成的测试输入不应提交。

实现和配置保留原有版权及许可证声明，详见[来源说明](BOARDS_ZH.md#来源许可证)。
