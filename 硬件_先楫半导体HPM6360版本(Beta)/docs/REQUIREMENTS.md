# PISCIS-I HPM6360 模块硬件需求

- 文档状态：Draft / Rev A0
- 更新日期：2026-09-16
- 目标工程：`PISCIS-I_HPM6360_Module.kicad_pro`

## 1. 目标和范围

设计一块不大于 65 mm × 65 mm 的便携式音色/控制核心板，以 HPM6360 为主控，同时支持 USB-MIDI 与 USB 高速数据、立体声耳机、双扬声器、本地控件、OLED、1S 锂电边充边用，以及面向扩展底板/多音色模块的 POGO 接口。

本版不将以太网、TF 卡、板载调试器、USB 3.x SuperSpeed 或 USB-PD 高压快充纳入必须范围。原理图设计时可为 TF/SDIO 保留测试点，但不在 A0 版上承诺实装。

## 2. 系统级需求

| ID | 需求 | A0 验收方式 |
|---|---|---|
| SYS-001 | 成品 PCB 外形包络不得超过 65.0 mm × 65.0 mm。 | KiCad 板框尺寸和 Gerber 外形报告。 |
| SYS-002 | 采用 6 层堆叠作为首版基线，优先保证 144-pin QFP 扇出、USB HS 连续参考平面与模拟/开关电源隔离。 | Stack-up 审查；DRC 通过。 |
| SYS-003 | 主控使用 `HPM6360IPA2`，默认为 20×20 mm、0.5 mm pitch 的 eLQFP144L 封装。 | 原理图位号、BOM、封装和引脚复用表一致。 |
| SYS-004 | 保留 JTAG/SWD 类调试触点、RESET、BOOT_MODE[1:0]、24 MHz 主时钟与 32.768 kHz RTC 时钟。 | 上电、ISP、J-Link/CMSIS-DAP 下载和时钟测试。 |
| SYS-005 | 关键外设引脚必须通过 HPM Pinmux Tool/HPM SDK 验证，不仅依赖开发板排针位置。 | 保存 pinmux 导出文件和人工交叉复核。 |

## 3. USB Type-C 与 MIDI

| ID | 需求 | A0 基线 |
|---|---|---|
| USB-001 | 使用一个紧凑型 16-contact USB Type-C 母座，连接 HPM6360 USB0 内置 HS PHY。 | GCT `USB4105-GF-A` 或经封装复核的同类器件。 |
| USB-002 | 产品 A0 作 UFP/Sink 设备，支持 USB 2.0 High-Speed 480 Mb/s。 | 不布 USB 3.x TX/RX 超高速差分对。 |
| USB-003 | 固件以 USB composite device 形式同时暴露 USB-MIDI 和高速数据类（初定 vendor bulk）。 | Windows/macOS/Linux 枚举；MIDI 双向和大包连续传输。 |
| USB-004 | D+/D- 配置低电容 ESD 保护，以 90 Ω 差分阻抗、等长、无支节为布线约束。 | TPD2EUSB30 类保护器；USB-IF 预一致性检查。 |
| USB-005 | CC1/CC2 检测不得用于推断未公告的充电电流；高于默认电流时须按 Type-C Rp 结果限流。 | TUSB320LAI 检测 Default/1.5 A/3 A，MCU 控制充电器限流模式。 |

注：如需 USB Host/OTG 或 USB-PD，需增加 VBUS source 开关、放电、过流和更完整的 Type-C/PD 控制，不能将 A0 UFP 原理图直接视为 OTG 方案。

## 4. 电源与锂电

| ID | 需求 | A0 基线 |
|---|---|---|
| PWR-001 | 支持单节 3.7 V 锂离子/锂聚合物电池，建议 1000–2000 mAh、自带保护板和 10 kΩ NTC。 | 电池插座防反接；热敏电阻检测。 |
| PWR-002 | 实现 PowerPath 边充边用：插入 USB 时系统优先由 USB 供电且充电，超出输入限额时电池可补充系统负载。 | BQ24074，首版充电电流上限 800 mA，热设计审查后再调整。 |
| PWR-003 | 3V3_SYS 连续电流设计值不低于 2 A；MCU 内核电源网络严格复用 HPM6360 EVK/数据手册。 | TPS62825A 2 A buck + HPM6360 内部 DCDC 外围。 |
| PWR-004 | 为扩展底板提供可关断、限流的 5V_EXP，目标 1.0 A 连续/1.5 A 限流。 | TPS61088 5 V boost + TPS22919 load switch；不使用时可 DNP boost 功率级。 |
| PWR-005 | 耳机热插拔、Class-D 负载阶跃、OLED 升压和 USB 并发工况不得使 MCU 掉电或复位。 | 电源时序/负载阶跃测试；检查 3V3_SYS 调制和复位线。 |
| PWR-006 | “边充边用”表示不间断运行，不保证在默认 USB 500 mA 来源下、最大扬声器音量与满载扩展同时发生时仍净充电。 | 三种 USB 电流宣告下分别记录电池净电流。 |

## 5. 人机界面

| ID | 需求 | A0 基线 |
|---|---|---|
| UI-001 | 配置 5 个 10 kΩ 线性滑动电位器，有效行程 15 mm。 | Bourns PTA 15 mm 系列；每路 ADC 前置 1 kΩ + 100 nF 低通。 |
| UI-002 | 配置 6 个带按压开关的增量式旋转编码器，直径/本体约 12 mm。 | Bourns PEC11R，24 detent 优先；硬件 RC 预留、固件消抖。 |
| UI-003 | OLED 有效分辨率不低于 128×32，外形约 30 mm × 11.5 mm。 | Winstar WEO012832D，SSD1306，3 V，4-wire SPI。 |
| UI-004 | 滑杆、编码器轴、OLED 可视区与机壳开孔要以 1:1 纸样和 3D 装配确认。 | 先做 placement-only PCB，再冻结电路封装。 |

## 6. 音频

| ID | 需求 | A0 基线 |
|---|---|---|
| AUD-001 | HPM6360 通过一组独立 I2S 连接立体声 codec，不与扩展 TDM 总线共用一个外设实例。 | TLV320AIC3204；48 kHz/24 bit 为默认音频格式。 |
| AUD-002 | 3.5 mm TRS 母座提供立体声耳机输出，不默认支持带麦克风的 TRRS。 | Same Sky `SJ-3523-SMT-TR`；边缘水平贴片。 |
| AUD-003 | 两只被动式 8 Ω 扬声器立体声工作，单只额定不低于 0.7 W。 | 2 × CMS-151125-18SP-X8 或同尺寸 15×11 mm/8 Ω/1 W 器件。 |
| AUD-004 | 扬声器由立体声模拟输入 Class-D 功放驱动，电池 3.6 V/8 Ω 时目标约 0.72 W/ch。 | TPA2012D2 WQFN；双通道 BTL 输出。 |
| AUD-005 | PCB 为每只扬声器同时提供 JST SH 1.0 mm 2-pin 插座与并联焊盘。 | `SM02B-SRSS-TB(LF)(SN)` ×2；焊盘使用 `SPK_L+/-`, `SPK_R+/-`。 |
| AUD-006 | Class-D 四个扬声器输出端均为浮地 BTL，不得与 GND 相连，不得合并左右声道负端。 | 原理图 ERC 网标和丝印检查。 |
| AUD-007 | 耳机/扬声器两路必须能独立 mute，上下电无明显 pop/click。 | Codec soft mute + 功放 shutdown 时序测试。 |

## 7. POGO 扩展与模块级联

| ID | 需求 | A0 基线 |
|---|---|---|
| EXP-001 | 底层提供 24-pin（2×12，2.00 mm pitch）无阻焊覆盖的 POGO 触点区。 | 本地封装 `Pogo_Interface_2x12_P2.00mm_Bottom`。 |
| EXP-002 | 控制/事件平面使用 CAN-FD，音频平面使用同步 TDM，I2C/UART 仅作枚举或维护。 | 详见 `POGO_INTERFACE.md`。 |
| EXP-003 | 底板向外供电必须有限流和关断功能，禁止 USB VBUS、电池和其他模块电源直接反灌。 | 5V_EXP 通过 load switch；断电电阻/方向性测试。 |
| EXP-004 | 控制层可多点级联；多源上行音频不得被动并联多个推挽输出。 | 活动底板/时隙聚合器或双接口菊链二选一后再释放量产接口。 |

## 8. 制造、布局和可测试性

- MFG-001：首版按 6 层、1.6 mm 板厚、外层 1 oz 铜基线；最终阻抗参数由板厂 stack-up 计算。
- MFG-002：USB D+/D- 单独网类；其下方不跨分割，连接器、ESD 和 MCU 按信号流排列。
- MFG-003：数字、充电/boost、Class-D 和 codec/耳机分区布局；不分割地平面，通过电流回路和元件位置控制噪声。
- MFG-004：HPM6360 eLQFP144L 和所有 QFN 的中央焊盘、导热过孔、阻焊桥与锡膏分窗要经 DFM 复核；eLQFP 外围引脚可手焊，但中央裸露焊盘不能仅靠普通烙铁可靠焊接。
- MFG-005：保留 USB D+/D-、VBUS、BAT、VSYS、3V3_SYS、5V_EXP、RESET、I2C、CAN_TX/RX、I2S/TDM 与每个扬声器输出的测试点。
- MFG-006：POGO 触点原型可用 ENIG；如预计对接循环数高或有擦拭运动，评估选择性硬金。

## 9. 依据与参考

- [立创开源平台：HPM6360-测试板](https://oshwhub.com/hasaki6/hpm6360-ce-shi-ban)：官方 EVK 复刻，已用于确认基本电路、USB、SDRAM 和 CAN 的参考方向；它不替代最新数据手册。
- [HPMicro HPM6300 系列数据手册 Rev2.7](https://www.hpmicro.com/Public/Uploads/uploadfile/files/20250205/HPM6300DSV27.pdf)：`HPM6360IPA2` 的 PA 封装为 20×20 mm、0.5 mm pitch 的 eLQFP144；具备 USB HS、I2S、CAN-FD，且 PA 封装引出 108 GPIO 和 16-bit SDRAM 接口。
- [HPM6360EVK 用户手册](https://doc.hpmicro.com/Public/Uploads/uploadfile/files/20241024/HPM6360EVKUGV1.0.pdf)：开发板支持 USB 2.0 OTG，并给出 I2S1 扩展引脚、W9825G6KH SDRAM 和 W25Q128 Flash 等参考配置。

## 10. 待产品决策

1. USB 是否有硬性 Host/OTG 需求；A0 当前按 UFP/Sink 处理。
2. 级联音色模块是“活动底板星形聚合”还是“每模块双口菊链”；这会改变音频层电气规范。
3. 扩展底板的峰值/连续功率、电池容量和期望续航。
4. 两只扬声器的声学腔体、响度、孔位与是否需要耳机插入检测。
5. 外部 SDRAM/PSRAM 容量、音色数据存储量和是否需要 TF 卡。
