# ESP32-C5-Touch-LCD-1.9

[English](README.md)

微雪 ESP32-C5-WROOM-1-N16R8、1.9 英寸 SPI LCD 开发板。
本配置保留产品示例的外设名称和自定义设备元数据；自定义设备使用
`init_skip: true`，显示、传感器、存储和电量计的初始化策略由应用实现。

- I2C：端口 0，SDA GPIO2、SCL GPIO3，启用内部上拉。
- LCD 与 TF 共用 SPI2：MOSI5、MISO6、SCLK7；TF CS4。
- LCD：ST7789 兼容，170 x 320，X 偏移35；CS23/DC24/RESET26，背光10。
- 触摸：CST816D 兼容，地址0x15，RESET8/INT0。
- IMU：QMI8658 0x6B、INT1；电量计：AXP2602 0x62、INT25、10毫欧采样电阻。
- BOOT28 与电源按键9均为输入，不通过软件驱动电源锁存按键。

参考：[产品文档](https://www.waveshare.com/wiki/ESP32-C5-Touch-LCD-1.9)。
引脚与提供的产品原理图进行了核对。LCD 控制器、显示尺寸、偏移和触摸兼容性
沿用产品实现。持有人已在正确的 ESP32-C5 rev v1.0 板卡上确认产品固件的显示、
触摸、传感器、存储和睡眠唤醒基本功能；这些结果不等于屏厂参数认证。Flash、PSRAM 和应用功能由使用方配置。

本变更新增板型，不代表任意使用方应用均已完成硬件验收。产品仓库应在本 PR 合入、组件注册表
发布且使用方验证通过后，再删除自己的本地板定义。
