const {test} = require('node:test');
const assert = require('node:assert/strict');
const {shouldPublish, ensureTag} = require('./github_release.cjs');
const context = {repo: {owner: 'vendor', repo: 'boards'}, sha: 'tested-commit'};
const missing = () => { throw Object.assign(new Error('Missing'), {status: 404}); };
const unavailable = () => { throw Object.assign(new Error('Unavailable'), {status: 503}); };
function fixture(repos = {}, git = {}) {
    return {context, github: {rest: {repos, git}}, core: {info() {}}};
}

test('dry run does not query or mutate releases', async () => {
    assert.equal(await shouldPublish(fixture(), 'v0.1.0', true), true);
});
test('new version proceeds, completed version skips', async () => {
    assert.equal(await shouldPublish(fixture({getReleaseByTag: missing}), 'v0.1.0', false), true);
    assert.equal(await shouldPublish(fixture({getReleaseByTag: async () => ({data: {draft: false}})}), 'v0.1.0', false), false);
});
test('draft and unavailable release status do not authorize publishing', async () => {
    await assert.rejects(shouldPublish(fixture({getReleaseByTag: async () => ({data: {draft: true}})}), 'v0.1.0', false), /draft/);
    await assert.rejects(shouldPublish(fixture({getReleaseByTag: unavailable}), 'v0.1.0', false), /Unavailable/);
});
test('new tag points only to the tested workflow commit', async () => {
    const calls = [];
    await ensureTag(fixture({}, {getRef: missing, createRef: async args => calls.push(args)}), 'v0.1.0');
    assert.deepEqual(calls, [{...context.repo, ref: 'refs/tags/v0.1.0', sha: context.sha}]);
});
test('matching lightweight and annotated tags are reusable without mutation', async () => {
    await ensureTag(fixture({}, {getRef: async () => ({data: {object: {type: 'commit', sha: context.sha}}})}), 'v0.1.0');
    await ensureTag(fixture({}, {
        getRef: async () => ({data: {object: {type: 'tag', sha: 'annotated'}}}),
        getTag: async () => ({data: {object: {type: 'commit', sha: context.sha}}})
    }), 'v0.1.0');
});
test('different source cannot move an existing version tag', async () => {
    await assert.rejects(ensureTag(fixture({}, {getRef: async () => ({data: {object: {type: 'commit', sha: 'other'}}})}), 'v0.1.0'), /another commit/);
});
test('tag lookup network failure cannot be treated as missing', async () => {
    await assert.rejects(ensureTag(fixture({}, {getRef: unavailable}), 'v0.1.0'), /Unavailable/);
});
