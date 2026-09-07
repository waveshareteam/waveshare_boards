# 板卡包发布

[English](PUBLISHING.md) · [首页](../README_ZH.md)

## 组件名称

GitHub 仓库为 `waveshareteam/waveshare-boards`，在线组件库名称为 **`waveshare/waveshare-boards`**，
沿用[微雪组件上传工作流](https://github.com/waveshareteam/Waveshare-ESP32-components/blob/master/.github/workflows/upload_component.yml)
使用的命名空间。清单中保留 `esp_board_manager`、`board_manager` 和 `boards` 标签，
便于 Board Manager 发现组件包。

提交 PR、合并或普通 CI 都不会发布组件库版本。
`compote component pack --name waveshare-boards` 可以在无令牌情况下验证本地打包，
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
2. 在审核通过的提交上创建 `v<version>` 标签，例如 `v0.1.0`。
3. 从该标签运行 **Publish board pack**，取消勾选 **dry_run**。
   工作流检查标签与清单版本一致且提交属于 `main`，重新编译全部板卡，
   再上传 `waveshare/waveshare-boards`。
4. 检查组件库版本页面，并在全新应用中验证安装。

已发布版本不可覆盖。重复版本会失败，不会把不同源码静默视为成功。
后续变更使用新版本号，参阅 [Compote 上传说明](https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/compote_cli.html#upload)。
