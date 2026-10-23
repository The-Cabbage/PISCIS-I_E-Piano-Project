# Rev A0 库与 3D 模型清单

状态定义：

- **已收集 STEP**：项目目录内已有 STEP；已有候选封装的器件同时改为 `${KIPRJMOD}` 相对路径。只有机械模型、尚无生产封装的器件会单独注明。
- **封装已收集**：已有候选 `.kicad_mod`，但 STEP 缺失或仍需与最新数据手册复核。
- **符号已收集**：项目内已有 KiCad 10 官方符号快照。
- **门禁**：在进入正式原理图/PCB 前必须完成的动作。

| 器件 | 符号 | 项目封装 | STEP | 当前状态 / 门禁 |
|---|---|---|---|---|
| HPM6360IPA2 | `PISCIS_Private:HPM6360IPA2`（官方数据手册图 2 派生，145 pin） | **`PISCIS_Private:HPM6360IPA2_EQFP144_20x20mm_P0.5mm_LS22mm_EP5x5mm_OSHWHub`** | `LQFP-144_20x20mm_P0.5mm.step` | 已从 `ProPrj_HPM6360-测试板_2026-09-19.epro2` 的原始 `.epru` 数据导入、含 145 PAD，并通过 KiCad SVG 导出验证。来源封装为 `EQFP-144_L20.0-W20.0-P0.50-LS22.0-TL-EP5.0`：外围 pad 1.6600×0.2800 mm、EP 5.0000×5.0000 mm。保留 `...EP5x5mm` 数据手册候选作为带分窗焊膏的生产工艺备选 |
| W25Q128JVPIQ | `PISCIS_K10_Memory_Flash:W25Q128JVP` | `PISCIS_HPM6360:WSON-8-1EP_6x5mm_P1.27mm_EP3.4x4.3mm` | 已收集 | 订货后缀与温度等级仍需冻结 |
| W9825G6KH-6I | 待建立/导入 | `PISCIS_HPM6360:TSOP-II-54_22.2x10.16mm_P0.8mm` | 已收集 | 与 EVK 同系列；核引脚表和布线时序 |
| USB4105-GF-A | 使用 Type-C 16-pin 标准符号 | `PISCIS_HPM6360:USB_C_Receptacle_GCT_USB4105-xx-A_16P_TopMnt_Horizontal` | 已收集 | 壳脚长度后缀必须与采购料一致 |
| TUSB320LAI | `PISCIS_K10_Interface_USB:TUSB320I` | `PISCIS_HPM6360:Texas_X2QFN-12_1.6x1.6mm_P0.4mm` | 未收集 | 核对 RWB 的尺寸与焊盘图；官方页面提供 CAD 入口 |
| TPD2EUSB30DRTR | `PISCIS_K10_Power_Protection:TPD2EUSB30` | `PISCIS_HPM6360:Texas_DRT-3` | 已收集 | D+/D- 走线方向和 Pin-1 需在摆放时检查 |
| BQ24074RGTR | `PISCIS_K10_Battery_Management:BQ24074RGT` | `PISCIS_HPM6360:VQFN-16-1EP_3x3mm_P0.5mm_EP1.6x1.6mm` | 未收集 | 校核 RGT0016A EP、散热过孔与焊膏开窗 |
| TPS62825ADMQ | 待建立/导入 | 待建立 VSON-HR-6 / DMO | 未收集 | 精确订货号和 DMO 封装需先冻结 |
| TPS61088RHLR | 待建立/导入 | `PISCIS_HPM6360:Texas_VQFN-RHL-20` | 未收集 | 候选封装来自 KiCad；必须与 RHL0020A 核对 |
| TPS22919DCKR | `PISCIS_K10_Power_Management:TPS22919DCK` | `PISCIS_HPM6360:SOT-363_SC-70-6` | 已收集 | 核对 DCK 引脚编号 |
| BQ27441DRZR-G1A（选配） | `PISCIS_K10_Battery_Management:BQ27441DRZR-G1A` | 待建立 DRZ-12 | 未收集 | DNP 默认；决定是否显示 SOC 后再投入 |
| TLV320AIC3204IRHBR | 待建立/导入 | `PISCIS_HPM6360:Texas_RHB0032E_VQFN-32-1EP_5x5mm_P0.5mm_EP3.45x3.45mm` | 已收集 | RHB 有多个 EP 版本，须按器件数据手册确认 E/M 变体 |
| TPA2012D2RTJR | 待建立/导入 | `PISCIS_HPM6360:WQFN-20-1EP_4x4mm_P0.5mm_EP2.7x2.7mm` | 已收集 | 与 RTJ0020A 核对 EP 和 Pin-1 |
| SJ-3523-SMT-TR | 使用 TRS Jack 通用符号并标 MPN | `PISCIS_HPM6360:Jack_3.5mm_CUI_SJ-3523-SMT_Horizontal` | 无可靠模型 | Same Sky 官方 3D 页面当前明确显示无模型；外壳用数据手册包络建模 |
| SM02B-SRSS-TB(LF)(SN) | 2-pin connector | `PISCIS_HPM6360:JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal` | 已收集 | 两路扬声器各 1 个 |
| S2B-PH-K-S(LF)(SN) | 2-pin connector | `PISCIS_HPM6360:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal` | 已收集 | 电池接口；极性丝印必须清楚 |
| PTA1543-2010DPB103 | `Device:R_Potentiometer` | `PISCIS_HPM6360:Potentiometer_Bourns_PTA1543_Single_Slide` | 未收集 | 精确后缀、滑杆高度和面板开槽待样品冻结 |
| PEC11R-4215F-S0024 | `Device:Rotary_Encoder_Switch` | 待建立 PEC11R-4xxxF-S 精确封装 | 未收集 | 依据 Bourns 图纸建立并用实物 1:1 校核 |
| EC09E1524418 | 待建立/导入 | 待建立（以原厂安装孔图为准） | 已收集 | 70×70 控制面板的 9 mm 顶面操作候选；6 枚建议 3×2 排列。此特定型号为 20 mm 平轴，必须与外壳/旋钮高度共同冻结 |
| EC05E1220202 | 待建立/导入 | 待建立（以原厂 SMT land 图为准） | 已收集 | **5 mm、SMT/回流焊、侧向操作、4.5 mm 安装高度**；纵向空间极紧时的 6 枚首选 |
| EC05E1220401 | 待建立/导入 | 待建立（以原厂 SMT land 图为准） | 已收集 | **5 mm、SMT/回流焊、顶面操作**；需要顶面旋钮时的优先候选，先用 STEP 复核总高度 |
| SSSS811501 | 待建立/导入 | 待建立（以原厂 land pattern 为准） | 已收集 | 1P3T、9.7×2.6×1.4 mm、水平拨动；推荐板边布置，外壳须留拨动槽 |
| WEO012832D | 待按接口版本建立 | 待按 FPC/引脚版本建立 | 未取得 | 先冻结完整后缀、FPC 方向、玻璃区和可视区 |
| X096-2864KSWPG17-C30 | 待建立/导入 | 30P、0.5 mm FPC 座待建立 | 未收集 | Wisevision 0.96 in、128×64、SSD1315；立创商品 `C18723027` 当前没有公开 3D 模型，采用前须以数据手册复核 30P FPC 尾排接触面 |
| TF31-30S-0.5SH(800) | 30P FPC connector | 待按 Hirose 2D 图建立 | 已收集 | Hirose 原厂：底接触、0.5 mm pitch、0.30 mm FPC、SMT/ZIF、2.0 mm 高；可与 OLED 尾排匹配，但须确认尾排触点朝下 |
| CMS-151125-18SP-X8 | 不上板 | 不上板；PCB 端用 JST-SH/焊盘 | 官方无模型 | 机壳 BOM 件；用 15×11 mm 包络做结构验证 |
| TCAN3413DDFR | 待建立/导入 | `PISCIS_HPM6360:Texas_DDF0008A_SOT-8_1.6x2.9mm_P0.65mm` | 未收集 | 核对 DDF0008A；CAN 终端默认 DNP |
| TC2030-IDC 调试触点 | 6-pin connector | `PISCIS_HPM6360:Tag-Connect_TC2030-IDC-NL_2x03_P1.27mm_Vertical` | 未收集 | JTAG 信号数不足时改 10-pin |
| 24-pin POGO 扩展 | 自定义 2×12 connector | `PISCIS_HPM6360:Pogo_Interface_2x12_P2.00mm_Bottom` | 不适用 | 与底板 POGO 型号、压缩高度和开窗尺寸联调 |

## 已收集 STEP 文件

1. `LQFP-144_20x20mm_P0.5mm.step`（机械外形；不定义 HPM6360 中央裸露焊盘）
2. `USB_C_Receptacle_GCT_USB4105-xx-A_16P_TopMnt_Horizontal.step`
3. `JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal.step`
4. `JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal.step`
5. `Texas_RHB0032E_VQFN-32-1EP_5x5mm_P0.5mm_EP3.45x3.45mm.step`
6. `WSON-8-1EP_6x5mm_P1.27mm_EP3.4x4.3mm.step`
7. `TSOP-II-54_22.2x10.16mm_P0.8mm.step`
8. `WQFN-20-1EP_4x4mm_P0.5mm_EP2.7x2.7mm.step`
9. `SOT-363_SC-70-6.step`
10. `Texas_DRT-3.step`
11. `ALPS_EC09E1524418.step`（原厂 9 mm 顶面操作旋转编码器，控制件候选）
12. `ALPS_SSSS811501.step`（原厂 1P3T 超薄滑动开关，控制件候选）
13. `ALPS_EC05E1220202_side.step`（原厂 5 mm 贴片编码器，侧向/4.5 mm 安装高度）
14. `ALPS_EC05E1220401_top.step`（原厂 5 mm 贴片编码器，顶面操作）
15. `Hirose_TF31-30S-0.5SH-800.step`（原厂 30P、0.5 mm、底接触、0.30 mm FPC 的 SMT ZIF 连接器）

## 复核来源

- HPMicro HPM6300 系列与 EVK：<https://www.hpmicro.com/en/product-center/microcontroller/hpm6300>
- HPM6300 数据手册 Rev2.7：<https://www.hpmicro.com/Public/Uploads/uploadfile/files/20250205/HPM6300DSV27.pdf>
- TI 产品页及 CAD 入口：<https://www.ti.com/design-development/packaging.html>
- GCT USB4105：<https://gct.co/connector/usb4105>
- Bourns PTA：<https://www.bourns.com/docs/Product-Datasheets/PTA.pdf>
- Bourns PEC11R：<https://www.bourns.com/docs/Product-Datasheets/PEC11R.pdf>
- Same Sky SJ-3523 3D 状态：<https://www.sameskydevices.com/product/resource/3dmodel/sj-3523-smt-tr>
- Same Sky CMS-151125-18SP-X8 3D 状态：<https://www.sameskydevices.com/product/resource/3dmodel/cms-151125-18sp-x8>
- KiCad 官方 3D 库：<https://gitlab.com/kicad/libraries/kicad-packages3D>
