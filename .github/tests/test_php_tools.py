"""Exercise the Linux CI entry point without installing packages or building code."""

import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[2]
FAKE_TOOL = r'''
import json
import os
from pathlib import Path
import shutil
import sys

name = Path(sys.argv[0]).name
args = sys.argv[1:]
with open(os.environ["CI_TOOL_LOG"], "a") as log:
    log.write(json.dumps({"name": name, "args": args, "cwd": os.getcwd(),
                          "runtime": os.environ.get("TEST_PHP_EXECUTABLE"),
                          "phpize": os.environ.get("PHPIZE"),
                          "config": os.environ.get("PHP_CONFIG")}) + "\n")
tools = json.loads(Path(os.environ["CI_TOOL_SETTINGS"]).read_text())
tool = tools.get(name, {})
if tool.get("fail"):
    sys.exit(1)
if tool.get("role") == "php":
    if "-r" in args:
        print(tool["version"].rsplit(".", 1)[0])
elif tool.get("role") == "phpize":
    if args == ["--version"]:
        print("Configuring for:\nPHP Api Version:         " + tool["api"])
elif tool.get("role") == "php-config":
    if args == ["--version"]:
        print(tool["version"])
    elif args == ["--include-dir"]:
        print(os.environ["CI_PHP_HEADERS"])
    else:
        sys.exit(1)
elif name == "git" and args[:1] == ["init"]:
    target = Path(args[1])
    target.mkdir()
    shutil.copy2(Path(os.environ["CI_FIXTURE_ROOT"]) / "configure", target / "configure")
'''


class PhpToolsTest(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="php-mustache-ci-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        shutil.copytree(REPOSITORY / ".github/scripts", self.root / ".github/scripts")
        self.bin = self.root / "bin"
        self.bin.mkdir()
        self.log = self.root / "tools.jsonl"
        self.settings = self.root / "tools.json"
        self.headers = self.root / "include/main"
        self.headers.mkdir(parents=True)
        (self.headers / "php.h").write_text("#define PHP_API_VERSION 20240924\n")
        self.tools = {}
        for suffix, version, api in [("", "8.3.33", "20230831"),
                                     ("8.4", "8.4.24", "20240924")]:
            for role in ["php", "phpize", "php-config"]:
                self.add_tool(role + suffix, role=role, version=version, api=api)
        for name in ["sudo", "git", "autoreconf", "make", "lcov"]:
            self.add_tool(name)
        self.write_tool(self.root / "configure")
        (self.root / "tests").mkdir()
        (self.root / "tests/example.phpt").touch()
        (self.root / "run-tests.php").touch()

    def write_tool(self, path):
        path.write_text(f"#!{sys.executable}\n" + FAKE_TOOL)
        path.chmod(0o755)

    def add_tool(self, name, **settings):
        self.tools[name] = settings
        path = self.bin / name
        self.write_tool(path)
        return str(path)

    def run_ci(self, **overrides):
        self.settings.write_text(json.dumps(self.tools))
        env = os.environ.copy()
        for name in ["TEST_PHP_EXECUTABLE", "PHPIZE", "PHP_CONFIG", "GITHUB_RUN_ID"]:
            env.pop(name, None)
        env.update(PATH=str(self.bin) + os.pathsep + env["PATH"],
                   PHP_VERSION="8.4", COVERAGE="false", LIBMUSTACHE_VERSION="fixture",
                   CI_TOOL_LOG=str(self.log), CI_TOOL_SETTINGS=str(self.settings),
                   CI_PHP_HEADERS=str(self.headers.parent), CI_FIXTURE_ROOT=str(self.root))
        env.update(overrides)
        result = subprocess.run(["bash", ".github/scripts/linux.sh"], cwd=self.root,
                                env=env, text=True, capture_output=True, timeout=30)
        events = [json.loads(line) for line in self.log.read_text().splitlines()]
        return result, events

    def assert_build_tools(self, events, runtime, phpize, config):
        self.assertTrue(any(event["name"] == phpize and not event["args"] for event in events))
        configure = next(event for event in events
                         if event["name"] == "configure" and event["cwd"] == str(self.root))
        self.assertIn("--with-php-config=" + config, configure["args"])
        tests = next(event for event in events if "run-tests.php" in event["args"])
        self.assertEqual(tests["name"], Path(runtime).name)
        self.assertEqual(tests["runtime"], runtime)

    def test_matrix_tools_are_used_despite_different_path_defaults(self):
        result, events = self.run_ci()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assert_build_tools(events, str(self.bin / "php8.4"), "phpize8.4",
                                str(self.bin / "php-config8.4"))

    def test_coverage_preserves_explicit_tools_with_spaces(self):
        runtime = self.add_tool("custom php", role="php", version="8.4.24")
        phpize = self.add_tool("custom phpize", role="phpize", api="20240924")
        config = self.add_tool("custom php-config", role="php-config", version="8.4.24")
        result, events = self.run_ci(COVERAGE="true", TEST_PHP_EXECUTABLE=runtime,
                                     PHPIZE=phpize, PHP_CONFIG=config)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assert_build_tools(events, runtime, "custom phpize", config)
        configure = next(event for event in events
                         if event["name"] == "configure" and event["cwd"] == str(self.root))
        self.assertTrue(any(arg.startswith("CXXFLAGS=--coverage") for arg in configure["args"]))

    def test_relative_override_paths_are_resolved_for_child_processes(self):
        runtime = self.add_tool("custom php", role="php", version="8.4.24")
        phpize = self.add_tool("custom phpize", role="phpize", api="20240924")
        config = self.add_tool("custom php-config", role="php-config", version="8.4.24")
        result, events = self.run_ci(
            TEST_PHP_EXECUTABLE=os.path.relpath(runtime, self.root),
            PHPIZE=os.path.relpath(phpize, self.root),
            PHP_CONFIG=os.path.relpath(config, self.root),
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        tests = next(event for event in events if "run-tests.php" in event["args"])
        self.assertEqual(tests["runtime"], runtime)
        self.assertEqual(tests["phpize"], phpize)
        self.assertEqual(tests["config"], config)

    def assert_rejected_before_build(self, result, events):
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(any(event["name"] in ["git", "configure", "make"] for event in events))

    def test_runtime_mismatch_stops_before_build(self):
        result, events = self.run_ci(TEST_PHP_EXECUTABLE=str(self.bin / "php"))
        self.assert_rejected_before_build(result, events)
        self.assertIn("8.4", result.stderr)
        self.assertIn("8.3", result.stderr)

    def test_config_mismatch_stops_before_build(self):
        result, events = self.run_ci(PHP_CONFIG=str(self.bin / "php-config"))
        self.assert_rejected_before_build(result, events)

    def test_phpize_api_mismatch_stops_before_build(self):
        result, events = self.run_ci(PHPIZE=str(self.bin / "phpize"))
        self.assert_rejected_before_build(result, events)

    def test_missing_override_stops_before_build(self):
        result, events = self.run_ci(PHP_CONFIG=str(self.bin / "missing-php-config"))
        self.assert_rejected_before_build(result, events)
        self.assertIn("missing-php-config", result.stderr)

    def test_failed_version_query_stops_before_build(self):
        self.tools["php8.4"]["fail"] = True
        result, events = self.run_ci()
        self.assert_rejected_before_build(result, events)


if __name__ == "__main__":
    unittest.main()
