# PISCIS-I 项目级 KiCad 库

本目录是 Rev A0 的可追溯库快照，不代表所有器件已经完成封装冻结。库的目标是让已核实的符号、封装和 STEP 模型不依赖另一台电脑的全局 KiCad 安装。

## 目录

- `footprints/PISCIS_HPM6360.pretty/`：从 KiCad 10 官方库整理的候选封装，以及项目自定义的 24-pin POGO 封装。
- `symbols/vendor-kicad10/`：只对当前已能直接复用的官方符号库做版本快照。
- `3dmodels/PISCIS_HPM6360.3dshapes/`：已在本机逐文件确认存在的 STEP 模型。
- `MODEL_MANIFEST.md`：器件与符号、封装、3D 状态的对应表。
- `LICENSE-KICAD-LIBRARIES.md`：随快照保留的 KiCad 官方库许可。

## 使用规则

1. 原理图中优先选择 `PISCIS_K10_*` 符号库里已列出的精确型号；找不到精确型号时不得用引脚数相同的通用 IC 代替。
2. PCB 中优先选择 `PISCIS_HPM6360` 封装库。封装名称存在不等于已经通过数据手册复核；以清单中的“状态”为准。
3. 已收集的 STEP 使用 `${KIPRJMOD}` 相对路径。未收集到的模型仍保留 KiCad 全局模型引用，BOM 中标为待补齐。
4. HPM6360 eLQFP144L、OLED FPC、PEC11R、扬声器和若干 TI 小封装必须在原理图冻结前完成 1:1 尺寸、焊盘、Pin-1 和高度复核。项目已收集 20×20 mm LQFP 的机械 STEP，但 HPM6360 中央裸露焊盘的生产封装仍待官方 CAD/CAE 确认。
5. 不允许为了 3D 外观而使用尺寸近似模型。外壳碰撞检查中，“无模型”优于错误模型。

## 来源与许可

封装、符号和 STEP 快照来自本机 KiCad 10.0.6 官方库。KiCad 库采用 CC-BY-SA 4.0 并带设计输出例外；重新分发库集合时仍需保留许可与归属。项目自定义 POGO 封装在本仓库内维护。
