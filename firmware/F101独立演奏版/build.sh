#!/usr/bin/env bash
set -euo pipefail
source_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${F101_WORKSPACE:-$HOME/f101-zephyr}"
. ./env.sh
app_dir=apps/piscis_standalone
build_dir=build/piscis_standalone
mkdir -p "$app_dir" "$source_dir/output"
cp -a "$source_dir"/{core,organ,synth,src,boards,prj.conf,CMakeLists.txt} "$app_dir/"
export CMAKE_BUILD_PARALLEL_LEVEL=4
west build -b yuzukineko -d "$build_dir" "$app_dir" 2>&1 | tee "$source_dir/output/build.log"
cp "$build_dir"/zephyr/{zephyr.bin,zephyr.elf,zephyr.map,.config} "$source_dir/output/"
