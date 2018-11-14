#!/usr/bin/env bash

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
    set -e -o pipefail

    echo "Installing libmustache ${LIBMUSTACHE_VERSION}"

    local dir=${INSTALL_PREFIX}/src/libmustache
    rm -rf ${dir}
    mkdir -p $(dirname ${dir})
    git clone -b ${LIBMUSTACHE_VERSION} git://github.com/jbboehr/libmustache.git ${dir}
    cd ${dir}
    autoreconf -fiv
    ./configure --prefix=${INSTALL_PREFIX} --without-mustache-spec
    make all install
)

function before_install() (
    set -e -o pipefail

    # Don't install this unless we're actually on travis
    if [[ "${COVERAGE}" = "true" ]] && [[ "${TRAVIS}" = "true" ]]; then
        gem install coveralls-lcov
    fi
)

function install() (
    set -e -o pipefail

    install_libmustache

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

function before_script() (
    set -e -o pipefail

    if [[ "${COVERAGE}" = "true" ]]; then
        echo "Initializing coverage"
        lcov --directory . --zerocounters
        lcov --directory . --capture --compat-libtool --initial --output-file coverage.info
    fi
)

function script() (
    set -e -o pipefail

    echo "Running main test suite"
    ${TEST_PHP_EXECUTABLE} run-tests.php -n -d extension=modules/mustache.so ./tests/*.phpt
)

function after_success() (
    set -e -o pipefail

    if [[ "${COVERAGE}" = "true" ]]; then
        echo "Processing coverage"
        lcov --no-checksum --directory . --capture --compat-libtool --output-file coverage.info
        lcov --remove coverage.info "/usr*" \
            --remove coverage.info "*/.phpenv/*" \
            --remove coverage.info "/home/travis/build/include/*" \
            --compat-libtool \
            --output-file coverage.info

        echo "Uploading coverage"
        coveralls-lcov coverage.info
    fi
)

function after_failure() (
    set -e -o pipefail

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
