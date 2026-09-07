# Publishing the board pack

[简体中文](PUBLISHING_ZH.md) · [Home](../README.md)

## Component identity

The GitHub repository is `waveshareteam/waveshare_boards`. The registry identity is
**`waveshare/waveshare_boards`**, using the same namespace as the
[Waveshare component upload workflow](https://github.com/waveshareteam/Waveshare-ESP32-components/blob/master/.github/workflows/upload_component.yml).
Keep the `esp_board_manager`, `board_manager`, and `boards` tags in the manifest
so Board Manager can discover the pack.

Pull requests run CI without publishing. A push or merge to `main` automatically
starts the release workflow. A completed GitHub Release for the manifest version
makes the publication jobs skip; publishing a new version runs the full build matrix.
`compote component pack --name waveshare_boards` validates the package locally without
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
   and merge the PR after all required checks pass. The initial version is `0.1.0`.
2. **Publish board pack** runs automatically on `main`. For a new version, it
   compiles every board, creates `v<version>` on that exact commit, uploads
   `waveshare/waveshare_boards`, and creates the GitHub Release after the registry
   confirms success. Release validation always builds the full matrix, including
   when the ordinary CI change classifier selects fewer boards.
3. Verify the registry version page and install it into a fresh application:

   ```bash
   idf.py add-dependency "waveshare/waveshare_boards==0.1.0"
   ```

Merging with an already released version does not publish again. Increase the
manifest version when subsequent changes should reach registry users. Published
versions and existing tags are never overwritten or moved. An existing tag for
another commit stops the upload instead of publishing different source under it.

For an upload failure, fix credentials or permissions and rerun the failed jobs
on the same workflow commit. If a tag was already created, a manual real upload
can also run from that tag with **dry_run** cleared. Manual real uploads accept
`main` or the matching version tag, and require the source to belong to `main`.
If the upload succeeded but GitHub Release creation failed, rerun only the failed
`release` job; the successful upload job is kept. Do not re-upload an immutable
registry version. Release runs are serialized and are not cancelled by later merges.

The [Compote upload reference](https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/compote_cli.html#upload)
describes registry validation and duplicate-version behavior.
