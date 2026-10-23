# PISCIS-I HPM6360 便携式音源模块

本目录是 PISCIS-I 的 HPM6360 硬件工作区。原始 A0 基线保留作参考；新的 **Rev.A 电子乐器评估板** 位于 `hardware/rev_a/`，已经建立分层原理图源、HPM6360 引脚分配、8+8 控制扩展板和 130 mm × 90 mm 六层初始布局。Rev.A 当前是工程验证版本，尚未完成 PCB 布线或投产审核。

## 快速入口

- KiCad 工程：`PISCIS-I_HPM6360_Module.kicad_pro`
- [Rev.A 重构工程](hardware/rev_a/README.md)
- [Rev.A 接口定义](hardware/rev_a/INTERFACES.md)
- [Rev.A HPM6360 Pinmux 表](hardware/rev_a/pinmux.csv)
- [Rev.A 验证状态与投板门槛](hardware/rev_a/VALIDATION.md)
- [系统需求](docs/REQUIREMENTS.md)
- [元器件选型](docs/COMPONENT_SELECTION.md)
- [Rev A0 BOM 工作簿](outputs/01a0a15e-6745-7f52-aa7f-aea0cbdf02fa/PISCIS-I_HPM6360_BOM_RevA0.xlsx)
- [项目级 KiCad 库与 3D 模型清单](hardware/libraries/MODEL_MANIFEST.md)
- [24-Pin POGO 扩展与级联接口](docs/POGO_INTERFACE.md)
- [下一步工作](docs/NEXT_STEPS.md)

## Rev.A 已冻结基线

- 首版是 130 mm × 90 mm、6 层、便于探测和返修的评估板，不再受 65 mm × 65 mm 限制。
- MCU 默认改为 `HPM6360IPA2`，封装为 20 mm × 20 mm、0.5 mm pitch 的 `eLQFP144L`。外围引脚可拖焊返修；底部裸露焊盘仍建议用钢网 + 热台/回流或热风焊接。
- USB-C 固定为 5 V 电源输入；USB-A 固定为 Host，用于 USB MIDI 外设。
- 采用 `W9825G6KH-6I` 32 MiB SDRAM、`GD25Q128ESIGR` QSPI NOR 和 4-bit SDIO microSD。
- `ES8388` 提供立体声耳机输出；单声道 Class-D 通过 JST-PH 接口驱动 4 Ω / 3 W 被动扬声器。
- 独立控制扩展板提供 8 路 10 kΩ 模拟控件和 8 个 A/B 增量编码器，通过 2×8 IDC 短排线连接。
- A0 的电池、双扬声器和 24-pin POGO 架构不进入本次 Rev.A。

## 使用

KiCad 10.0.6 或更高版本直接打开 `PISCIS-I_HPM6360_Module.kicad_pro`。原有自定义库 `PISCIS_Local` 保留；新增的可追溯库快照为 `PISCIS_HPM6360`，位于 `hardware/libraries/footprints/PISCIS_HPM6360.pretty/`，已收集 STEP 位于相邻的 `3dmodels/` 目录。项目私有库为 `PISCIS_Private`，位于 `hardware/libraries/private/`，包含 HPM6360IPA2 145-pin 符号、两种官方图纸派生的 eLQFP144L 中央焊盘候选封装及现有项目资源副本；使用前请阅读该目录的 `README.md`。

在器件订货或打样前，必须完成 [下一步工作](docs/NEXT_STEPS.md) 中的机械布局冻结、电源预算、HPM6360 引脚复用审核和样品可得性确认。
