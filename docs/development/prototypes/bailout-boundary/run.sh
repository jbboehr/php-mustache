#!/usr/bin/env bash
set -euo pipefail

prototype_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
prototype_binary=$(mktemp "${TMPDIR:-/tmp}/mustache-bailout-prototype.XXXXXX")
trap 'rm -f -- "${prototype_binary}"' EXIT

prototype_cflag_text=$(pkg-config --cflags mustache)
prototype_lib_text=$(pkg-config --libs mustache)
read -r -a prototype_cflags <<< "${prototype_cflag_text}"
read -r -a prototype_libs <<< "${prototype_lib_text}"
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -pedantic \
  "${prototype_cflags[@]}" "${prototype_dir}/test.cpp" \
  "${prototype_libs[@]}" -o "${prototype_binary}"
"${prototype_binary}"
