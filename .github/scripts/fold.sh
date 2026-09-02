#!/usr/bin/env bash

set -e -o pipefail

function cifold () (
    set -e -o pipefail
    local name=$1
    shift

    if [[ -n "${GITHUB_RUN_ID:-}" ]]; then
        echo "::group::$name"
        trap 'echo "::endgroup::"' EXIT
    fi

    "$@"
)
