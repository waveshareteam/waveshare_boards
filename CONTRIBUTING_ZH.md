# 贡献指南

[English](CONTRIBUTING.md) · [首页](README_ZH.md)

通过 Pull Request 提交板卡定义、修复或文档。

1. 将每块板卡的 `board_info.yaml`、`board_peripherals.yaml`、`board_devices.yaml`
   及可选的初始化源码保存在同一目录。名称必须唯一并与目录名相同，最多嵌套三层。
2. 硬件修改应基于对应产品版本的原理图或官方硬件资料。
   提供来源链接或允许公开的参考文件，说明核对内容，实机测试与编译结果分别报告。
3. 板级适配代码留在本地，通用驱动通过托管依赖引入，说明版本约束及验证范围。
4. 重新生成[板卡目录](README_ZH.md#board-catalog)，同步维护英文和简体中文文档，保留已有许可证声明。
5. 运行 [CI 验证](docs/CI_ZH.md)中的命令，确认最新 PR 提交的全部所选检查通过。
   不提交生成组件、构建目录、凭据、个人路径或设备标识。

PR 描述应说明板卡、行为变化、测试过的框架和 Board Manager 版本，以及尚未完成的硬件验证。
组件发布步骤见[发布指南](docs/PUBLISHING_ZH.md)。
