#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "$0")"

rm -rf build

cmake_args=(
  -S .
  -B build
  -G Ninja
  -DCMAKE_BUILD_TYPE=Release
)

if [[ -n "${JUCE_DIR:-}" ]]; then
  cmake_args+=("-DJUCE_DIR=${JUCE_DIR}")
elif [[ -d "$HOME/Python/JUCE/JUCE" ]]; then
  cmake_args+=("-DJUCE_DIR=$HOME/Python/JUCE/JUCE")
fi

cmake "${cmake_args[@]}"
cmake --build build
