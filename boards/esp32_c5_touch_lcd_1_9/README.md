# ESP32-C5-Touch-LCD-1.9

[简体中文](README_ZH.md)

Waveshare ESP32-C5-WROOM-1-N16R8 board with a 1.9-inch SPI LCD.
This profile is imported from the product examples and preserves their named
peripherals and custom device metadata. Applications implement product policy;
custom devices use `init_skip: true` so generic Board Manager initialization does
not implicitly initialize the display, sensors, storage or battery gauge.

- I2C: port 0, SDA GPIO2, SCL GPIO3, internal pull-ups enabled.
- SPI2 shared by LCD and TF: MOSI5, MISO6, SCLK7; TF CS4.
- LCD: ST7789-compatible, 170 x 320, X gap35; CS23/DC24/RESET26, backlight10.
- Touch: CST816D-compatible 0x15, RESET8/INT0.
- IMU: QMI8658 0x6B, INT1; gauge: AXP2602 0x62, INT25, 10 milliohm shunt.
- BOOT28 and power-key9 are inputs. The power latch is not a software output.

Reference: [product documentation](https://www.waveshare.com/wiki/ESP32-C5-Touch-LCD-1.9).
Pin assignments were compared with the supplied product schematic. LCD controller,
visible geometry, gap and touch compatibility retain the product implementation;
the board owner has confirmed display, touch, sensor, storage and sleep/wakeup
functionality using the product firmware on the correct ESP32-C5 rev v1.0 board.
These observations do not establish a manufacturer-approved panel specification. Flash/PSRAM and application features are
configured by the consuming application, not inferred from profile generation.

Validation and migration: this change adds a profile and does not certify physical
operation of arbitrary consuming applications. The product repository must retain its local definition until this PR
is merged, the component registry publishes it and consuming application tests pass.
