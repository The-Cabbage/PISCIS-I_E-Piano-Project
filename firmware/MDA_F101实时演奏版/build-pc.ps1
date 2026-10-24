param([string]$Compiler='F:\c\ucrt64\bin\g++.exe')
$ErrorActionPreference='Stop'
$oldPath=$env:PATH
$env:PATH=(Split-Path $Compiler)+';'+$env:PATH
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Path output -Force | Out-Null
    & $Compiler -mwindows -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -finput-charset=UTF-8 -I core -I src -I pc pc/main_pc_mda.cpp -lwinmm -luser32 -lgdi32 -lsetupapi -ladvapi32 -o output/MDA_F101_Live.exe
    if($LASTEXITCODE -ne 0){throw '电脑端编译失败'}
    & .\output\MDA_F101_Live.exe --protocol-test
    if($LASTEXITCODE -ne 0){throw '协议或琴键检查失败'}
} finally {Pop-Location;$env:PATH=$oldPath}
