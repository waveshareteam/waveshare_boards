// GitHub release operations are separate from registry credentials and uploads.
async function shouldPublish({github, context, core}, tag, dryRun) {
    if (dryRun) return true;
    try {
        const {data} = await github.rest.repos.getReleaseByTag({...context.repo, tag});
        if (data.draft) throw new Error(`Finish or remove the existing draft release ${tag} before publishing`);
        core.info(`${tag} already has a completed release; skipping publication`);
        return false;
    } catch (error) {
        if (error.status === 404) return true;
        throw error;
    }
}

async function ensureTag({github, context}, tag) {
    let target;
    try {
        const {data} = await github.rest.git.getRef({...context.repo, ref: `tags/${tag}`});
        target = data.object;
    } catch (error) {
        if (error.status !== 404) throw error;
        await github.rest.git.createRef({...context.repo, ref: `refs/tags/${tag}`, sha: context.sha});
        return;
    }
    while (target.type === 'tag') {
        const {data} = await github.rest.git.getTag({...context.repo, tag_sha: target.sha});
        target = data.object;
    }
    if (target.type !== 'commit' || target.sha !== context.sha) {
        throw new Error(`${tag} points to another commit; retry the tagged run or use a new version`);
    }
}

module.exports = {shouldPublish, ensureTag};
