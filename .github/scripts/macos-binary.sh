#!/usr/bin/env bash

set -Eeu -o pipefail

export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_ARGS="-W $RUNNER_TEMP/phpt-results.txt"
export TEST_PHP_JUNIT="$RUNNER_TEMP/phpt-junit.xml"

php -n <<'PHP'
<?php
$version = PHP_MAJOR_VERSION . '.' . PHP_MINOR_VERSION;
$ts = PHP_ZTS ? 'zts' : 'nts';
if ($version !== getenv('PHP_VERSION') || $ts !== getenv('PHP_THREAD_SAFETY') || PHP_DEBUG) {
    fwrite(STDERR, "Unexpected PHP build: $version $ts\n");
    exit(1);
}
PHP

phpize
./configure --enable-mustache --with-libmustache="$RUNNER_TEMP/libmustache-sdk"
make -j2
# Homebrew ZTS php-config can report NONE for the CLI; use the validated PHP.
make test PHP_EXECUTABLE="$(command -v php)"

test "$(lipo -archs modules/mustache.so)" = arm64
otool -L modules/mustache.so > "$RUNNER_TEMP/mustache-libraries.txt"
cat "$RUNNER_TEMP/mustache-libraries.txt"
awk 'NR > 1 && $1 !~ /^\/usr\/lib\// && $1 !~ /^\/System\/Library\// { exit 1 }' \
    "$RUNNER_TEMP/mustache-libraries.txt"

package="php_mustache-${PACKAGE_REF}_php${PHP_VERSION}-arm64-darwin-bsdlibc-${PHP_THREAD_SAFETY}.zip"
mkdir -p artifacts
zip -j "artifacts/$package" modules/mustache.so LICENSE
unzip -q "artifacts/$package" -d "$RUNNER_TEMP/mustache-package"
php -n -d "extension=$RUNNER_TEMP/mustache-package/mustache.so" --ri mustache
php -n -d "extension=$RUNNER_TEMP/mustache-package/mustache.so" -r '
if ((new Mustache())->render("Hello {{name}}", ["name" => "PIE"]) !== "Hello PIE") {
    fwrite(STDERR, "Packaged extension failed to render\n");
    exit(1);
}
'
echo "package=$package" >> "$GITHUB_OUTPUT"
