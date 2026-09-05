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
export TEST_PHP_EXECUTABLE="${TEST_PHP_EXECUTABLE:-php}"
export PHPIZE="${PHPIZE:-phpize}"
export PHP_CONFIG="${PHP_CONFIG:-php-config}"

function verify_php_tools() {
    local variable executable
    for variable in TEST_PHP_EXECUTABLE PHPIZE PHP_CONFIG; do
        if ! executable=$(command -v -- "${!variable}") || [[ ! -x "${executable}" ]]; then
            echo "${variable} is not an executable: ${!variable}" >&2
            return 1
        fi
        if [[ "${executable}" != /* ]]; then
            executable="${PWD}/${executable}"
        fi
        printf -v "${variable}" '%s' "${executable}"
    done

    local runtime_version config_version expected_version include_dir php_api phpize_version phpize_api
    runtime_version=$("${TEST_PHP_EXECUTABLE}" -n -r 'echo PHP_MAJOR_VERSION, ".", PHP_MINOR_VERSION;') || return 1
    config_version=$("${PHP_CONFIG}" --version) || return 1
    expected_version="${PHP_VERSION:-${runtime_version}}"
    if [[ -z "${runtime_version}" || "${runtime_version}" != "${expected_version}" ||
          "${config_version}" != "${runtime_version}."* ]]; then
        echo "PHP version mismatch: expected ${expected_version}, runtime ${runtime_version}, php-config ${config_version}" >&2
        return 1
    fi

    include_dir=$("${PHP_CONFIG}" --include-dir) || return 1
    php_api=$(awk '$1 == "#define" && $2 == "PHP_API_VERSION" { print $3 }' "${include_dir}/main/php.h") || return 1
    phpize_version=$("${PHPIZE}" --version) || return 1
    phpize_api=$(awk '/^PHP Api Version:/ { print $4 }' <<< "${phpize_version}")
    if [[ -z "${php_api}" || "${phpize_api}" != "${php_api}" ]]; then
        echo "PHP API mismatch: php-config headers ${php_api}, phpize ${phpize_api}" >&2
        return 1
    fi

    echo "PHP ${runtime_version}: ${TEST_PHP_EXECUTABLE}"
    echo "php-config ${config_version}: ${PHP_CONFIG}"
    echo "phpize API ${phpize_api}: ${PHPIZE}"
}

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

    "${PHPIZE}"
    if [[ "${COVERAGE}" = "true" ]]; then
        ./configure --enable-mustache \
            --with-php-config="${PHP_CONFIG}" \
            --with-libmustache="${INSTALL_PREFIX}" \
            CXXFLAGS="--coverage -fprofile-arcs -ftest-coverage ${CFLAGS}" \
            LDFLAGS="--coverage -lgcov ${LDFLAGS}"
    else
        ./configure --enable-mustache \
            --with-php-config="${PHP_CONFIG}" \
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

    "${TEST_PHP_EXECUTABLE}" run-tests.php -n -d extension=modules/mustache.so "-j$(nproc --all)" ./tests/*.phpt
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

    verify_php_tools
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
