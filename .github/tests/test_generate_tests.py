"""Run the PHPT generator against isolated specifications and output failures."""

import argparse
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import unittest

try:
    import resource
except ImportError:
    resource = None


GENERATOR = Path(__file__).resolve().parents[2] / "generate-tests.php"
PHP = os.environ.get("TEST_PHP_EXECUTABLE", "php")


def expected_phpt(name):
    return (f"--TEST--\n{name}\n"
            "--DESCRIPTION--\nRender a UTF-8 value.\n"
            "--SKIPIF--\n<?php if(!extension_loaded('mustache')) die('skip '); ?>\n"
            "--FILE--\n<?php\n"
            "$test = array (\n"
            f"  'name' => '{name}',\n"
            "  'desc' => 'Render a UTF-8 value.',\n"
            "  'template' => '{{name}}',\n"
            "  'data' => \n"
            "  array (\n"
            "    'name' => 'Zoë',\n"
            "  ),\n"
            "  'expected' => 'Zoë',\n"
            ");\n"
            "$mustache = new Mustache();\n"
            "echo \"<render>\", $mustache->render($test[\"template\"], $test[\"data\"]), \"</render>\";\n"
            "?>\n"
            "--EXPECT--\n<render>Zoë</render>").encode()


def limit_output_size():
    signal.signal(signal.SIGXFSZ, signal.SIG_IGN)
    resource.setrlimit(resource.RLIMIT_FSIZE, (128, 128))


class GenerateTestsTest(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="php-mustache-generator-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.specs = self.root / "specs"
        self.specs.mkdir()
        self.output = self.root / "tests"
        self.output.mkdir()
        self.first = self.output / "mustache-spec-sample-first.phpt"
        self.later = self.output / "mustache-spec-sample-later.phpt"
        tests = [dict(name=name, desc="Render a UTF-8 value.", template="{{name}}",
                      data={"name": "Zoë"}, expected="Zoë") for name in ["First", "Later"]]
        (self.specs / "sample.json").write_text(json.dumps({"tests": tests}), encoding="utf-8")

    def run_generator(self, **options):
        return subprocess.run([PHP, "-n", str(GENERATOR), str(self.specs)],
                              cwd=self.root, capture_output=True, timeout=30, **options)

    def assert_write_failed(self, result):
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn(self.first.name.encode(), result.stdout + result.stderr)
        self.assertFalse(self.later.exists(), "generation continued after a failed write")

    def test_success_produces_complete_phpt_files(self):
        result = self.run_generator()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(sorted(path.name for path in self.output.iterdir()),
                         [self.first.name, self.later.name])
        for path, name in [(self.first, "First"), (self.later, "Later")]:
            with self.subTest(name=name):
                contents = path.read_bytes()
                self.assertEqual(contents, expected_phpt(name))
                php = contents.split(b"--FILE--\n", 1)[1].split(b"--EXPECT--\n", 1)[0]
                lint = subprocess.run([PHP, "-n", "-l"], input=php, capture_output=True, timeout=30)
                self.assertEqual(lint.returncode, 0, lint.stdout + lint.stderr)

    def test_unopenable_output_stops_generation(self):
        self.first.mkdir()
        self.assert_write_failed(self.run_generator())

    @unittest.skipUnless(resource is not None and hasattr(resource, "RLIMIT_FSIZE"),
                         "requires a POSIX file-size limit")
    def test_partial_write_stops_generation(self):
        result = self.run_generator(preexec_fn=limit_output_size)
        self.assertEqual(self.first.stat().st_size, 128, "fixture did not produce a partial write")
        self.assert_write_failed(result)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--php", default=PHP)
    options, tests = parser.parse_known_args()
    PHP = options.php
    unittest.main(argv=[sys.argv[0], *tests])
