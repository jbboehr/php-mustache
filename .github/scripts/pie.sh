#!/usr/bin/env bash

set -Eeu -o pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPOSITORY_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

export PHP_VERSION=${PHP_VERSION:-8.4}
export PIE_VERSION=${PIE_VERSION:-1.4.10}
export DEBIAN_FRONTEND=noninteractive

SUDO=${SUDO-sudo}
SKIP_SYSTEM_PACKAGES=${SKIP_SYSTEM_PACKAGES:-false}
RUNNER_TEMP=${RUNNER_TEMP:-${TMPDIR:-/tmp}}
WORK_DIR="$(mktemp -d "${RUNNER_TEMP}/php-mustache-pie.XXXXXX")"
INSTALL_PREFIX="${WORK_DIR}/libmustache-prefix"
SOURCE_DIR="${WORK_DIR}/php-mustache"
PIE_PHAR="${WORK_DIR}/pie.phar"
PIE_CONFIG_HOME="${WORK_DIR}/config"
PIE_COMPOSER_HOME="${WORK_DIR}/composer"

function run_privileged() {
    if [[ -n "${SUDO}" ]]; then
        "${SUDO}" "$@"
    else
        "$@"
    fi
}

function install_system_packages() {
    run_privileged add-apt-repository -y ppa:ondrej/php
    run_privileged apt-get update
    run_privileged apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        curl \
        g++ \
        gcc \
        git \
        jq \
        libjson-c-dev \
        libtool \
        libyaml-dev \
        m4 \
        make \
        nlohmann-json3-dev \
        "php${PHP_VERSION}-cli" \
        "php${PHP_VERSION}-dev" \
        pkg-config
}

if [[ "${SKIP_SYSTEM_PACKAGES}" != "true" ]]; then
    install_system_packages
fi

# shellcheck disable=SC1091
source "${SCRIPT_DIR}/vars.sh"

PHP_EXECUTABLE=${PHP_EXECUTABLE:-$(command -v "php${PHP_VERSION}")}
PHP_CONFIG=${PHP_CONFIG:-$(command -v "php-config${PHP_VERSION}")}

export PKG_CONFIG_PATH="${INSTALL_PREFIX}/lib/pkgconfig${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}"
export LD_LIBRARY_PATH="${INSTALL_PREFIX}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"

git clone https://github.com/jbboehr/libmustache.git "${WORK_DIR}/libmustache"
git -C "${WORK_DIR}/libmustache" checkout --detach "${LIBMUSTACHE_VERSION}"
(
    cd "${WORK_DIR}/libmustache"
    autoreconf -fiv
    ./configure --prefix="${INSTALL_PREFIX}" --without-mustache-spec
    make -j2
    make install
)

git clone --local --no-hardlinks --no-checkout "${REPOSITORY_ROOT}" "${SOURCE_DIR}"
git -C "${SOURCE_DIR}" checkout -B pie-smoke "${GITHUB_SHA:-HEAD}"

curl --fail --location --retry 3 \
    --output "${PIE_PHAR}" \
    "https://github.com/php/pie/releases/download/${PIE_VERSION}/pie.phar"
gh attestation verify --owner php "${PIE_PHAR}"
"${PHP_EXECUTABLE}" "${PIE_PHAR}" --version

function run_pie() {
    run_privileged env \
        XDG_CONFIG_HOME="${PIE_CONFIG_HOME}" \
        COMPOSER_HOME="${PIE_COMPOSER_HOME}" \
        PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" \
        LD_LIBRARY_PATH="${LD_LIBRARY_PATH}" \
        "${PHP_EXECUTABLE}" "${PIE_PHAR}" "$@"
}

run_pie repository:add \
    --no-interaction \
    --no-cache \
    --with-php-config="${PHP_CONFIG}" \
    path "${SOURCE_DIR}"

run_pie install \
    --no-interaction \
    --no-cache \
    --no-build-tools-check \
    --no-system-dependencies-check \
    --skip-enable-extension \
    --make-parallel-jobs=2 \
    --with-php-config="${PHP_CONFIG}" \
    --with-libmustache="${INSTALL_PREFIX}" \
    jbboehr/php-mustache:dev-pie-smoke

EXTENSION_PATH="$("${PHP_CONFIG}" --extension-dir)/mustache.so"
if [[ ! -f "${EXTENSION_PATH}" ]]; then
    echo "PIE did not install ${EXTENSION_PATH}" >&2
    exit 1
fi

# shellcheck disable=SC2016
"${PHP_EXECUTABLE}" -n -d "extension=${EXTENSION_PATH}" -r '
if (!extension_loaded("mustache")) {
    fwrite(STDERR, "PIE-installed mustache extension did not load\n");
    exit(1);
}

$actual = (new Mustache())->render("Hello {{name}}", array("name" => "PIE"));
if ($actual !== "Hello PIE") {
    fwrite(STDERR, "Unexpected render: " . var_export($actual, true) . "\n");
    exit(1);
}

echo $actual, "\n";
'
