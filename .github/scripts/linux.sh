#!/usr/bin/env bash

set -e -o pipefail

source .github/scripts/vars.sh
source .github/scripts/fold.sh

# config
export PHP_VERSION=${PHP_VERSION:-"8.3"}
export COVERAGE=${COVERAGE:-false}
export DEBIAN_FRONTEND=noninteractive
export SUDO=sudo

function install_apt_packages() (
    ${SUDO} add-apt-repository ppa:ondrej/php
    ${SUDO} apt-get update
    ${SUDO} apt-get install -y libjson-c-dev libyaml-dev nlohmann-json3-dev "php${PHP_VERSION}-dev" lcov
)

cifold "install apt packages" install_apt_packages

# source and execute the shared build and test suite
source .github/scripts/suite.sh
run_all
