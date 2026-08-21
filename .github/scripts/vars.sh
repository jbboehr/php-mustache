#!/usr/bin/env bash

export PS4=' \e[33m$(date +"%H:%M:%S"): $BASH_SOURCE@$LINENO ${FUNCNAME[0]} -> \e[0m'

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
    echo "${BASH_SOURCE[0]} must be sourced, not executed" >&2
    exit 1
fi

if [[ -z "${LIBMUSTACHE_VERSION:-}" ]]; then
    if ! command -v jq >/dev/null 2>&1; then
        echo "jq is required to read the locked libmustache revision" >&2
        return 1
    fi

    LIBMUSTACHE_SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
    LIBMUSTACHE_FLAKE_LOCK="${LIBMUSTACHE_SCRIPT_DIR}/../../flake.lock"
    if ! LIBMUSTACHE_VERSION="$(
        jq --exit-status --raw-output \
            '.nodes[.nodes[.root].inputs.libmustache].locked.rev | select(type == "string" and length > 0)' \
            "${LIBMUSTACHE_FLAKE_LOCK}"
    )"; then
        echo "Could not read the locked libmustache revision from ${LIBMUSTACHE_FLAKE_LOCK}" >&2
        return 1
    fi
fi
export LIBMUSTACHE_VERSION
