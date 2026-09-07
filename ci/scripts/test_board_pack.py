import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch


import board_pack as bp


class BoardPackTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.add_board('alpha')
        self.add_board('display/beta')
        self.write('ci/versions.json', (bp.ROOT / 'ci/versions.json').read_text())
        self.write('idf_component.yml', 'dependencies:\n  espressif/esp_board_manager:\n    version: ">=0.7.2"\n')
        self.boards = bp.discover(self.root)

    def write(self, name, content):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding='utf-8')

    def add_board(self, directory, name=None):
        self.write(f'{directory}/board_info.yaml', f'board: {name or Path(directory).name}\nchip: esp32s3\n')
        self.write(f'{directory}/board_devices.yaml', 'devices: []\n')
        self.write(f'{directory}/board_peripherals.yaml', 'peripherals: []\n')

    def git(self, *args):
        return subprocess.check_output(['git', *args], cwd=self.root, stderr=subprocess.DEVNULL).decode().strip()

    def initialize_git(self):
        self.git('init', '-q')
        self.git('config', 'user.name', 'Test')
        self.git('config', 'user.email', 'test@example.invalid')
        self.git('add', '.')
        self.git('commit', '-qm', 'Initial fixture')
        return self.git('rev-parse', 'HEAD')

    def cli(self, *args):
        # Copy the real CLI into a fixture repository to exercise its default ROOT.
        self.write('ci/scripts/board_pack.py', (bp.ROOT / 'ci/scripts/board_pack.py').read_text())
        output = self.root / 'outputs.txt'
        result = subprocess.run([sys.executable, 'ci/scripts/board_pack.py', *args],
                                cwd=self.root, env={**os.environ, 'GITHUB_OUTPUT': str(output)},
                                capture_output=True, text=True)
        values = dict(line.split('=', 1) for line in output.read_text().splitlines()) if output.exists() else {}
        return result, values

    def test_discovery_excludes_generated_and_upstream(self):
        for root in ('ci', 'managed_components', 'components', 'firmware', 'libraries', 'integrations', '.hidden'):
            self.add_board(f'{root}/ignored')
        self.assertEqual(['alpha', 'beta'], [b['board'] for b in bp.discover(self.root)])

    def test_invalid_or_duplicate_board_fails(self):
        self.add_board('other/alpha')
        with self.assertRaises(ValueError):
            bp.discover(self.root)
        shutil.rmtree(self.root / 'other')
        self.add_board('wrong', name='different')
        with self.assertRaises(ValueError):
            bp.discover(self.root)

    def test_missing_or_malformed_yaml_fails(self):
        self.write('alpha/board_info.yaml', '[]')
        with self.assertRaises(ValueError):
            bp.discover(self.root)
        self.add_board('alpha')
        (self.root / 'alpha/board_devices.yaml').unlink()
        with self.assertRaises(FileNotFoundError):
            bp.discover(self.root)

    def test_depth_is_checked(self):
        self.add_board('boards/esp32s3/deep')
        with self.assertRaises(ValueError):
            bp.discover(self.root)

    def test_grouped_board_component_routes_through_workflow_cli(self):
        self.add_board('boards/gamma')
        base = self.initialize_git()
        self.write('boards/gamma/components/custom/source.c', 'int value;\n')
        self.git('add', '.')
        self.git('commit', '-qm', 'Board component fixture')
        result, values = self.cli('matrix', '--base', base, '--bmgr', '0.7.2')
        self.assertEqual(0, result.returncode, result.stderr)
        cells = json.loads(values['matrix'])['include']
        self.assertEqual(2, len(cells))
        self.assertEqual({'gamma'}, {c['board'] for c in cells})
        routed = bp.route(bp.discover(self.root), ['boards/gamma/components/custom/CHANGELOG.md'])
        self.assertTrue(routed['docs_only'])
        self.assertEqual([], routed['boards'])

    def test_docs_beside_source_select_zero(self):
        for path in ('README.md', 'alpha/README.md', 'ci/test_app/README.md',
                     'examples/esp-idf/app/README.md', 'examples/arduino/sketch/README.md',
                     'libraries/lib/README.md', 'docs/catalog.svg'):
            with self.subTest(path=path):
                result = bp.route(self.boards, [path])
                self.assertEqual([], result['boards'])
                self.assertTrue(result['docs_only'])

    def test_direct_shared_global_and_governance(self):
        selected = bp.route(self.boards, ['alpha/setup_device.c'])
        self.assertEqual(['alpha'], [b['board'] for b in selected['boards']])
        for path in ('ci/test_app/main/main.c', 'idf_component.yml', 'ci/versions.json',
                     '.github/workflows/ci.yml', 'ci/scripts/test_board_pack.py'):
            self.assertEqual(self.boards, bp.route(self.boards, [path])['boards'])
        for path in ('LICENSE', '.gitignore', '.github/ISSUE_TEMPLATE/bug_report.yml'):
            self.assertEqual([], bp.route(self.boards, [path])['boards'])
        self.assertEqual(['unclassified.c'], bp.route(self.boards, ['unclassified.c'])['unknown'])

    def test_mixed_firmware_scope(self):
        paths = ['firmware/README.md', 'firmware/app/main.c', 'firmware/app/sdkconfig.defaults',
                 'firmware/factory.bin', 'firmware/resources.zip']
        result = bp.route(self.boards, paths)
        self.assertEqual([], result['boards'])
        self.assertFalse(result['docs_only'])
        self.assertEqual(['documentation', 'source_or_config', 'source_or_config', 'binary', 'archive'],
                         [p['kind'] for p in result['firmware']])

    def test_workflow_docs_command_and_output(self):
        base = self.initialize_git()
        self.write('alpha/README.md', '# Documentation\n')
        self.git('add', '.')
        self.git('commit', '-qm', 'Documentation fixture')
        result, values = self.cli('matrix', '--base', base)
        self.assertEqual(0, result.returncode, result.stderr)
        self.assertEqual('false', values['has_builds'])
        self.assertEqual('true', values['docs_only'])
        self.assertEqual({'include': []}, json.loads(values['matrix']))

    def test_empty_and_missing_diff_fail_cli(self):
        base = self.initialize_git()
        for ref in (base, 'missing-ref'):
            result, values = self.cli('matrix', '--base', ref)
            self.assertEqual(2, result.returncode)
            self.assertEqual({}, values)
        with self.assertRaises(ValueError):
            bp.route(self.boards, [])

    def test_real_rename_and_deletion_include_old_path(self):
        self.write('alpha/source.c', 'int value;\n')
        base = self.initialize_git()
        self.git('mv', 'alpha/source.c', 'display/beta/source.c')
        self.git('commit', '-qm', 'Move fixture')
        paths = bp.changed_paths(self.root, base)
        self.assertEqual({'alpha/source.c', 'display/beta/source.c'}, set(paths))
        self.assertEqual(self.boards, bp.route(self.boards, paths)['boards'])
        old = self.git('rev-parse', 'HEAD')
        self.git('rm', '-r', 'alpha')
        self.git('commit', '-qm', 'Remove fixture')
        remaining = bp.discover(self.root)
        self.assertEqual(remaining, bp.route(remaining, bp.changed_paths(self.root, old))['boards'])

    def test_registry_exact_stable_endpoints(self):
        from idf_component_tools.utils import ComponentVersion
        releases = [SimpleNamespace(version=ComponentVersion(v)) for v in
                    ('0.7.2', '0.9.0', '0.10.0', '1.0.0-rc1')]
        with patch('idf_component_tools.registry.service_details.get_storage_client') as client:
            client.return_value.versions.return_value.versions = releases
            self.assertEqual(['0.7.2', '0.10.0'], bp.registry_versions(self.root))
            client.return_value.versions.return_value.versions = releases[:1]
            self.assertEqual(['0.7.2'], bp.registry_versions(self.root))
            client.return_value.versions.side_effect = RuntimeError('registry unavailable')
            with self.assertRaises(RuntimeError):
                bp.registry_versions(self.root)

    def test_full_matrix_cli_records_exact_tooling(self):
        result, values = self.cli('matrix', '--all', '--bmgr', '0.7.2')
        self.assertEqual(0, result.returncode, result.stderr)
        cells = json.loads(values['matrix'])['include']
        self.assertEqual(4, len(cells))
        self.assertEqual({'v5.5.5', 'v6.1'}, {c['idf'] for c in cells})
        self.assertEqual({'idf-component-manager==2.5.0', 'idf-component-manager==3.0.3'},
                         {c['component_manager'] for c in cells})
        self.assertEqual('true', values['has_builds'])

    def test_matrix_only_adds_integration_for_declared_profiles(self):
        self.write('integrations/brookesia_hal_custom/profiles/alpha/sdkconfig.defaults', '# profile\n')
        result, values = self.cli('matrix', '--all', '--bmgr', '0.7.2')
        self.assertEqual(0, result.returncode, result.stderr)
        cells = json.loads(values['matrix'])['include']
        self.assertEqual(5, len(cells))
        opt_in = [cell for cell in cells if cell['integration'] == 'brookesia']
        self.assertEqual(1, len(opt_in))
        self.assertEqual({'v6.1'}, {cell['idf'] for cell in opt_in})
        self.assertEqual({'alpha'}, {cell['board'] for cell in opt_in})
        self.assertTrue(bp.route(self.boards, ['integrations/brookesia_hal_custom/README.md'])['docs_only'])
        self.assertEqual(self.boards, bp.route(self.boards, ['integrations/brookesia_hal_custom/src/plugin.cpp'])['boards'])

    def test_integration_cli_preserves_pack_and_requires_component(self):
        self.write('ci/test_app/main/idf_component.yml',
                   'dependencies:\n  waveshare_boards:\n    override_path: ../../../\n')
        result, _ = self.cli('integration', 'brookesia')
        self.assertEqual(2, result.returncode)
        self.write('integrations/brookesia_hal_custom/CMakeLists.txt', 'idf_component_register()\n')
        result, _ = self.cli('integration', 'brookesia')
        self.assertEqual(0, result.returncode, result.stderr)
        path = self.root / 'ci/test_app/main/idf_component.yml'
        deps = bp.load_yaml(path)['dependencies']
        self.assertEqual('../../../', deps['waveshare_boards']['override_path'])
        self.assertEqual('../../../integrations/brookesia_hal_custom', deps['brookesia_hal_custom']['override_path'])
        result, _ = self.cli('integration', 'none')
        self.assertEqual(0, result.returncode, result.stderr)
        self.assertEqual({'waveshare_boards'}, set(bp.load_yaml(path)['dependencies']))

    def test_pin_is_exact_and_preserves_pack(self):
        self.write('ci/test_app/main/idf_component.yml',
                   'dependencies:\n  espressif/esp_board_manager:\n    version: "*"\n  waveshare_boards:\n    override_path: ../../../\n')
        result, _ = self.cli('pin', '0.7.2')
        self.assertEqual(0, result.returncode)
        deps = bp.load_yaml(self.root / 'ci/test_app/main/idf_component.yml')['dependencies']
        self.assertEqual('==0.7.2', deps['espressif/esp_board_manager']['version'])
        self.assertEqual('../../../', deps['waveshare_boards']['override_path'])
        for value in ('*', '0.7.2;false', '1.0.0-rc1'):
            with self.assertRaises(ValueError):
                bp.pin(self.root, value)


if __name__ == '__main__':
    unittest.main()
