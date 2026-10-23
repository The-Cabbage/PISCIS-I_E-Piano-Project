# PISCIS_Private 私有器件库

此目录是项目可随仓库交付的 KiCad 私有库包。工程通过根目录的 `sym-lib-table` 和 `fp-lib-table` 注册：

- `PISCIS_Private.kicad_sym`：`PISCIS_Private:HPM6360IPA2`，共 145 个焊盘定义（144 个外围引脚 + 底部 VSS 裸露焊盘）。引脚编号和名称来自 HPMicro《HPM6300 数据手册 Rev2.7》图 2。
- 同一符号库还包含 `SM8082AAAC`、`XC6210B332MR` 和 `SPX3819M5-L-1-8`；对应 SOT-23-5 封装及 3D 模型位于 `PISCIS_Private.pretty` 和 `PISCIS_Private.3dshapes`。其中 SM8082AAAC 资产来自 EasyEDA/LCSC C479068，另两颗来自 KiCad 官方库并按厂商数据手册核对。
- `PISCIS_Private:W25Q128JVP`：华邦 128-Mbit SPI/Dual/Quad NOR Flash，已绑定私有库的 `WSON-8-1EP_6x5mm_P1.27mm_EP3.4x4.3mm` 封装与 STEP 模型；裸露焊盘为 9 号 GND 引脚。
- `PISCIS_Private:SYT13S03SMD`：矽力杰双通道超低电容 TVS/ESD 阵列（现行料号 `SY205253SMD`），1/2 脚为 `I/O_1`/`I/O_2`，3 脚为 GND；已绑定按原厂推荐焊盘图建立的 `Silergy_DFN-3_1.0x0.6mm_SYT13S03SMD` 封装。
- `PISCIS_Private:PJ-320D-B-SMT`：星坤 3.5 mm 水平贴片 4 节 TRRS 母座，已绑定 `AUDIO-SMD_XKB_PJ-320D-B` 封装。引脚为 1=TIP、4=RING1、3=RING2、2=SLEEVE；该型号没有插入检测开关。
- `PISCIS_Private:HX_PJ-320-3P_SMT`：韩下 `HX PJ-320-3P SMT / C19189086` 3.5 mm 水平贴片 3 节 TRS 母座，已绑定 `AUDIO-SMD_HX-PJ-320-3P-SMT`。厂家端子号保持为 `A=SLEEVE`、`B=RING`、`C=TIP`；该型号没有插入检测开关。
- `PISCIS_Private.pretty/`：现有的 17 个项目候选封装副本、两种 HPM6360 eLQFP144L 数据手册候选封装，以及一份从已登录 OSHWHub 工程 `.epro2` 原始数据导入的 HPM6360 封装。
- `PISCIS_Private.3dshapes/`：15 个已收集 STEP 模型的项目副本；包含五份原厂下载的紧凑控制件模型。详见 `CONTROL_MODEL_MANIFEST.md`。
- `vendor-kicad10/`：原有 KiCad 10 符号快照的副本，保留其许可证和来源；具体表项仍由 `PISCIS_K10_*` 显式注册，避免合并后符号冲突。

## Rev.A EasyEDA/LCSC 器件

以下器件已从 EasyEDA 官方组件记录转换并加入 `PISCIS_Private.kicad_sym` 与 `PISCIS_Private.pretty/`。符号的 `Footprint` 属性已经指向本项目的 `PISCIS_Private` 封装库。

| LCSC | 私有库符号 | 私有库封装 |
|---|---|---|
| C2928891 | `GD25Q128ESIGR` | `SOP-8_L5.3-W5.3-P1.27-LS8.0-BL` |
| C70590 | `X322524MOB4SI` | `CRYSTAL-SMD_4P-L3.2-W2.5-BL` |
| C371129 | `X252024MOB4SI` | `CRYSTAL-SMD_4P-L2.5-W2.0-BL` |
| C895444 | `FUSB303BTMX` | `X2QFN-12_L1.6-W1.6-P0.40-BL` |
| C42441843 | `MT9700-N` | `SOT-23-5_L2.9-W1.6-P0.95-LS2.9-BL` |
| C365736 | `ES8388` | `QFN-28_L4.0-W4.0-P0.45-BL-EP` |
| C153722 | `ESD5304D-10_TR` | `UDFN-10_L2.5-W1.0-P0.50-BL` |
| C153721 | `ESD5311N-2_TR` | `X1-DFN1006-2_L1.0-W0.6_ESD5311N-2-TR` |
| C340697 | `SMBJ5.0A_C340697` | `SMB_L4.6-W3.6-LS5.3-RD` |
| C165948 | `TYPE-C-31-M-12` | `USB-C_SMD-TYPE-C-31-M-12_1` |
| C97572 | `W9825G6KH-6I` | `TSOP-54_L22.2-W10.2-P0.80-LS11.8-BL` |
| C2884940 | `PJ-320D-B-SMT` | `AUDIO-SMD_XKB_PJ-320D-B` |
| C19189086 | `HX_PJ-320-3P_SMT` | `AUDIO-SMD_HX-PJ-320-3P-SMT` |
| C720477 | `TS-1088-AR02016` | `SW_SPST_TS-1088-xR020` |

EasyEDA 原始 JSON 保存在项目根目录 `easyeda_components/raw/`，用于来源追踪。项目现收录 `XKB PJ-320D-B-SMT / C2884940`（4 节 TRRS）与 `hanxia HX PJ-320-3P SMT / C19189086`（3 节 TRS）两种耳机座；当前均未加入 3D 模型。0.5 mm 60Pin BTB 仍等待机械型号确定。

### HX PJ-320-3P SMT 来源声明

`C19189086` 的符号与封装几何源自 **嘉立创EDA/EasyEDA官方库**（<https://lceda.cn/>、<https://easyeda.com>），并按韩下 `HX PJ-320-3P SMT` 数据手册核对后转换为 KiCad 格式。转换件保留 EasyEDA 官方库要求的来源声明；原始 API 记录保存在 `easyeda_components/raw/C19189086.json`。

`TS-1088-AR02016 / C720477` 的项目封装与 STEP 模型迁自 `云台/Edition1/02_Component_Libraries/libraries/`，符号按同一 LCSC 记录导入。它是 3.9 × 3.0 × 2.0 mm 的瞬时常开贴片轻触开关，适合 MCU RESET 等瞬时按键，不是保持型电源拨档开关。

32.768 kHz 晶振的 Rev.A 选型已由 `X201232768KGD2SI / C383844` 更改为 `X252024MOB4SI / C371129`。旧器件没有出现在当前原理图或 PCB 中，其库数据仅保留作历史追踪，不再作为 Rev.A 推荐器件。

## HPM6360IPA2 使用门禁

HPMicro 公开资料确认型号为 20 mm × 20 mm、0.5 mm pitch 的 eLQFP144L，图 26 给出了两种与芯片 L/F 尺寸关联的中央裸露焊盘：`5.00 mm × 5.00 mm` 和 `9.74 mm × 9.74 mm`。因此私有库提供：

- `PISCIS_Private:HPM6360IPA2_eLQFP144L_20x20mm_P0.5mm_EP5x5mm`
- `PISCIS_Private:HPM6360IPA2_eLQFP144L_20x20mm_P0.5mm_EP9.74x9.74mm`

两者均带有 VSS 焊盘 `145` 和分窗 F.Paste 图形。2026-09-19 已在登录后的 OSHWHub/LCSC EDA Pro 参考 PCB 中读取 U4 的原始封装属性：`EQFP-144_L20.0-W20.0-P0.50-LS22.0-TL-EP5.0`，明确对应 `EP5x5mm` 版本；因此 `PISCIS_Private:HPM6360IPA2_eLQFP144L_20x20mm_P0.5mm_EP5x5mm` 是当前已确认的选择。`EP9.74x9.74mm` 保留为数据手册另一种几何候选，不用于本参考板复刻。`LQFP-144_20x20mm_P0.5mm.step` 仅为机械外形 STEP，不证明裸露焊盘尺寸。

同时提供原始工程导入版：`PISCIS_Private:HPM6360IPA2_EQFP144_20x20mm_P0.5mm_LS22mm_EP5x5mm_OSHWHub`。它由 `hardware/libraries/sources/oshwhub/HPM6360-测试板.epru` 中的 145 个 PAD 记录自动转换，保留该参考工程的 1.6600 × 0.2800 mm 外围焊盘和 5.0000 × 5.0000 mm EP 几何。导入脚本为 `tools/import_oshwhub_hpm6360.mjs`。

导入版的 EP 沿用原工程的整面 F.Paste；量产时推荐优先采用 `...EP5x5mm` 分窗焊膏候选，并在样板回流后确认空洞率。导入版的职责是精确保存参考工程的封装几何，而不是替代焊膏工艺评审。

## OSHWHub 参考工程

参考工程为 [HPM6360-测试板](https://oshwhub.com/hasaki6/hpm6360-ce-shi-ban)，项目 UUID 为 `620b037be8e5472687ec720da9c4a6f5`，公开许可证为 GPL-3.0。2026-09-19 使用已登录会话打开 PCB1，在 U4 属性面板确认型号 `HPM6360IPA2`、LCSC 料号 `C9900061349` 与封装 `EQFP-144_L20.0-W20.0-P0.50-LS22.0-TL-EP5.0`。本私有库的符号仍是官方数据手册派生；封装几何现已由参考工程确认。若后续取得 PCB 源文件，应再对焊盘宽/长、阻焊扩展与 F.Paste 分窗作逐项机器比对。

## 来源

- HPMicro HPM6300 数据手册 Rev2.7：<https://doc.hpmicro.com/Public/Uploads/uploadfile/files/20250205/HPM6300DSV27.pdf>
- HPMicro HPM6300 CAD/CAE 资料页（访问时返回“暂无相关数据”）：<https://www.hpmicro.com/design-resources/chip-information>
- OSHWHub 参考工程：<https://oshwhub.com/hasaki6/hpm6360-ce-shi-ban>
- Winbond W25Q128JV 数据手册：<https://www.winbond.com/resource-files/w25q128jv_dtr%20revc%2003272018%20plus.pdf>
- Winbond 2025 Code Storage Flash Memory Product Selection Guide（`P` package = WSON-8 6x5 mm）：<https://www.winbond.com/export/sites/winbond/product-selection-guide/file/2025-Product-Selection-Guide-Winbond-Code-Storage-Flash-Memory.pdf>
- Silergy SYT13S03SMD / SY205253SMD 产品页：<https://www.silergy.com/productsview/sy205253smd>
- Silergy SY205253SMD 数据手册：<https://www.silergy.com/download/downloadFile?ftype=note&id=5304&type=product>
