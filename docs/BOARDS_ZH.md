# 板卡组织与迁移说明

[English](BOARDS.md) · [首页](../README_ZH.md)

## 目录结构

仓库与组件统一命名为 `waveshare_boards`，组件库标识为
`waveshare/waveshare_boards`。所有板卡定义统一放在：

```text
boards/
  esp32_s31_touch_lcd_4_3c/
  esp32_s3_lr1121_epaper_2_9/
  esp32_s3_lr1121_oled_1_54_xf/
  esp32_s3_touch_lcd_7/
  esp32_s3_touch_amoled_1_75c/
  esp32_s3_touch_amoled_1_8/
  esp32_s3_touch_amoled_2_16/
```

每个型号的 `board_info.yaml`、`board_devices.yaml`、`board_peripherals.yaml`、
初始化源码与板级组件保存在一起。型号目录必须与唯一的 `board` 字段一致。
名称保留芯片前缀，通过自动生成的目录表中的芯片列区分产品。
只有硬件版本需要独立选择配置时，才增加版本后缀。

不要再增加 `boards/esp32s3/<型号>/` 这一层。Board Manager 0.7.2 从应用的
`components/` 根目录扫描本地组件，组件包名称本身已经占用三层搜索深度中的一层。
`components/waveshare_boards/boards/<型号>/` 在限制内，再加芯片目录就超出限制。
托管安装与覆盖依赖从包根目录开始扫描，但仓库结构也必须兼容本地克隆。
`_boards` 后缀能够通过该版本对覆盖依赖名称的筛选，无需修改上级组件或传入 customer-path 参数。
依据见固定版本的[扫描实现](https://github.com/espressif/esp-board-manager/blob/2beb9b22b0892b343bd555a1ebc9929a7edce8fc/generators/config_generator.py)。

## S31 Touch LCD 4.3C 定义范围

`esp32_s31_touch_lcd_4_3c` 使用 **ESP32-S31（`esp32s31`）**，与 ESP32-S3 不同。
配置包含 800 × 480 RGB565 显示与两个 PSRAM 帧缓冲、GT911 触摸、
默认 60% 的 LEDC 背光、低电平供电使能的四线 SDMMC、ES8389 音频输入输出、
RS485，以及自定义 RTC、IMU、电量计和 CAN-FD 设备。
随板提供的 `setup_device.c` 校验 Board Manager 选出的 GT911 地址，再调用托管触摸驱动。

声明的接线与产品[硬件说明](https://github.com/waveshareteam/ESP32-S31-Touch-LCD-4.3C/blob/8007c28a43f19fea878a61df5ad34ee61cecb5e4/HARDWARE_REFERENCE_ZH.md)一致，
进一步核对可参考 [S31 原理图](https://github.com/waveshareteam/ESP32-S31-Touch-LCD-4.3C/blob/8007c28a43f19fea878a61df5ad34ee61cecb5e4/hardware/schematics/ESP32-S31-Touch-LCD-4.3C-Schematics.pdf)。
这不代表已完成完整原理图审计或本次导入配置的实机测试。

| 接口 | 配置的 GPIO / 参数 |
| --- | --- |
| I2C0 | SDA 0、SCL 1 |
| RGB 显示 | D0–D15：8–19、33–36；PCLK 40，26 MHz；DE 43、HSYNC 44、VSYNC 45 |
| 触摸 / 背光 | GT911 复位 46、中断 2；背光 47，5 kHz |
| SDMMC | CLK 24、CMD 25、D0–D3：20–23；电源使能 56，低电平有效 |
| I2S0 / 功放 | BCLK 37、WS 38、DOUT 39、DIN 42；功放使能 57 |
| RS485 | UART1 TX 6、RX 7、RTS/DE 61；115200 波特率，半双工 |
| CAN-FD | TX 4、RX 5；仲裁速率 500 kbit/s，数据速率 2 Mbit/s |
| RTC / IMU / 电量计 | PCF85063ATL `0x51`，IRQ 3；QMI8658 `0x6B`/`0x6A`，IRQ 54；BQ27220 `0x55` |

上表传感器地址为 7 位地址。GT911 的 7 位地址为 `0x5D`/`0x14`，
ES8389 为 `0x10`；YAML 使用 Board Manager 的 8 位表示 `0xBA`/`0x28` 和 `0x20`。
音频默认 48 kHz、16 位立体声，软件未配置外部 MCLK，不能据此推断硬件没有 MCLK 网络。
电池设计容量 650 mAh 是软件默认值。

显示、触摸、背光、SD 供电/挂载和音频没有跳过初始化。
未插 SD 卡可能导致自动板卡初始化失败；挂载失败不会自动格式化。
应用需为显示帧缓冲配置 PSRAM，此目录没有附带板级 sdkconfig 默认文件。
`rtc_pcf85063`、`imu_qmi8658`、`bat_bq27220` 和 `canfd` 设置了
`init_skip: true`；通过 Board Manager 使用这些自定义设备时，
需由应用提供适配代码并显式初始化。RS485 声明描述 UART，此处没有定义 USB 设备或主机栈初始化。

产品的[工具链配置](https://github.com/waveshareteam/ESP32-S31-Touch-LCD-4.3C/blob/8007c28a43f19fea878a61df5ad34ee61cecb5e4/releases/config.json)固定使用
ESP-IDF `6.2.0-dev-08e0d30a74ad`（提交
`08e0d30a74ad0bfd5a34933142b80f45619ee410`）、Board Manager `0.7.5`、
Component Manager `3.1.2` 和 codec `2.0.0-beta5`。
这些是产品工程的参考版本，不代表本组件包已完成同样验证。
当前 IDF 5.5.5/6.1 CI 尚不能作为 S31 的有效编译矩阵，详见 [CI 限制](CI_ZH.md#esp32-s31-覆盖缺口)。

## LR1121 ePaper 2.9 定义范围

`esp32_s3_lr1121_epaper_2_9` 导入产品原有示例板卡配置中的三份 YAML。
已公开的[产品示例](https://github.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/blob/4aa6c326ca3a23317b180c86bbaea15d3327386f/examples/esp-idf/README.md#board-manager-integration)
通过 `waveshare/waveshare_boards` 使用此配置。配置描述 ESP32-S3、兼容 SSD16xx 的
2.9 英寸电子纸、LR1121 无线、PCF85063A RTC、AXP2602 电量计、一颗 WS2812 状态灯和显示电源控制。

下列数值是配置参考，不代表已经完成原理图核对：

| 接口 | 配置的 GPIO / 参数 |
| --- | --- |
| I2C | SDA 13、SCL 14；RTC 地址 `0x51`，电量计地址 `0x62` |
| 共用 SPI2 | SCLK 40、MOSI 45、MISO 46；显示 10 MHz，无线 8 MHz |
| 电子纸 | BUSY 8、复位 9、DC 17、CS 18；电源 GPIO38，高电平有效 |
| LR1121 | DIO1 6、NSS 42、复位 39、BUSY 41 |
| 其他 | RTC 中断 7；WS2812 GPIO21，一颗 GRB 灯；BOOT 输入 GPIO0 |

`status_led` 使用 Board Manager 原生 RMT `led_strip` 设备。
它与四个自定义设备（`epaper_ssd16xx`、`rtc_pcf85063a`、`bat_axp2602`、
`radio_lr1121`）均保留 `init_skip: true`，需要显式初始化。
自定义设备还需要应用提供并注册初始化/反初始化适配代码及驱动依赖，
本组件包未包含这些代码；仅有 YAML 不会实现显示刷新或无线通信。
电子纸与无线共用 `spi_shared`，分别使用独立片选。

`epaper_power_ctrl` 使用原生 GPIO 电源控制设备，**没有**跳过初始化，
其 GPIO38 默认输出高电平；显示设备通过 `power_ctrl_device` 引用它。
应用启动和休眠时需考虑这一电源行为。BOOT 仅声明为 GPIO，未定义按键设备或 SD 卡设备。
AXP2602 的 10 mΩ 采样电阻值是配置默认值，仍需核对实装硬件。

产品仓库已公开[硬件说明](https://github.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/blob/4aa6c326ca3a23317b180c86bbaea15d3327386f/docs/hardware-facts.zh-CN.md)
和 [Rev1.1 原理图](https://github.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/blob/4aa6c326ca3a23317b180c86bbaea15d3327386f/hardware/schematics/ESP32-S3-LR1121-ePaper-2.9-XF-Schematics.pdf)。
上述 GPIO 分配与硬件说明一致；本次导入未完成完整原理图审计或实机验证，
具体 PCB 版本、显示/供电时序、无线配置及电量测量仍需分别验证。
虽然设备名为 `epaper_power_ctrl`，GPIO38 实际控制电子纸、无线、WS2812 和 microSD 的共用电源域。
README 使用已发布的[产品图片](https://raw.githubusercontent.com/waveshareteam/ESP32-S3-LR1121-ePaper-2.9/4aa6c326ca3a23317b180c86bbaea15d3327386f/docs/images/ESP32-S3-LR1121-ePaper-2.9-details-5-2.webp)，
固定到产品仓库提交 `4aa6c326ca3a23317b180c86bbaea15d3327386f`。

## LR1121 OLED 1.54 XF 定义范围

`esp32_s3_lr1121_oled_1_54_xf` 的三份 YAML 来自
[产品出厂示例源码中的板卡配置](https://github.com/waveshareteam/esp32-s3-lr1121-oled-1.54/tree/a0bf73932ef06d047810b17888cc7e33f064680e/firmware/12_factory_demo/components/esp32_s3_lr1121_oled_1_54_support/boards/esp32_s3_lr1121_oled_1_54_xf)，保留原有硬件参数。
对应硬件资料为产品仓库中的[原理图](https://github.com/waveshareteam/esp32-s3-lr1121-oled-1.54/blob/a0bf73932ef06d047810b17888cc7e33f064680e/hardware/schematics/esp32-s3-lr1121-oled-1_54_schematics.pdf)。
配置描述 ESP32-S3 GPIO、共用 I2C、LR1121 SPI、扬声器与麦克风 I2S、按键和单线 SDMMC。

本次提供硬件定义，所有设备保留 `init_skip: true`，由应用按需初始化。
`radio`、`oled`、`rtc`、`battery` 和 `audio` 为自定义设备；本组件包未包含这些设备的
初始化回调、产品适配代码或托管驱动依赖。应用必须提供并注册适配代码，再按名称初始化设备。
仅调用 `esp_board_manager_init()` 不会使这些设备可用。
按键和 SD 使用 Board Manager 内置设备类型，同样需要显式初始化；SD 挂载失败不会自动格式化。

无线适配代码还需初始化单独声明的 `gpio_radio_busy`，它不在自定义 radio 的四项外设列表中。
音频默认使用 16 kHz、STD 扬声器输出与四时隙 TDM 麦克风输入；对讲机的 8 kHz 采集等应用配置
需另行设置。产品工程采用发布后的板卡定义时，应移除应用中重复的板卡 YAML，同时保留所需适配代码。

生成与编译检查不验证设备初始化、共用 I2S 工作情况、射频设置、电源行为或实际接线。
本次导入尚未进行实机测试；声明完整产品支持前，需核对具体 PCB 版本并分别验证这些功能。

## 导入来源与适配

三个 AMOLED 定义来自
[Espressif Brookesia 的 6a087b6 提交](https://github.com/espressif/esp-brookesia/tree/6a087b6d76e989802b72fdb835928b273af76af8/hal/brookesia_hal_boards/boards/waveshare)。
已有 LCD 7 定义仅移动位置，不改变硬件配置。仓库保留板级初始化与 AXP2101 适配代码，
显示、触摸、音频及 GPIO 扩展器驱动继续使用组件库依赖。

本次对导入配置做了以下适配：

- 显式包含 `dev_custom.h`，适配 Board Manager 0.7.2 的自定义设备注册接口。
- 将原有 QSPI 亮度命令提取为公共函数 `waveshare_amoled_set_brightness()`，
  应用可以直接调用它控制面板亮度，详见[亮度控制](INTEGRATIONS_ZH.md)。
- `sdkconfig.defaults.board` 仅保留硬件默认配置，由应用自行选择功能配置和分区表。
- 将 1.8、2.16 的 SD 挂载失败自动格式化关闭，挂载失败会报错，不会清空已有卡片。
- 删除值相同的重复 YAML 键，修正过时的引脚注释。

导入的 PMU 电源轨及充电初始化保持不变。显示、触摸、音频、SD、外设配置和面板初始化
继续保存在各板卡目录。公共函数继续提供运行时 AMOLED 调光，保留原命令格式和百分比换算。
AXP2101 实现继续随板卡维护，因为 1.8 的初始化还涉及 GPIO 扩展器，
合并这些路径属于另一项硬件行为修改。

## 硬件资料核对

下列引脚已对照官方原理图核对；除明确标注 TCA9554 扩展器的引脚外，数字均为 ESP32 GPIO。
三块板卡共用的音频引脚为 BCLK 9、WS 45、DOUT 8、DIN 10、功放使能 46。

| 型号 | I2C SDA / SCL | QSPI 时钟 / CS / 数据 0–3 | LCD 复位 | 触摸复位 / 中断 | 音频 MCLK | SD CLK / CMD / D0 |
| --- | --- | --- | --- | --- | --- | --- |
| 1.75C | 15 / 14 | 38 / 12 / 4, 5, 6, 7 | 1 | 2 / 11 | 16 | 未声明 |
| 1.8 V1 | 15 / 14 | 11 / 12 / 4, 5, 6, 7 | 扩展器 0 | 扩展器 2 / 21 | 16 | 2 / 1 / 3 |
| 2.16 | 15 / 14 | 38 / 12 / 4, 5, 6, 7 | 39 | 40 / 11 | 42 | 2 / 1 / 3 |

1.8 的显示电源使能为 TCA9554 引脚 1。三个配置均使用 7 位地址为 `0x34` 的 AXP2101。
1.75C 和 2.16 使用 CO5300 显示、CST9217 触摸、ES8311 音频输出及 ES7210 输入；
原版 1.8 使用 SH8601、通过 FT5x06 驱动兼容的 FT3168，以及负责输入输出的 ES8311。
三个显示尺寸分别为 466×466、368×448、480×480。

主要资料：

- [1.75C 产品文档](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.75C)及
  [原理图](https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.75C/ESP32-S3-Touch-AMOLED-1.75C-schematic.pdf)。
- [1.8 产品文档](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.8)及
  [原理图](https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8/ESP32-S3-Touch-AMOLED-1.8.pdf)。
- [2.16 产品文档](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-2.16)及
  [原理图](https://files.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-2.16/ESP32-S3-Touch-AMOLED-2.16-Schematic.pdf)。

核对范围是已声明的引脚映射及控制器选择，不代表模拟电路、充电特性、电源与复位时序、
PSRAM、显示颜色、触摸坐标、音频或 SD 实机功能已经验证，需在对应硬件版本上分别测试。

## 1.8 硬件版本兼容评估

导入的 `esp32_s3_touch_amoled_1_8` 配置目前对应 **V1（SH8601 / FT3168）**。
V2 使用 CO5300 / CST820，不能直接将未适配的配置视为兼容 V2。
可通过产品背面标签识别版本。

采用同一型号运行时识别 V1/V2 是可行的。微雪现有
[托管 BSP 实现](https://github.com/waveshareteam/Waveshare-ESP32-components/blob/d081959d3841e0b370c2957c122bf8604ab42bc8/bsp/esp32_s3_touch_amoled_1_8/esp32_s3_touch_amoled_1_8.c)
已通过探测触摸控制器，选择 FT5x06 或 CST816S 系列驱动，并给 CO5300 设置 16 像素横向偏移。
CST816S 是兼容驱动的 API 名称，V2 实际安装的是 CST820。

移植到本组件包需加入两套触摸依赖，在 Board Manager 探测 I2C 之前完成复位，
确保探测地址与实际驱动一致，并处理显示偏移。电源、音频和 SD 定义可继续共用。
这是兼容性评估，尚未实现 V2 支持；实现后仍需对两个硬件版本分别进行显示和触摸实测。

## 来源许可证

Brookesia 板卡包采用 Apache-2.0，导入文件保留原 SPDX 声明，包括初始化代码中的 CC0-1.0。
请保留原版权头。托管驱动遵循各自的许可证，本组件包不改变其授权。
详见[上游板卡包许可证](https://github.com/espressif/esp-brookesia/blob/6a087b6d76e989802b72fdb835928b273af76af8/hal/brookesia_hal_boards/license.txt)及
[CC0-1.0 条款](https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)。
