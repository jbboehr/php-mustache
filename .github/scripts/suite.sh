#!/usr/bin/env bash

source .github/scripts/vars.sh
source .github/scripts/fold.sh

export COVERAGE=${COVERAGE:-true}
export LIBMUSTACHE_VERSION=${LIBMUSTACHE_VERSION:-master}
export TRAVIS_BUILD_DIR=${TRAVIS_BUILD_DIR:-`pwd`}
export INSTALL_PREFIX=${HOME}/build
export PKG_CONFIG_PATH="${INSTALL_PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}"
export LD_LIBRARY_PATH="${INSTALL_PREFIX}/lib:${LD_LIBRARY_PATH}"

export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE=`which php`

function install_libmustache() (
    set -o errexit -o pipefail -o xtrace

    rm -rf libmustache
    git clone https://github.com/jbboehr/libmustache.git
    cd libmustache
    git checkout ${LIBMUSTACHE_VERSION}
    autoreconf -fiv
    ./configure --prefix=${INSTALL_PREFIX} --without-mustache-spec
    make all install
)

function before_install() (
    set -o errexit -o pipefail

    return 0
)

function build_php_mustache() (
    set -o errexit -o pipefail -o xtrace

    phpize
    if [[ "${COVERAGE}" = "true" ]]; then
        ./configure --enable-mustache \
            --with-libmustache="${INSTALL_PREFIX}" \
            CXXFLAGS="--coverage -fprofile-arcs -ftest-coverage ${CFLAGS}" \
            LDFLAGS="--coverage -lgcov ${LDFLAGS}"
    else
        ./configure --enable-mustache \
            --with-libmustache="${INSTALL_PREFIX}"
    fi
    make clean all
)

function install() (
    set -o errexit -o pipefail

    cifold "install libmustache" install_libmustache
    cifold "main build step" build_php_mustache
)

function initialize_coverage() (
    set -o errexit -o pipefail -o xtrace

    lcov --directory . --zerocounters
    lcov --directory . --capture --compat-libtool --initial --output-file coverage.info
)

function before_script() (
    set -e -o pipefail

    if [[ "${COVERAGE}" = "true" ]]; then
        cifold "initialize coverage" initialize_coverage
    fi
)

function test_php_mustache() (
    set -o errexit -o pipefail -o xtrace

    ${TEST_PHP_EXECUTABLE} run-tests.php -n -d extension=modules/mustache.so ./tests/*.phpt
)

function script() (
    set -o errexit -o pipefail

    cifold "main test suite" test_php_mustache
)

function process_coverage() (
    set -o errexit -o pipefail -o xtrace

    lcov --no-checksum --directory . --capture --compat-libtool --output-file coverage.info
    lcov --remove coverage.info "/usr*" \
        --remove coverage.info "*/.phpenv/*" \
        --remove coverage.info "/home/travis/build/include/*" \
        --compat-libtool \
        --output-file coverage.info
)

function after_success() (
    set -o errexit -o pipefail

    if [[ "${COVERAGE}" = "true" ]]; then
        cifold "upload coverage" process_coverage
    fi
)

function after_failure() (
    # set -o errexit -o pipefail

    for i in `find tests -name "*.out" 2>/dev/null`; do
        echo "-- START ${i}";
        cat ${i};
        echo "-- END";
    done
    for i in `find tests -name "*.mem" 2>/dev/null`; do
        echo "-- START ${i}";
        cat ${i};
        echo "-- END";
    done
)

function run_all() (
    set -e
    trap after_failure ERR
    before_install
    install
    before_script
    script
    after_success
)
