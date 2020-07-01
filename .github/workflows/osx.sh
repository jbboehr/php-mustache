#!/usr/bin/env bash

set -e -o pipefail

source .ci/vars.sh
source .ci/fold.sh

# config
export PHP_VERSION=${PHP_VERSION:-"7.4"}
export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE=${TEST_PHP_EXECUTABLE:-`which php`}

function install_brew_packages() (
    set -o errexit -o pipefail -o xtrace

    brew install autoconf automake libtool pkg-config json-c libyaml php@$PHP_VERSION
)

function install_libmustache() (
    set -o errexit -o pipefail -o xtrace

    git clone https://github.com/jbboehr/libmustache.git
    cd libmustache
    git checkout ${LIBMUSTACHE_VERSION}
    autoreconf -i
    ./configure --without-mustache-spec
    make
    make install
)

function install() (
    set -o errexit -o pipefail -o xtrace

    phpize
    ./configure
    make
)

function script() (
    set -o errexit -o pipefail -o xtrace

    make test
)

cifold "install brew packages" install_brew_packages
cifold "install libmustache" install_libmustache
cifold "install" install
cifold "script" script
