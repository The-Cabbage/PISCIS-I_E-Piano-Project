#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "此脚本需要在 Mac 上运行。"; exit 1
fi
if ! xcrun --find clang++ >/dev/null 2>&1; then
  echo "请先在终端执行：xcode-select --install"
  echo "安装苹果命令行开发工具后，重新运行本脚本。"
  exit 1
fi
mkdir -p build
CXX="$(xcrun --find clang++)"
SDK="$(xcrun --sdk macosx --show-sdk-path)"
FLAGS=(-std=c++17 -O2 -mmacosx-version-min=11.0 -isysroot "$SDK" -Isource -pthread)
echo "正在编译与测试音源……"
"$CXX" "${FLAGS[@]}" source/test_core.cpp source/MdaEPianoEngine.cpp source/mdaEPiano.cpp -o build/test_core
./build/test_core
echo "正在构建苹果界面与音频输出……"
"$CXX" "${FLAGS[@]}" -fobjc-arc -c source/main_macos.mm -o build/main_macos.o
"$CXX" "${FLAGS[@]}" -c source/MdaEPianoEngine.cpp -o build/MdaEPianoEngine.o
"$CXX" "${FLAGS[@]}" -c source/mdaEPiano.cpp -o build/mdaEPiano.o
APP="MDA电钢琴.app"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
"$CXX" -mmacosx-version-min=11.0 -isysroot "$SDK" build/main_macos.o build/MdaEPianoEngine.o build/mdaEPiano.o -framework Cocoa -framework AudioToolbox -o "$APP/Contents/MacOS/MDA_EPiano"
cp Info.plist "$APP/Contents/Info.plist"
if command -v codesign >/dev/null 2>&1; then
  codesign --force --sign - "$APP"
  codesign --verify "$APP"
fi
echo "构建完成：$PWD/$APP"
echo "本次生成 $(uname -m) 架构；请在本机试弹确认声音与操作。"
open "$APP"
