# 板卡组织与迁移说明

[English](BOARDS.md) · [首页](../README_ZH.md)

## 目录结构

仓库与组件统一命名为 `waveshare_boards`，组件库标识为
`waveshare/waveshare_boards`。所有板卡定义统一放在：

```text
boards/
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
