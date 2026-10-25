#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "此脚本使用 macOS 命令行开发工具。"
  exit 1
fi
mkdir -p build
CXX="$(xcrun --find clang++)"
SDK="$(xcrun --sdk macosx --show-sdk-path)"
"$CXX" -std=c++17 -O2 -mmacosx-version-min=11.0 -isysroot "$SDK" -Isource \
  source/test_memory.cpp source/MdaEPianoEngine.cpp source/mdaEPiano.cpp \
  -o build/test_memory
exec ./build/test_memory "$@"
