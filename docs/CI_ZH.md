# 板卡包 CI

[English](CI.md) · [首页](../README_ZH.md)

## 编译约定

[工作流](https://github.com/waveshareteam/waveshare_boards/blob/main/.github/workflows/ci.yml)
发现第一方板卡定义，并为每块选中的板卡编译 `ci/test_app`。
它不扫描上游板卡包，也不发布固件产物。

`ci/versions.json` 选择两个受支持版本线的稳定版本：ESP-IDF **v5.5.5** 和 **v6.1**。
每次运行从组件库解析一次 Board Manager 版本，按根目录清单的范围选择最早和最新的稳定已发布版本，
相同版本合并。组件库查询失败会使任务失败，每个矩阵项均记录明确版本。

Board Manager 初始最低版本为 **0.7.2**，与参考板卡包模板一致。
当前最低版与最新版相同，普通板卡模式在两个 IDF 版本线上共八项编译，
另加三个 AMOLED 型号在 IDF 6.1 下的可选 Brookesia 适配测试，总计十一项。
调整最低版本前需要明确兼容性变更并测试。
不支持的芯片能力配置或生成失败会使 CI 失败，不会被计为编译成功。

IDF 5.5 的 Component Manager 固定为 **2.5.0**，因为参考模板记录了 **2.4.8** 对 Board Manager
Kconfig 条件依赖的处理问题。辅助工具为 **esp-bmgr-assist 0.8.3**。
IDF 6.1 使用符合官方 3.0.x 约束的 Component Manager **3.0.3**，以支持 CMake 接口版本 5；2.5.0 不支持该接口。
升级矩阵时重新评估这些工具版本，并在两个 IDF 版本线上重新验证生成和编译。

Board Manager 0.7.2 可在托管安装、`components/` 本地克隆和清单 `override_path`
依赖三种形式下识别 `waveshare_boards`。集成测试使用覆盖依赖，不传 `-c` 参数。
组件包内采用 `boards/<完整型号>/`：本地扫描从应用的 `components/` 根目录开始，
再增加一层芯片目录会超过其三层搜索限制。详见[目录与源码依据](BOARDS_ZH.md)。

可选适配使用 HAL interface / lib_utils 0.8.2。上游 lib_utils 0.8 要求 IDF 6.0–6.2，
因此 `ci/versions.json` 通过 `brookesia_idf` 声明其原生支持的测试版本线。
普通板卡模式仍覆盖两个版本线，三个保留的应用配置标识需要额外适配测试的板卡。
独立适配编译不会套用完整应用配置。详见[集成覆盖与上游约束](INTEGRATIONS_ZH.md)。

## 变更路由

每个 PR 都运行轻量的元数据、单元测试、导航和打包检查。
`Board pack checks` 是可用于分支保护的固定汇总状态。

| 变更 | 板卡编译 |
| --- | --- |
| 根目录或嵌套 Markdown、文档图片 | 不编译 |
| Issue 表单、许可证、忽略规则 | 不编译 |
| 单个板卡目录内的文件 | 对应板卡 |
| CI 脚本、测试、工作流、集成组件、根清单或 CMake | 全部板卡 |
| 删除或重命名板卡路径 | 同时考虑新旧路径；已移除路径可能选择全部剩余板卡 |
| 固件文档、源码、二进制、归档 | 单独报告，不触发板卡编译 |
| 未识别的非文档输入 | 全部板卡，并报告路径供检查 |
| 空 Git 差异或无法读取差异 | 报错，不静默回退 |
| 手动或发布前验证 | 全部板卡 |

`ci/scripts/board_pack.py matrix` 写入 `matrix` 与 `has_builds` 输出，
编译任务直接使用这些输出。新提交会取消该 PR 的过时运行；发布运行串行执行，
不会因后续 PR 提交被取消。

`check_docs.py` 仅检查根目录和 docs 的双语配对、本地导航目标以及主页章节对称性，
不代表完整的 Markdown 所有权、锚点、同语言路由或隐私信息审计。
修改公开文档时仍需检查这些事项。

## 本地复现

将仓库克隆到名为 `waveshare_boards` 的目录，激活所需的 ESP-IDF 环境，为 IDF 5.5 安装 Component Manager 2.5.0，为 IDF 6.1 安装 3.0.3，然后在该目录执行：

```bash
python -m pip install PyYAML==6.0.3 esp-bmgr-assist==0.8.3
python -m unittest discover -s ci/scripts -p 'test_*.py' -v
python ci/scripts/board_pack.py check
python scripts/update_supported_boards_table.py --check
python ci/scripts/check_docs.py
python ci/scripts/board_pack.py matrix --all
python ci/scripts/board_pack.py pin 0.7.2
python ci/scripts/board_pack.py integration none
idf.py -C ci/test_app bmgr -l
idf.py -C ci/test_app bmgr -b esp32_s3_touch_lcd_7
idf.py -C ci/test_app build
compote component pack --name waveshare_boards
```

每个 IDF / Board Manager 组合使用独立干净副本，避免生成组件、依赖锁和 sdkconfig 跨环境混用。
离线复现矩阵时，可在 matrix 命令末尾添加 `--bmgr 0.7.2`。
检查 PR 差异时，在完整 Git 历史下使用 `matrix --base origin/main`。
集成测试清单中的固定版本属于临时构建输入，只有主动调整基线时才提交其变化。

## 验证依据与边界

板卡 YAML 和 `setup_device.c` 是板级源码，不是通用驱动副本。
GT911 驱动继续使用板卡原有的托管依赖 **1.2.1** 约束；更换版本前需在两个框架版本线及硬件上验证。
初始化函数校验 Board Manager 选出的 I2C 地址，然后调用托管驱动。

编译成功证明 API 和生成代码兼容，不证明接线、PSRAM 配置、触摸复位时序或实机运行正确。
官方原理图链接与 AMOLED 迁移核对内容见[板卡说明](BOARDS_ZH.md)。
实机运行和应用层集成需要分别验证。

主要资料：[IDF v5.5.5](https://github.com/espressif/esp-idf/releases/tag/v5.5.5)、
[IDF v6.1](https://github.com/espressif/esp-idf/releases/tag/v6.1)、
[5.5 到 6.0 迁移](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32s3/migration-guides/release-6.x/6.0/index.html)、
[6.0 到 6.1 迁移](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32s3/migration-guides/release-6.x/6.1/index.html)、
[Board Manager](https://components.espressif.com/components/espressif/esp_board_manager)。
