# EasyEDA 元件收集（Rev.A）

本目录保存通过 EasyEDA 官方 API 获取的器件原始 JSON。文件名使用 LCSC 编号，便于在 EasyEDA 标准版/专业版中直接检索并放置。

## 已确认精确料号

| LCSC | 型号 | EasyEDA 封装 |
|---|---|---|
| C2928891 | GD25Q128ESIGR | SOP-8_L5.3-W5.3-P1.27-LS8.0-BL |
| C70590 | X322524MOB4SI | CRYSTAL-SMD_4P-L3.2-W2.5-BL |
| C371129 | X252024MOB4SI | CRYSTAL-SMD_4P-L2.5-W2.0-BL |
| C895444 | FUSB303BTMX | X2QFN-12_L1.6-W1.6-P0.40-BL |
| C42441843 | MT9700-N | SOT-23-5_L2.9-W1.6-P0.95-LS2.9-BL |
| C365736 | ES8388 | QFN-28_L4.0-W4.0-P0.45-BL-EP |
| C153722 | ESD5304D-10/TR | UDFN-10_L2.5-W1.0-P0.50-BL |
| C153721 | ESD5311N-2/TR | X1-DFN1006-2_L1.0-W0.6_ESD5311N-2-TR |
| C340697 | SMBJ5.0A | SMB_L4.6-W3.6-LS5.3-RD |
| C165948 | TYPE-C-31-M-12 | USB-C_SMD-TYPE-C-31-M-12_1 |
| C97572 | W9825G6KH-6I | TSOP-54_L22.2-W10.2-P0.80-LS11.8-BL |
| C2884940 | PJ-320D-B-SMT | AUDIO-SMD_XKB_PJ-320D-B |
| C19189086 | HX PJ-320-3P SMT | AUDIO-SMD_HX-PJ-320-3P-SMT |
| C720477 | TS-1088-AR02016 | SW_SPST_TS-1088-xR020（云台项目迁入） |

## 在 EasyEDA 中使用

优先在“元件库/LCSC”中直接搜索上表的 `Cxxxxxx` 编号。目录中的 JSON 作为离线核查和后续转换来源，不建议手工重画已存在的官方库器件。

## 尚需机械定型的器件

- DATA/POWER USB-C：已收集候选 `TYPE-C-31-M-12 / C165948`（16Pin USB2.0），建议两路统一，但下单前必须按外壳高度、定位柱和板边位置核对实物图纸。
- 3.5 mm TRS：已确认并收集 `hanxia HX PJ-320-3P SMT / C19189086`；端子为 `A=SLEEVE`、`B=RING`、`C=TIP`。
- 0.5 mm 60Pin BTB：不得只按“60Pin/0.5 mm”选封装；必须先确定公母座、合高、定位柱和配对系列。
- W9825G6KH-6I：已确认并收集 EasyEDA/LCSC `C97572`。

## 设计提醒

- 32.768 kHz 晶振的 Rev.A 选型已由 `X201232768KGD2SI / C383844` 更改为 `X252024MOB4SI / C371129`；旧 JSON 仅保留作来源追踪。
- `MT9700-N` 用于两个位置时复用同一库器件，各放置 1 颗。
- `ESD5311N-2/TR` 共 4 颗：POWER USB CC 两颗、音频 L/R 两颗。
- `SMBJ5.0A` 共 2 颗：两个 USB VBUS 各一颗。
- 5V Power Mux 不对应专用 IC，不应收集 TPS2116 库件。
