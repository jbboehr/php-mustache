"""Check that the generated arginfo header records the current PHP stub."""

import hashlib
from pathlib import Path
import re
import unittest


REPOSITORY = Path(__file__).resolve().parents[2]


class ArginfoTest(unittest.TestCase):
    def test_header_records_current_stub_hash(self):
        stub = (REPOSITORY / "mustache.stub.php").read_bytes()
        header = (REPOSITORY / "mustache_arginfo.h").read_bytes()
        regenerate = "Run php build/gen_stub.php -f mustache.stub.php to regenerate it."
        match = re.search(rb"\* Stub hash: ([0-9a-f]{40}) \*/", header)
        self.assertIsNotNone(match, "mustache_arginfo.h has no valid stub hash. " + regenerate)

        # PHP's build/gen_stub.php normalizes CRLF before computing this hash.
        expected = hashlib.sha1(stub.replace(b"\r\n", b"\n"), usedforsecurity=False).hexdigest()
        self.assertEqual(match.group(1).decode(), expected,
                         "mustache_arginfo.h is stale. " + regenerate)


if __name__ == "__main__":
    unittest.main()
