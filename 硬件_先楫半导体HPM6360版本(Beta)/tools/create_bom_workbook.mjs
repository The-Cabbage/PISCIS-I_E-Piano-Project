import fs from "node:fs/promises";
import { SpreadsheetFile, Workbook } from "/Users/admin/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/@oai/artifact-tool/dist/artifact_tool.mjs";

const outputDir = `${process.cwd()}/outputs/01a0a15e-6745-7f52-aa7f-aea0cbdf02fa/`;
const outputPath = `${outputDir}PISCIS-I_HPM6360_BOM_RevA0.xlsx`;

const bomRows = [
  [1,"主控",1,"HPM6360IPA2","HPMicro","600 MHz RISC-V MCU；USB 2.0 HS、I2S/TDM、CAN-FD、108 GPIO、16-bit SDRAM","eLQFP144L；20×20 mm / 0.5 mm pitch / 中央裸露焊盘","SMT（外围脚可手焊）","否","首选","待从官方/参考工程导入并核脚","待建立 HPMicro eLQFP144L 精确封装","LQFP-144_20x20mm_P0.5mm.step","已收集 STEP","https://www.hpmicro.com/Public/Uploads/uploadfile/files/20250205/HPM6300DSV27.pdf","确认 Figure 26 中适用于 HPM6360 的 D2/E2 裸露焊盘尺寸，再释放生产封装","外围脚可拖焊；底部裸露焊盘仍需热台/回流或热风"],
  [2,"存储",1,"W25Q128JVPIQ","Winbond","128 Mbit QSPI NOR Flash","WSON-8 6×5 mm","SMT","否","首选","PISCIS_K10_Memory_Flash:W25Q128JVP","PISCIS_HPM6360:WSON-8-1EP_6x5mm_P1.27mm_EP3.4x4.3mm","WSON-8-1EP_6x5mm_P1.27mm_EP3.4x4.3mm.step","已收集 STEP","https://www.winbond.com/hq/product/code-storage-flash-memory/serial-nor-flash/?__locale=en&partNo=W25Q128JV","冻结温度等级和订货后缀","符号与封装均来自 KiCad 10 快照"],
  [3,"存储",1,"W9825G6KH-6I","Winbond","256 Mbit、16-bit SDRAM；HPM6360EVK 同系列参考","TSOP-II-54 22.2×10.16 mm / 0.8 mm pitch","SMT","可选","候选待冻结","待建立/导入","PISCIS_HPM6360:TSOP-II-54_22.2x10.16mm_P0.8mm","TSOP-II-54_22.2x10.16mm_P0.8mm.step","已收集 STEP","https://www.winbond.com/hq/product/specialty-dram/sdram/?__locale=en&partNo=W9825G6KH","算法内存预算后决定是否实装","如内置 SRAM 足够可 DNP"],
  [4,"USB",1,"USB4105-GF-A","GCT","USB 2.0 Type-C 16-contact 母座；USB-MIDI + HS 数据","Top-mount horizontal；壳脚长度按后缀","SMT+TH shell","否","首选","USB_C_Receptacle_USB2.0_16P","PISCIS_HPM6360:USB_C_Receptacle_GCT_USB4105-xx-A_16P_TopMnt_Horizontal","USB_C_Receptacle_GCT_USB4105-xx-A_16P_TopMnt_Horizontal.step","已收集 STEP","https://gct.co/connector/usb4105","采购后缀必须与壳脚长度/镀层一致","官方页提供 3D 生成器和 EDA 入口"],
  [5,"USB",1,"TUSB320LAI","Texas Instruments","Type-C CC 检测、方向和电流广告；A0 配 UFP","X2QFN-12 RWB 1.6×1.6 mm","SMT","否","首选","PISCIS_K10_Interface_USB:TUSB320I","PISCIS_HPM6360:Texas_X2QFN-12_1.6x1.6mm_P0.4mm","—","官方 CAD 入口；本地未收集","https://www.ti.com/product/TUSB320LAI","核对 RWB0012A land pattern 与焊接能力","0.4 mm pitch，钢网和贴装能力需确认"],
  [6,"USB保护",1,"TPD2EUSB30DRTR","Texas Instruments","USB D+/D- 双通道低电容 ESD","SOT-9X3 / DRT-3","SMT","否","首选","PISCIS_K10_Power_Protection:TPD2EUSB30","PISCIS_HPM6360:Texas_DRT-3","Texas_DRT-3.step","已收集 STEP","https://www.ti.com/product/TPD2EUSB30","靠近 Type-C 接口放置并核 Pin-1","高速差分线不得绕行"],
  [7,"电池管理",1,"BQ24074RGTR","Texas Instruments","1S 锂电线性充电 + Dynamic PowerPath；支持边充边用","VQFN-16 RGT 3×3 mm","SMT","否","首选","PISCIS_K10_Battery_Management:BQ24074RGT","PISCIS_HPM6360:VQFN-16-1EP_3x3mm_P0.5mm_EP1.6x1.6mm","—","官方 CAD 入口；本地未收集","https://www.ti.com/product/BQ24074","完成 800 mA 热预算、EP/钢网/过孔设计","1.5 A 是上限，不代表 65 mm 板上可持续"],
  [8,"主电源",1,"TPS62825ADMQ","Texas Instruments","3.3 V / 2 A 同步 Buck","VSON-HR-6 DMO 1.5×1.5 mm","SMT","否","候选待冻结","待建立/导入","待建立 DMO-6","—","官方 CAD 入口；本地未收集","https://www.ti.com/product/TPS62825A","冻结精确订货号、输出配置、L/C 和 DMO 封装","超小封装，打样能力门禁"],
  [9,"扩展电源",1,"TPS61088RHLR","Texas Instruments","单节锂电升压 5V_EXP；扩展底板供电","VQFN-20 RHL 4.5×3.5 mm","SMT","可选","选配/DNP","待建立/导入","PISCIS_HPM6360:Texas_VQFN-RHL-20","—","封装已收集；STEP 未收集","https://www.ti.com/product/TPS61088","先冻结扩展功率预算和电感/散热设计","A0 默认允许 DNP"],
  [10,"扩展电源",1,"TPS22919DCKR","Texas Instruments","5V_EXP 1.5 A 负载开关；限浪涌/短路保护","SC-70-6 / DCK","SMT","否","首选","PISCIS_K10_Power_Management:TPS22919DCK","PISCIS_HPM6360:SOT-363_SC-70-6","SOT-363_SC-70-6.step","已收集 STEP","https://www.ti.com/product/TPS22919","核 DCK 引脚编号和热条件","位于 boost 与 POGO 之间"],
  [11,"电池管理",1,"BQ27441DRZR-G1A","Texas Instruments","1S 电量计；显示 SOC/剩余时间时使用","VSON-12 DRZ","SMT","是","选配/DNP","PISCIS_K10_Battery_Management:BQ27441DRZR-G1A","待建立 DRZ-12","—","未收集","https://www.ti.com/product/BQ27441-G1","UI 是否需要可信 SOC 决定是否实装","默认 DNP，可用 MCU ADC 粗测电压"],
  [12,"音频",1,"TLV320AIC3204IRHBR","Texas Instruments","立体声 ADC/DAC、I2S/TDM、耳放和 line out","VQFN-32 RHB 5×5 mm","SMT","否","首选","待建立/导入","PISCIS_HPM6360:Texas_RHB0032E_VQFN-32-1EP_5x5mm_P0.5mm_EP3.45x3.45mm","Texas_RHB0032E_VQFN-32-1EP_5x5mm_P0.5mm_EP3.45x3.45mm.step","已收集 STEP","https://www.ti.com/product/TLV320AIC3204","核 RHB0032E/M EP 版本和音频电源树","耳机由 codec 驱动；line out 送扬声器功放"],
  [13,"音频",1,"TPA2012D2RTJR","Texas Instruments","双路模拟输入 Class-D 扬声器功放","WQFN-20 RTJ 4×4 mm","SMT","否","首选","待建立/导入","PISCIS_HPM6360:WQFN-20-1EP_4x4mm_P0.5mm_EP2.7x2.7mm","WQFN-20-1EP_4x4mm_P0.5mm_EP2.7x2.7mm.step","已收集 STEP","https://www.ti.com/product/TPA2012D2","核 RTJ0020A EP 与输出 EMI 网络","按 8 Ω 扬声器和电池最低电压复算功率"],
  [14,"音频接口",1,"SJ-3523-SMT-TR","Same Sky","3.5 mm 立体声 TRS 水平贴片耳机座","Horizontal SMT","SMT","否","首选","Connector:AudioJack3","PISCIS_HPM6360:Jack_3.5mm_CUI_SJ-3523-SMT_Horizontal","—","官方明确无 3D 模型","https://www.sameskydevices.com/product/interconnect/connectors/audio-connectors/jacks/sj-3523-smt-tr","用数据手册机械包络校核机壳开孔","无插入检测开关"],
  [15,"扬声器",2,"CMS-151125-18SP-X8","Same Sky","15×11 mm、8 Ω、1 W 被动扬声器","机壳内置；焊线","OFFBOARD","否","候选待冻结","不上板","不上板","—","官方明确无 3D 模型","https://www.sameskydevices.com/product/audio/speakers/miniature-%2810-mm~40-mm%29/cms-151125-18sp-x8","声腔、线长和实际声压样机验证","结构设计用 15×11 mm 包络"],
  [16,"扬声器连接",2,"SM02B-SRSS-TB(LF)(SN)","JST","双路扬声器 1.0 mm 间距水平插座","JST SH 1×2","SMT","否","首选","Connector_Generic:Conn_01x02","PISCIS_HPM6360:JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal","JST_SH_SM02B-SRSS-TB_1x02-1MP_P1.00mm_Horizontal.step","已收集 STEP","https://www.jst-mfg.com/product/index.php?series=231","同时预留并联焊盘和清晰极性","线端壳 SHR-02V-S"],
  [17,"电池连接",1,"S2B-PH-K-S(LF)(SN)","JST","1S 锂电池连接器，2.0 mm 间距侧插","JST PH 1×2","TH","否","首选","Connector_Generic:Conn_01x02","PISCIS_HPM6360:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal","JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal.step","已收集 STEP","https://www.jst-mfg.com/product/index.php?series=199","冻结电池线束极性和壳体方向","不沿用不同厂商的反极性成品线定义"],
  [18,"控制件",5,"PTA1543-2010DPB103","Bourns","15 mm 行程、10 kΩ 线性滑动电位器","PTA1543 THT","TH","否","候选待冻结","Device:R_Potentiometer","PISCIS_HPM6360:Potentiometer_Bourns_PTA1543_Single_Slide","—","封装已收集；STEP 未收集","https://www.bourns.com/docs/Product-Datasheets/PTA.pdf","精确后缀、滑杆高度、旋钮和面板槽需样品确认","5 只在 65 mm 板上机械风险高"],
  [19,"控制件",6,"PEC11R-4215F-S0024","Bourns","12 mm、24 detents、增量编码器带按压","PEC11R-4xxxF-S；THT","TH","否","候选待冻结","Device:Rotary_Encoder_Switch","待建立 PEC11R-4xxxF-S","—","未收集","https://www.bourns.com/docs/Product-Datasheets/PEC11R.pdf","按官方图建立封装并用实物 1:1 校核","需要 18 路数字输入或矩阵/扩展器"],
  [20,"显示",1,"WEO012832D（完整后缀待定）","Winstar","0.91 in、128×32 OLED、SSD1306、3 V、4-wire SPI","COG/FPC；约 30×11.5×1.26 mm","FPC","否","候选待冻结","待按接口版本建立","待按 FPC 版本建立","—","未取得","https://www.winstar.com.tw/products/oled-module/graphic-oled-display/128x32-oled.html","冻结完整后缀、FPC 引脚/方向、可视区","不要先放通用 4-pin OLED 模块封装"],
  [21,"模块总线",1,"TCAN3413DDFR","Texas Instruments","3.3 V CAN-FD 收发器；模块控制/MIDI 事件总线","SOT-23-THIN-8 DDF","SMT","否","首选","待建立/导入","PISCIS_HPM6360:Texas_DDF0008A_SOT-8_1.6x2.9mm_P0.65mm","—","封装已收集；STEP 未收集","https://www.ti.com/product/TCAN3413","核 DDF0008A；级联终端电阻默认 DNP","端节点才装 120 Ω"],
  [22,"调试",1,"TC2030-IDC-NL","Tag-Connect","6-pin 无排针调试/量产触点","2×3 / 1.27 mm","CONTACT","否","候选待冻结","Connector_Generic:Conn_02x03_Odd_Even","PISCIS_HPM6360:Tag-Connect_TC2030-IDC-NL_2x03_P1.27mm_Vertical","—","封装已收集；STEP 未收集","https://www.tag-connect.com/product/tc2030-idc-nl","核 HPM JTAG 信号数；不足则改 10-pin","另设 RESET/BOOT/电源测试点"],
  [23,"扩展接口",1,"CUSTOM-POGO-2X12-P2.00","Project","24-pin 底层 POGO；5V、CAN-FD、TDM、I2C/维护及级联信号","2×12 / 2.00 mm bottom pads","CONTACT","否","自定义接口","Connector_Generic:Conn_02x12_Odd_Even","PISCIS_HPM6360:Pogo_Interface_2x12_P2.00mm_Bottom","—","不适用","docs/POGO_INTERFACE.md","与底板 POGO 型号、压缩量、开窗和禁布区联调","当前是接口占位和电气定义基线"],
  [24,"电池",1,"1S LiPo pack with PCM + 10k NTC（TBD）","TBD","单节锂聚合物电池包；保护板与温度检测","由机壳和续航预算决定","OFFBOARD","否","待需求冻结","不上板","不上板","—","供应商相关","docs/REQUIREMENTS.md","先完成平均/峰值功耗、容量、厚度与运输认证预算","不得采购无保护裸电芯作为成品方案"]
];

const passiveRows = [
  [1,"MCU/数字去耦","100 nF","0402 X7R 10 V",30,"±10%","数量估算","每个电源脚附近；最终按 HPM6360 电源域统计"],
  [2,"局部储能","1 µF","0402/0603 X7R 10 V",12,"±10%","数量估算","MCU、Codec、接口 IC 周边"],
  [3,"电源输入/输出","4.7 µF","0603 X7R 10 V",10,"降额后有效电容需校核","数量估算","Buck、LDO、模拟电源"],
  [4,"电源输入/输出","10 µF","0805 X7R 10 V",10,"降额后有效电容需校核","数量估算","充电器和 3.3 V 电源"],
  [5,"Boost/功放储能","22 µF","0805/1206 X7R 10 V",8,"低 ESR","数量估算","TPS61088 与 Class-D 回路"],
  [6,"系统 bulk","47–100 µF","1210/低高度聚合物",2,"按纹波/高度选择","待电源仿真","POGO 5 V 和扬声器负载阶跃"],
  [7,"通用上拉/下拉","10 kΩ","0402 1%",24,"50 ppm/°C", "数量估算","BOOT、EN、状态和控制输入"],
  [8,"I²C 上拉","4.7 kΩ","0402 1%",4,"按总线电容复算","候选","板内与 POGO 总线可能需分域"],
  [9,"滑杆 ADC 串联","1 kΩ","0402 1%",5,"与 100 nF 构成输入低通","候选","保护 ADC 并抑制滑杆噪声"],
  [10,"滑杆 ADC 滤波","100 nF","0402 X7R",5,"靠近 MCU ADC","候选","截止频率需按扫描速率复算"],
  [11,"编码器上拉","10 kΩ","0402 1%",18,"可由 MCU 内部上拉替代","DNP 候选","噪声测试后决定"],
  [12,"编码器去抖","10 nF","0402 C0G/X7R",18,"与上拉配合","DNP 候选","固件去抖足够时省略"],
  [13,"USB 串联阻尼","22 Ω","0402 1%",2,"按 HPM 参考设计/眼图调整","DNP 候选","靠近发送端；不可先盲装"],
  [14,"CAN 终端","120 Ω","0603 1%",1,"仅物理链末端装","默认 DNP","级联中间模块不得装终端"],
  [15,"CAN 分裂终端","60.4 Ω","0402 1%",2,"与中心电容配套","默认 DNP","与单 120 Ω 二选一"],
  [16,"模拟/数字电源隔离","600 Ω @100 MHz ferrite bead","0603",4,"额定电流按电源域","候选","Codec AVDD、DVDD/OLED 等"],
  [17,"Buck 电感","约 0.47–1.0 µH","≤3×3 mm shielded",1,"Isat/Irms 按 TI 设计工具","待计算","TPS62825A 输出网络未冻结"],
  [18,"Boost 电感","约 1.0–2.2 µH","低 DCR shielded",1,"Isat 需覆盖峰值电流","待计算","TPS61088 需按 5V_EXP 最大功率计算"],
  [19,"音频输入/输出网络","R/C 组合","0402/0603",1,"按 Codec/功放推荐值","待原理图","不在架构 BOM 中虚构数量"],
  [20,"扬声器 EMI 网络","磁珠/电容或共模器件","0603/0805",2,"以 EMC 与线长测试决定","DNP 候选","Class-D 输出不得直接对地加大电容"]
];

const sourceRows = [
  [1,"HPMicro","产品系列/封装","https://www.hpmicro.com/en/product-center/microcontroller/hpm6300",new Date("2026-09-16T00:00:00+08:00"),"HPM6360IPA2 产品页显示 LQFP144；数据手册将 PA 明确定义为 20×20 mm eLQFP144 P0.5"],
  [2,"HPMicro","HPM6360EVK 用户手册","https://www.hpmicro.com/Public/Uploads/uploadfile/files/20241024/HPM6360EVKUGV1.0.pdf",new Date("2026-09-16T00:00:00+08:00"),"确认 EVK 使用 W9825G6KH-6I SDRAM 与 W25Q128 系列 Flash"],
  [3,"OSHWHub","用户指定参考工程","https://oshwhub.com/hasaki6/hpm6360-ce-shi-ban",new Date("2026-09-16T00:00:00+08:00"),"自动抓取受 403/超时限制；未把页面内未验证库文件直接纳入项目"],
  [4,"Texas Instruments","器件产品页/CAD 入口","https://www.ti.com/design-development/packaging.html",new Date("2026-09-16T00:00:00+08:00"),"TI 产品页提供 CAD symbols/footprints/3D 入口；部分经 Ultra Librarian，不等同于已下载"],
  [5,"GCT","USB4105 产品与 3D/EDA 入口","https://gct.co/connector/usb4105",new Date("2026-09-16T00:00:00+08:00"),"产品为 Active，16 contacts、USB 2.0、Top Mount Horizontal"],
  [6,"Bourns","PTA 系列数据手册","https://www.bourns.com/docs/Product-Datasheets/PTA.pdf",new Date("2026-09-16T00:00:00+08:00"),"PTA1543 15 mm travel；精确后缀仍需样品冻结"],
  [7,"Bourns","PEC11R 数据手册","https://www.bourns.com/docs/Product-Datasheets/PEC11R.pdf",new Date("2026-09-16T00:00:00+08:00"),"PEC11R-4xxxF-S 的安装尺寸用于后续自建封装"],
  [8,"Same Sky","SJ-3523 3D 页面","https://www.sameskydevices.com/product/resource/3dmodel/sj-3523-smt-tr",new Date("2026-09-16T00:00:00+08:00"),"官网当前明确显示该型号 3D model 不存在"],
  [9,"Same Sky","CMS-151125-18SP-X8 3D 页面","https://www.sameskydevices.com/product/resource/3dmodel/cms-151125-18sp-x8",new Date("2026-09-16T00:00:00+08:00"),"官网当前明确显示该型号 3D model 不存在"],
  [10,"KiCad","官方 3D 模型库","https://gitlab.com/kicad/libraries/kicad-packages3D",new Date("2026-09-16T00:00:00+08:00"),"项目快照来自本机 KiCad 10.0.6 官方库；许可随项目保留"],
  [11,"KiCad","库许可","https://gitlab.com/kicad/libraries/kicad-packages3D-source/-/blob/master/LICENSE.md",new Date("2026-09-16T00:00:00+08:00"),"CC-BY-SA 4.0 + KiCad design-output exception"],
  [12,"HPMicro","HPM6300 数据手册 Rev2.7","https://www.hpmicro.com/Public/Uploads/uploadfile/files/20250205/HPM6300DSV27.pdf",new Date("2026-09-16T00:00:00+08:00"),"PA = 20×20 mm eLQFP144 P0.5；封装图名称为 eLQFP144L，外围总尺寸约 22 mm"]
];

const wb = Workbook.create();
const bom = wb.worksheets.add("BOM_A0");
const passive = wb.worksheets.add("Passive_Plan");
const lib = wb.worksheets.add("Library_Index");
const sources = wb.worksheets.add("Sources");

function applyTitle(sheet, title, subtitle, endCol) {
  sheet.showGridLines = false;
  sheet.getRange(`A1:${endCol}1`).format.fill = "#17365D";
  sheet.getRange(`A1:${endCol}1`).format.font = { bold: true, color: "#FFFFFF", size: 18 };
  sheet.getRange("A1").values = [[title]];
  sheet.getRange(`A2:${endCol}2`).format.fill = "#D9EAF7";
  sheet.getRange(`A2:${endCol}2`).format.font = { italic: true, color: "#244062", size: 10 };
  sheet.getRange("A2").values = [[subtitle]];
  sheet.getRange(`A1:${endCol}2`).format.rowHeight = 24;
}

applyTitle(bom,"PISCIS-I HPM6360 Rev A0 架构 BOM","候选器件与 CAD/STEP 状态；不是可直接下单的量产 BOM。所有“待冻结/门禁”项必须在原理图和采购前关闭。","Q");
bom.getRange("A3:Q3").values = [["版本","Rev A0","","生成日期",new Date("2026-09-16T00:00:00+08:00"),"","PCB 上限","≤65 × 65 mm","","BOM 性质","架构候选 / 不可直接下单","","","","","",""]];
bom.getRange("E3").setNumberFormat("yyyy-mm-dd");
const bomStart = 8;
const bomEnd = bomStart + bomRows.length - 1;
bom.getRange("A4:Q4").values = [["主器件行数",null,"","总装配数量",null,"","已收集 STEP",null,"","默认 DNP 行数",null,"","","","","",""]];
bom.getRange("B4").formulas = [[`=COUNTA(D${bomStart}:D${bomEnd})`]];
bom.getRange("E4").formulas = [[`=SUM(C${bomStart}:C${bomEnd})`]];
bom.getRange("H4").formulas = [[`=COUNTIF(N${bomStart}:N${bomEnd},"已收集 STEP")`]];
bom.getRange("K4").formulas = [[`=COUNTIF(I${bomStart}:I${bomEnd},"是")`]];
bom.getRange("A3:Q4").format.borders = { preset: "all", style: "thin", color: "#B4C6E7" };
bom.getRange("A5:Q5").format.fill = "#FFF2CC";
bom.getRange("A5:Q5").format.font = { bold: true, color: "#9C5700" };
bom.getRange("A5").values = [["采购门禁：HPM6360 eLQFP144L 中央裸露焊盘/引脚表、OLED 完整后缀、PEC11R 封装、电源 L/C、机械 1:1 排布尚未冻结。"]];
const bomHeaders = [["Line","类别","Qty/板","MPN / 订货号","制造商","功能/规格","封装/尺寸","装配","默认 DNP","选型状态","KiCad 符号","项目封装","本地 STEP 文件","3D 状态","数据手册 / CAD 来源","采购/设计门禁","备注"]];
bom.getRange("A7:Q7").values = bomHeaders;
bom.getRange(`A8:Q${bomEnd}`).values = bomRows;
const bomTable = bom.tables.add(`A7:Q${bomEnd}`, true, "BomA0Table");
bomTable.style = "TableStyleMedium2";
bom.freezePanes.freezeRows(7);
bom.getRange(`A7:Q${bomEnd}`).format.verticalAlignment = "center";
bom.getRange(`D8:Q${bomEnd}`).format.wrapText = true;
bom.getRange(`A8:C${bomEnd}`).format.horizontalAlignment = "center";
bom.getRange(`H8:J${bomEnd}`).format.horizontalAlignment = "center";
bom.getRange(`N8:N${bomEnd}`).conditionalFormats.add("containsText", { text: "已收集 STEP", format: { fill: "#C6EFCE", font: { color: "#006100", bold: true } } });
bom.getRange(`N8:N${bomEnd}`).conditionalFormats.add("containsText", { text: "未收集", format: { fill: "#FFF2CC", font: { color: "#9C5700" } } });
bom.getRange(`I8:I${bomEnd}`).conditionalFormats.add("containsText", { text: "是", format: { fill: "#E7E6E6", font: { color: "#666666", italic: true } } });
bom.getRange(`O8:O${bomEnd}`).format.font = { color: "#0563C1", underline: true };

const pEnd = 7 + passiveRows.length;
applyTitle(passive,"被动器件与磁性件计划","数量为原理图前估算，用于面积/成本/采购准备；不得作为贴片厂最终数量。","H");
passive.getRange("A4:H4").values = [["Line","功能","值/类别","建议封装","估算 Qty","首选规格","状态","说明"]];
passive.getRange(`A5:H${pEnd - 3}`).values = passiveRows;
const pTable = passive.tables.add(`A4:H${pEnd - 3}`, true, "PassivePlanTable");
pTable.style = "TableStyleMedium4";
passive.freezePanes.freezeRows(4);
passive.getRange(`B5:H${pEnd - 3}`).format.wrapText = true;
passive.getRange(`A5:A${pEnd - 3}`).format.horizontalAlignment = "center";

const libHeaders = [["Line","MPN / 对象","KiCad 符号","项目封装","本地 STEP","3D 状态","符号状态","封装状态","关键门禁","来源 URL"]];
const libRows = bomRows.slice(0,23).map((r) => [r[0],r[3],r[10],r[11],r[12],r[13],r[10].startsWith("待") ? "待建立/导入" : "已有/通用",r[11].startsWith("待") ? "待建立" : "候选已收集",r[15],r[14]]);
const libEnd = 5 + libRows.length - 1;
applyTitle(lib,"符号 / 封装 / 3D 模型索引","“已有”仅表示文件存在；精确封装仍以数据手册和实物 1:1 校核为准。","J");
lib.getRange("A4:J4").values = libHeaders;
lib.getRange(`A5:J${libEnd}`).values = libRows;
const libTable = lib.tables.add(`A4:J${libEnd}`, true, "LibraryIndexTable");
libTable.style = "TableStyleMedium9";
lib.freezePanes.freezeRows(4);
lib.getRange(`B5:J${libEnd}`).format.wrapText = true;
lib.getRange(`F5:F${libEnd}`).conditionalFormats.add("containsText", { text: "已收集 STEP", format: { fill: "#C6EFCE", font: { color: "#006100", bold: true } } });
lib.getRange(`I5:I${libEnd}`).conditionalFormats.add("containsText", { text: "必须", format: { fill: "#F4CCCC", font: { color: "#9C0006", bold: true } } });
lib.getRange(`J5:J${libEnd}`).format.font = { color: "#0563C1", underline: true };

const srcEnd = 5 + sourceRows.length - 1;
applyTitle(sources,"资料来源与核实记录","优先使用器件制造商和 KiCad 官方来源；链接用于后续封装/生命周期复核。","F");
sources.getRange("A4:F4").values = [["Ref","来源","资料类型","URL / 项目路径","核实日期","核实结论"]];
sources.getRange(`A5:F${srcEnd}`).values = sourceRows;
sources.getRange(`E5:E${srcEnd}`).setNumberFormat("yyyy-mm-dd");
const srcTable = sources.tables.add(`A4:F${srcEnd}`, true, "SourcesTable");
srcTable.style = "TableStyleMedium2";
sources.freezePanes.freezeRows(4);
sources.getRange(`B5:F${srcEnd}`).format.wrapText = true;
sources.getRange(`D5:D${srcEnd}`).format.font = { color: "#0563C1", underline: true };

const widths = {
  BOM_A0: [7,12,8,25,18,35,28,12,11,16,32,48,44,20,46,42,38],
  Passive_Plan: [7,22,22,22,12,28,16,46],
  Library_Index: [7,27,38,52,48,22,18,18,52,46],
  Sources: [7,20,28,58,14,64]
};
for (const [sheetName, cols] of Object.entries(widths)) {
  const sheet = wb.worksheets.getItem(sheetName);
  cols.forEach((width, i) => sheet.getCell(0, i).format.columnWidth = width);
  sheet.getUsedRange().format.autofitRows();
  sheet.getUsedRange().format.verticalAlignment = "center";
}

await fs.mkdir(outputDir, { recursive: true });
const xlsx = await SpreadsheetFile.exportXlsx(wb);
await xlsx.save(outputPath);

const inspect = await wb.inspect({ kind: "workbook,sheet,table", maxChars: 8000, tableMaxRows: 4, tableMaxCols: 8, tableMaxCellChars: 80 });
console.log(inspect.ndjson ?? inspect);
const formulaErrors = await wb.inspect({ kind: "match", searchTerm: "#REF!|#DIV/0!|#VALUE!|#NAME\\?|#N/A", options: { useRegex: true, maxResults: 50 }, maxChars: 5000 });
console.log(formulaErrors.ndjson ?? formulaErrors);

for (const sheetName of ["BOM_A0","Passive_Plan","Library_Index","Sources"]) {
  const preview = await wb.render({ sheetName, autoCrop: "all", scale: 0.75, format: "png" });
  const bytes = new Uint8Array(await preview.arrayBuffer());
  await fs.writeFile(`/tmp/PISCIS_${sheetName}.png`, bytes);
}

// artifact-tool may spill a large inspect result next to the workbook; keep the deliverable directory XLSX-only.
await fs.rm(`${outputPath}.inspect.ndjson`, { force: true });

console.log(outputPath);
