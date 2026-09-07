import importlib.util
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

import yaml

import board_pack
import check_docs
import check_release

ROOT = board_pack.ROOT
spec = importlib.util.spec_from_file_location('catalog', ROOT / 'scripts/update_supported_boards_table.py')
catalog = importlib.util.module_from_spec(spec)
spec.loader.exec_module(catalog)


class RepositoryCheckTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for filename, peer in [('README.md', 'README_ZH.md'), ('README_ZH.md', 'README.md')]:
            (self.root / filename).write_text(f'# Fixture\n\n[Language]({peer})\n\n## 📚 Docs\n')

    def test_documentation_pairs_navigation_and_symmetry(self):
        self.assertEqual([], check_docs.check(self.root))
        (self.root / 'README.md').write_text('[Language](README_ZH.md)\n\n[Missing](absent.md)\n## 🚀 Start\n')
        errors = check_docs.check(self.root)
        self.assertTrue(any('local link' in e for e in errors))
        self.assertTrue(any('icons' in e for e in errors))
        (self.root / 'README_ZH.md').unlink()
        self.assertTrue(any('companion' in e for e in check_docs.check(self.root)))

    def test_catalog_replaces_stale_boards_and_check_never_writes(self):
        directory = self.root / 'alpha'
        directory.mkdir()
        (directory / 'board_info.yaml').write_text('board: alpha\nchip: esp32s3\n')
        (directory / 'board_devices.yaml').write_text('devices:\n  - name: lcd\n')
        (directory / 'board_peripherals.yaml').write_text('peripherals: []\n')
        for name in ('README.md', 'README_ZH.md'):
            (self.root / name).write_text(catalog.BEGIN + '\n| removed |\n' + catalog.END)
        before = (self.root / 'README.md').read_text()
        self.assertEqual(2, len(catalog.update(self.root, check=True)))
        self.assertEqual(before, (self.root / 'README.md').read_text())
        catalog.update(self.root)
        self.assertEqual([], catalog.update(self.root, check=True))
        self.assertNotIn('removed', (self.root / 'README.md').read_text())
        self.assertIn('`lcd`', (self.root / 'README.md').read_text())
        self.assertIn('设备定义', (self.root / 'README_ZH.md').read_text())
        (self.root / 'README.md').write_text('missing markers')
        with self.assertRaises(ValueError):
            catalog.update(self.root)

    def test_real_publish_requires_main_or_matching_tag_and_main_ancestry(self):
        (self.root / 'idf_component.yml').write_text('version: "0.1.0"\n')
        check_release.check(self.root, 'true', 'refs/heads/feature')
        for ref in ('refs/heads/feature', 'refs/tags/v0.2.0'):
            with self.assertRaises(ValueError):
                check_release.check(self.root, 'false', ref)
        with patch('check_release.subprocess.run') as run:
            self.assertEqual('0.1.0', check_release.check(self.root, 'false', 'refs/heads/main'))
            check_release.check(self.root, 'false', 'refs/tags/v0.1.0')
            self.assertEqual(['git', 'merge-base', '--is-ancestor', 'HEAD', 'origin/main'], run.call_args.args[0])
            run.side_effect = subprocess.CalledProcessError(1, 'git')
            with self.assertRaises(subprocess.CalledProcessError):
                check_release.check(self.root, 'false', 'refs/tags/v0.1.0')

    def test_release_rejects_invalid_versions_and_modes(self):
        for version in ('01.0.0', '0.1.0-rc1', 'bad;command'):
            (self.root / 'idf_component.yml').write_text(f'version: {version!r}\n')
            with self.assertRaises(ValueError):
                check_release.check(self.root, 'true', 'refs/heads/main')
        with self.assertRaises(ValueError):
            check_release.check(self.root, 'yes', 'refs/heads/main')

    def test_release_cli_emits_manifest_version(self):
        output = self.root / 'outputs'
        result = subprocess.run([sys.executable, 'ci/scripts/check_release.py'], cwd=ROOT,
                                env={**os.environ, 'DRY_RUN': 'true', 'GITHUB_OUTPUT': str(output)},
                                capture_output=True, text=True)
        self.assertEqual(0, result.returncode, result.stderr)
        version = yaml.safe_load((ROOT / 'idf_component.yml').read_text())['version']
        self.assertEqual(f'version={version}\ntag=v{version}\n', output.read_text())

    def test_exact_lightweight_workflow_commands(self):
        for command in [['ci/scripts/board_pack.py', 'check'],
                        ['scripts/update_supported_boards_table.py', '--check'],
                        ['ci/scripts/check_docs.py'], ['ci/scripts/check_release.py']]:
            result = subprocess.run([sys.executable, *command], cwd=ROOT,
                                    env={**os.environ, 'DRY_RUN': 'true'},
                                    capture_output=True, text=True)
            self.assertEqual(0, result.returncode, result.stdout + result.stderr)

    def test_workflow_consumes_classifier_and_separates_credentials(self):
        ci = yaml.load((ROOT / '.github/workflows/ci.yml').read_text(), Loader=yaml.BaseLoader)
        self.assertIn('pull_request', ci['on'])
        self.assertNotIn('paths', ci['on']['pull_request'] or {})
        jobs = ci['jobs']
        self.assertIn('has_builds', jobs['build']['if'])
        self.assertIn('needs.validate.outputs.matrix', jobs['build']['strategy']['matrix'])
        self.assertIn('always()', jobs['result']['if'])
        self.assertNotIn('secrets.', (ROOT / '.github/workflows/ci.yml').read_text())
        publish = yaml.load((ROOT / '.github/workflows/publish.yml').read_text(), Loader=yaml.BaseLoader)
        self.assertEqual(['main'], publish['on']['push']['branches'])
        self.assertEqual('true', publish['on']['workflow_dispatch']['inputs']['dry_run']['default'])
        self.assertEqual('component-registry', publish['jobs']['upload']['environment'])
        self.assertEqual(['prepare', 'validate'], publish['jobs']['upload']['needs'])
        self.assertEqual('true', publish['jobs']['validate']['with']['full_validation'])
        self.assertIn('needs.prepare.outputs.publish', publish['jobs']['validate']['if'])
        self.assertEqual(['prepare', 'upload'], publish['jobs']['release']['needs'])
        self.assertIn('!inputs.dry_run', publish['jobs']['release']['if'])
        self.assertEqual('read', publish['permissions']['contents'])
        self.assertEqual('write', publish['jobs']['upload']['permissions']['contents'])
        scope = next(step for step in jobs['validate']['steps'] if step.get('id') == 'scope')
        self.assertIn('FULL_VALIDATION', scope['env'])
        self.assertIn('matrix --all', scope['run'])


if __name__ == '__main__':
    unittest.main()
