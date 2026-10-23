# USB-C 封装导入记录

日期：2026-09-28

## J1：主控 USB

- 选型：GCT `USB4105-GF-A`，16-contact USB 2.0 Type-C 母座。
- 原理图使用 14P USB2-only 符号，不包含 SBU1/SBU2。
- 导入封装：`PISCIS_Private:USB_C_Receptacle_GCT_USB4105-xx-A_14P_Compat_TopMnt_Horizontal`。
- 封装几何沿用项目中已按 GCT 图纸建立的 USB4105 封装；实体 A8/B8 焊盘保留，但设为无编号机械焊盘，明确保持未连接，其余 A/B 面触点及外壳 `SH` 与原理图逐一对应。
- 安装方向：上侧板边，插口朝板外。

官方资料：<https://gct.co/files/drawings/usb4105.pdf>

## USBC1：CH340 串口 USB

- 选型：Korean Hroparts `TYPE-C-31-M-12`，LCSC `C165948`。
- 导入封装：`PISCIS_Private:USB-C_SMD-TYPE-C-31-M-12_1`。
- 数据来源：项目中留存的 LCSC/EasyEDA 原始记录 `easyeda_components/raw/C165948.json`。
- 已核对 16 个信号/电源触点、4 个外壳固定脚和 2 个定位孔；焊盘编号与私有库符号完全一致。
- 安装方向：右侧板边，插口朝板外。

官方/供应商资料：<https://datasheet.lcsc.com/lcsc/1903211732_Korean-Hroparts-Elec-TYPE-C-31-M-12_C165948.pdf>

## 网络核对

- J1：VBUS、GND、CC1、CC2、USB_DP、USB_DM 和 SH 均已匹配；A8/B8 明确未连接。
- USBC1：VBUS、GND、CC1、CC2、DP、DN、SBU1、SBU2 和 4 个外壳脚均已匹配。
- 两个连接器均已加入根目录 PCB，并通过 courtyard 重叠、板框和安装孔保留区检查。

当前未导入 C165948 的 3D 模型；这不影响焊盘、钻孔或生产文件，但三维预览中仅显示该连接器的板级图形。
