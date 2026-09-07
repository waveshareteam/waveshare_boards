# Publishing the board pack

[简体中文](PUBLISHING_ZH.md) · [Home](../README.md)

## Component identity

The GitHub repository is `waveshareteam/waveshare-boards`. The registry identity is
**`waveshare/waveshare-boards`**, using the same namespace as the
[Waveshare component upload workflow](https://github.com/waveshareteam/Waveshare-ESP32-components/blob/master/.github/workflows/upload_component.yml).
Keep the `esp_board_manager`, `board_manager`, and `boards` tags in the manifest
so Board Manager can discover the pack.

No registry version is created by a pull request, a merge, or normal CI.
`compote component pack --name waveshare-boards` validates the package locally without
a token. Generated archives remain under ignored `dist/`.

## Configure credentials once

1. Sign in to [ESP Component Registry](https://components.espressif.com) using
   the GitHub account that will publish the component.
2. In **Permissions**, confirm that account can create components in `waveshare`.
   The namespace owner can grant a namespace role. GitHub organization membership
   does not automatically grant registry permissions. For this new component,
   coordinate with the existing namespace owner rather than requesting a second
   namespace. See [registry roles](https://docs.espressif.com/projects/idf-component-manager/en/latest/publish/explanation_registry_roles.html).
3. Open the account's **API Tokens** page, create a token with the
   **`write:components`** scope, and copy it directly into GitHub's secret field.
4. In this repository's **Settings → Environments**, configure the
   `component-registry` environment. Add an environment secret named
   **`IDF_COMPONENT_API_TOKEN`**. A repository Actions secret with that same name
   also works. Configure release reviewers or permitted deployment references
   according to the team's policy.
5. Never paste the token into documentation, issue comments, or committed files.

The workflow fixes the namespace and component name, so no repository variable
is needed. The automatic GitHub `GITHUB_TOKEN` cannot replace the registry token.

## Validate an upload

After the workflow is merged, open **Actions → Publish board pack → Run workflow**.
Choose `main` and keep **dry_run** checked. This first runs the entire build matrix,
then uses the configured secret to upload for registry validation without creating
a public version. A missing secret fails this manually requested publication check;
it does not affect ordinary PR CI.

## Publish a version

1. Update `version` in `idf_component.yml`, keep both language pages current,
   and merge the PR after all required checks pass.
2. Create a `v<version>` tag on that reviewed commit, for example `v0.1.0`.
3. Run **Publish board pack** from that tag and clear **dry_run**. The workflow
   checks that the tag matches the manifest and that the commit belongs to `main`,
   reruns all board builds, then uploads `waveshare/waveshare-boards`.
4. Verify the registry version page and install it into a fresh application.

Published versions are immutable. A duplicate version fails rather than silently
accepting different source. Make a new version for subsequent changes. See the
[Compote upload reference](https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/compote_cli.html#upload).
