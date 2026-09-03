#!/usr/bin/env bash

set -e -o pipefail

source .github/scripts/vars.sh
source .github/scripts/fold.sh

# config
export PHP_VERSION=${PHP_VERSION:-"8.3"}
export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
if [[ "${PHP_VERSION}" = "8.5" ]]; then
    PHP_FORMULA=php
else
    PHP_FORMULA="php@${PHP_VERSION}"
fi

function install_brew_packages() (
    set -o errexit -o pipefail -o xtrace

    brew install autoconf automake libtool pkg-config "${PHP_FORMULA}"
)

function install_libmustache() (
    set -o errexit -o pipefail -o xtrace

    git init libmustache
    git -C libmustache remote add origin https://github.com/jbboehr/libmustache.git
    git -C libmustache fetch --depth=1 origin "${LIBMUSTACHE_VERSION}"
    git -C libmustache checkout --detach FETCH_HEAD
    cd libmustache
    autoreconf -i
    ./configure \
        --prefix="$HOME/buildprefix" \
        --without-json \
        --without-mustache-spec \
        --without-yaml
    make
    make install
)

function install() (
    set -o errexit -o pipefail -o xtrace

    phpize
    ./configure --prefix=$HOME/buildprefix --with-libmustache=$HOME/buildprefix
    make
)

function script() (
    set -o errexit -o pipefail -o xtrace

    make test
)

cifold "install brew packages" install_brew_packages
PHP_PREFIX="$(brew --prefix "${PHP_FORMULA}")"
export PATH="$PHP_PREFIX/bin:$PHP_PREFIX/sbin:$PATH"
export TEST_PHP_EXECUTABLE="${TEST_PHP_EXECUTABLE:-$(command -v php)}"
cifold "install libmustache" install_libmustache
cifold "install" install
cifold "script" script
