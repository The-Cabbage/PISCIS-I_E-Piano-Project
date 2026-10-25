@echo off
chcp 65001 >nul
"D:\MIDI工具箱\F101开发环境\WSL程序\wsl.exe" -d MIDI-F101-Ubuntu -u midi --exec bash "/mnt/d/MIDI工具箱/F101开发环境/MDA_F101实时演奏版/build.sh"
set "result=%errorlevel%"
pause
exit /b %result%
