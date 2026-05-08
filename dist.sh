#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "$0")"

cmake --build build --config Release

mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"

cp -R \
  "build/AkiraMulti_artefacts/Release/VST3/Akira Multi.vst3" \
  "$HOME/Library/Audio/Plug-Ins/VST3/"

echo "Akira Multi VST3 plugin copied to ~/Library/Audio/Plug-Ins/VST3/"
