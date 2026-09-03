#!/usr/bin/env bash

source .github/scripts/vars.sh
source .github/scripts/fold.sh

export COVERAGE=${COVERAGE:-true}
export REPOSITORY_ROOT="${REPOSITORY_ROOT:-$PWD}"
export INSTALL_PREFIX=${HOME}/build
export PKG_CONFIG_PATH="${INSTALL_PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}"
export LD_LIBRARY_PATH="${INSTALL_PREFIX}/lib:${LD_LIBRARY_PATH}"

export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE="$(command -v php)"

function install_libmustache() (
    set -o errexit -o pipefail -o xtrace

    rm -rf libmustache
    git init libmustache
    git -C libmustache remote add origin https://github.com/jbboehr/libmustache.git
    git -C libmustache fetch --depth=1 origin "${LIBMUSTACHE_VERSION}"
    git -C libmustache checkout --detach FETCH_HEAD
    cd libmustache
    autoreconf -fiv
    ./configure \
        --prefix="${INSTALL_PREFIX}" \
        --without-json \
        --without-mustache-spec \
        --without-yaml
    make all install
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

function initialize_coverage() (
    set -o errexit -o pipefail -o xtrace

    lcov --directory . --zerocounters
    lcov --no-checksum --directory . --capture --compat-libtool --initial --output-file coverage.base
)

function test_php_mustache() (
    set -o errexit -o pipefail -o xtrace

    ${TEST_PHP_EXECUTABLE} run-tests.php -n -d extension=modules/mustache.so -j$(nproc --all) ./tests/*.phpt
)

function process_coverage() (
    set -o errexit -o pipefail -o xtrace

    lcov --no-checksum --directory . --capture --compat-libtool --output-file coverage.run
    lcov --add-tracefile coverage.base --add-tracefile coverage.run --output-file coverage.info
    lcov --extract coverage.info "${REPOSITORY_ROOT}/*" \
        --compat-libtool \
        --output-file coverage.info
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
    set -e -o pipefail
    trap after_failure ERR

    cifold "install libmustache" install_libmustache
    cifold "main build step" build_php_mustache
    if [[ "${COVERAGE}" = "true" ]]; then
        cifold "initialize coverage" initialize_coverage
    fi
    cifold "main test suite" test_php_mustache
    if [[ "${COVERAGE}" = "true" ]]; then
        cifold "upload coverage" process_coverage
    fi
)
