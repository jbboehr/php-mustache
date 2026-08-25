# Fuzzing

The PHP 8.3 coverage-guided fuzzer runs the extension and libmustache under AddressSanitizer and UndefinedBehaviorSanitizer. It exercises tokenizing, parsing, rendering from source and parsed templates, PHP data and partial conversion, exception cleanup, and renderer reuse. AST serialization is intentionally left to libmustache's own fuzz targets.

From the repository root, start a local campaign with a writable copy of the seed corpus:

```sh
FUZZ_CORPUS=$(mktemp -d /tmp/php-mustache-fuzz.XXXXXX)
cp -R fuzz/corpus/. "$FUZZ_CORPUS/"
nix run .#php83-clang-fuzzer -- -max_total_time=300 "$FUZZ_CORPUS"
```
