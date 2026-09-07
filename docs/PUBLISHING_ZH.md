# 板卡包发布

[English](PUBLISHING.md) · [首页](../README_ZH.md)

## 组件名称

GitHub 仓库为 `waveshareteam/waveshare_boards`，在线组件库名称为 **`waveshare/waveshare_boards`**，
沿用[微雪组件上传工作流](https://github.com/waveshareteam/Waveshare-ESP32-components/blob/master/.github/workflows/upload_component.yml)
使用的命名空间。清单中保留 `esp_board_manager`、`board_manager` 和 `boards` 标签，
便于 Board Manager 发现组件包。

PR 只运行 CI，不发布组件。推送或合并到 `main` 后会自动启动发布流程。
如果清单版本已有完整的 GitHub Release，则跳过发布；新版本先运行完整编译矩阵。
`compote component pack --name waveshare_boards` 可以在无令牌情况下验证本地打包，
生成的归档存放在已忽略的 `dist/` 目录。

## 一次性配置凭据

1. 使用将要发布组件的 GitHub 账号登录 [ESP Component Registry](https://components.espressif.com)。
2. 在 **Permissions** 中确认该账号有权在 `waveshare` 中创建组件。
   命名空间所有者可以授予命名空间角色；GitHub 组织成员身份不会自动获得组件库权限。
   这是一个新组件，请与现有命名空间所有者协作，不需要再申请第二个命名空间。
   参阅[组件库角色说明](https://docs.espressif.com/projects/idf-component-manager/en/latest/publish/explanation_registry_roles.html)。
3. 打开账号的 **API Tokens** 页面，创建具有 **`write:components`** 权限的令牌，
   将它直接复制到 GitHub 的 Secret 输入框。
4. 在本仓库 **Settings → Environments** 中配置 `component-registry` 环境，
   添加名为 **`IDF_COMPONENT_API_TOKEN`** 的 Environment secret。
   同名的 Repository Actions secret 也可以使用。
   按团队制度设置发布审核人或允许部署的分支、标签。
5. 不要把令牌粘贴到文档、Issue 评论或被提交的文件中。

工作流已经固定命名空间和组件名称，无需额外填写仓库变量。
GitHub 自动提供的 `GITHUB_TOKEN` 不能替代组件库令牌。

## 验证上传

工作流合并后，打开 **Actions → Publish board pack → Run workflow**，
选择 `main` 并保持 **dry_run** 勾选。工作流先运行完整编译矩阵，
然后使用已配置的 Secret 上传到组件库做校验，不创建公开版本。
缺少 Secret 时，这项手动发布校验会失败，但不影响普通 PR CI。

## 发布版本

1. 更新 `idf_component.yml` 中的 `version`，维护双语文档，所有必需检查通过后合并 PR。
   初始版本为 `0.1.0`。
2. `main` 上自动运行 **Publish board pack**。对于新版本，工作流先编译全部板卡，
   再在该确切提交上创建 `v<version>` 标签，上传 `waveshare/waveshare_boards`，
   组件库确认成功后创建 GitHub Release。发布前始终运行完整矩阵，
   即使普通 CI 的变更分类器只选择了部分板卡。
3. 检查组件库版本页面，并在全新应用中验证安装：

   ```bash
   idf.py add-dependency "waveshare/waveshare_boards==0.1.0"
   ```

合并时若版本已经发布，不会再次上传。后续修改需要交付给组件库用户时，必须增加清单版本号。
已发布版本不可覆盖，已有标签不会被移动。如果版本标签指向其他提交，工作流会停止上传，
不会用同一标签发布不同源码。

上传失败时，修正凭据或权限，然后在原工作流提交上重新运行失败的任务。
如果标签已经创建，也可从该标签手动运行并取消勾选 **dry_run**。
手动真实发布只接受 `main` 或匹配版本的标签，并要求提交属于 `main`。
如果组件上传成功而 GitHub Release 创建失败，只重跑失败的 `release` 任务，
保留已经成功的上传任务，避免重复上传不可变版本。发布流程串行执行，不会被后续合并取消。

[Compote 上传说明](https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/compote_cli.html#upload)
介绍了组件库校验和重复版本的处理方式。
