# MDA F101 板端源码包

打包日期：2026-10-25。
来源：PISCIS-I_E-Piano_Project/源代码/吴/MDA_F101实时演奏版 当前磁盘文件。

本包按原 CMakeLists.txt 的板端编译定义收集源码，并包含直接依赖的头文件、采样数据、配置和原有构建/RAM 加载脚本。原文件保持字节不变。

## 内容

- src/：Zephyr 入口、自定义 USB 请求/响应协议、CherryUSB CDC 传输。
- core/：MDA EPiano 合成核心、采样与配置。
- CMakeLists.txt、prj.conf：板端构建配置。
- build.sh、build.cmd：原有 Windows/WSL 构建脚本。
- load-ram.cmd、tools/load-ram.ps1：原有 FEL/PSRAM 加载脚本。
- core-import.json：上游核心文件来源与哈希记录。
- SHA256SUMS.txt：本包原文件和本说明的 SHA256 校验值。

未包含 pc/、电脑端构建和启动程序、输出二进制、历史测试结果或整机硬件资料。

## 构建与运行边界

当前板端程序使用 Zephyr、CherryUSB、yuzukineko 板定义及支持 F101 的外部 SDK/工具链。这些外部依赖不在本包中，其精确版本待补充；本包不是完整 SDK。

在原项目所需 SDK 环境已经就绪并加载其环境变量后，可将本目录作为 Zephyr 应用目录：

```sh
west build -b yuzukineko -d build/mda_f101_live <本源码包解压后的目录>
```

保留的 build.sh/build.cmd 含原开发环境的 D 盘、WSL 和 /home/midi/f101-zephyr 固定路径，使用前需核对本机路径。load-ram.ps1 还依赖外部 xfel.exe、Windows PowerShell 及 FEL 模式设备；相关工具不随包提供。上述脚本本次均未执行。

当前程序由电脑经 USB 请求驱动，每请求生成 441 帧、44.1 kHz、16 位立体声 PCM，再返回电脑播放；运行完整演奏链路仍需原工程的电脑端程序。本包没有实现脱离电脑的板载音频输出，也没有接入霍尔键盘。

本次仅完成打包和文件完整性校验，未重新编译或进行硬件验证。保留各源文件已有的许可声明；core/ 含 GPL-3.0-or-later 声明，src/usb_transport.c 含 Apache-2.0 标识。
