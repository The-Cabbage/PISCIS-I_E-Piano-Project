#!/usr/bin/env bash
set -euo pipefail
cd /home/midi/f101-zephyr
. ./env.sh
source_dir='/mnt/d/MIDI工具箱/F101开发环境/MDA_F101实时演奏版'
mkdir -p apps/mda_live logs
# Sync this isolated app only; do not alter the SDK or desktop project.
cp -a "$source_dir/core" "$source_dir/src" "$source_dir/tools" "$source_dir/CMakeLists.txt" "$source_dir/prj.conf" apps/mda_live/
log="logs/mda-$(date +%Y%m%d-%H%M%S).log"
export CMAKE_BUILD_PARALLEL_LEVEL=4
west build -b yuzukineko -d build/mda_live apps/mda_live 2>&1 | tee "$log"
out="$source_dir/output"
mkdir -p "$out"
for name in zephyr.bin zephyr.elf zephyr.map .config; do cp "build/mda_live/zephyr/$name" "$out/$name"; done
cp "$log" "$out/build.log"
(cd "$out"; sha256sum zephyr.bin zephyr.elf > SHA256SUMS.txt)
printf 'MDA build completed: %s\n' "$out"
