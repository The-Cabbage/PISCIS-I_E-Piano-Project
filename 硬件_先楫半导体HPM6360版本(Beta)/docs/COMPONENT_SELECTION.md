# Rev A0 元器件选型建议

- 状态：架构 BOM，不可直接用于采购
- 原则：优先小封装、可手工返修的主流封装、官方数据完整性和 HPM6360 EVK 经验
- 量产前门禁：样品可得性、封装 1:1 核对、器件生命周期、替代料和 JLC/LCSC 实时库存均需重新确认

## 1. 推荐架构 BOM

| 功能 | 首选器件 | 数量 | 封装/尺寸要点 | 选型理由与备注 |
|---|---|---:|---|---|
| MCU | HPMicro `HPM6360IPA2` | 1 | eLQFP144L，20×20 mm，0.5 mm pitch | 默认改为可拖焊/返修的引脚封装；有 USB HS PHY、I2S/TDM、CAN-FD、108 GPIO 和 16-bit SDRAM 接口。外围脚可手焊，底部裸露焊盘仍需热台/回流或热风。 |
| 启动 Flash | Winbond `W25Q128JVPIQ` 系列 | 1 | WSON-8, 6×5 mm | 128 Mbit QSPI NOR；开发板使用 W25Q128 家族。最终后缀按工温/供应选定。 |
| 运行内存 | Winbond `W9825G6KH-6I` | 1 | 54-pin TSOP-II | 256 Mbit/16-bit SDRAM，官方 EVK 同型参考。如算法证明内置 SRAM 足够可 DNP；如换小封装 PSRAM，先完成 XPI 时序验证。 |
| USB-C 母座 | GCT `USB4105-GF-A` | 1 | 16-contact, USB 2.0 Type-C | 紧凑、20,000 次插拔标称，含 CC1/CC2 且满足 USB 2.0 线路。 |
| Type-C CC 检测 | TI `TUSB320LAI` | 1 | X2QFN-12, 1.6×1.6 mm | 检测插入、方向、UFP/DFP 角色及 Default/1.5 A/3 A 电流广告。A0 配 UFP。 |
| USB ESD | TI `TPD2EUSB30` | 1 | SOT-9X3, 1×1 mm | 2 通道、0.7 pF 典型电容，放置在接口与主控之间且靠近接口。 |
| 充电 + PowerPath | TI `BQ24074RGTR` | 1 | VQFN-16, 3×3 mm | 1S 锂电、最高 1.5 A，具有 DPPM/PowerPath，系统可边充边用。初版设定 800 mA 上限以降低热风险。 |
| 3.3 V 主电源 | TI `TPS62825A` | 1 | VSON-HR-6, 1.5×1.5 mm | 2.4–5.5 V 输入、2 A 输出；为 MCU I/O、codec I/O、OLED 和逻辑供电。 |
| 5 V 扩展 boost | TI `TPS61088RHLR` | 1 | VQFN-20, 4.5×3.5 mm | 单节锂电升压到 5V_EXP；模块功率需求未冻结前保留 DNP 方案。 |
| 扩展负载开关 | TI `TPS22919DCKR` | 1 | SC-70 | 1.5 A 自保护负载开关，控制 5V_EXP 涌入和短路。 |
| 电量计（选配） | TI `BQ27441-G1` | 1 | WSON-12 | 如 UI 需显示 SOC/剩余时间则实装；否则只用 MCU ADC 测 BAT 电压。 |
| 立体声 codec | TI `TLV320AIC3204IRHBR` | 1 | VQFN-32, 5×5 mm | 2 ADC/2 DAC、I2S/TDM、立体声耳放和立体声 line out，给扬声器功放与耳机分别供源。 |
| 立体声扬声器功放 | TI `TPA2012D2RTJR` | 1 | WQFN-20, 4×4 mm | 模拟差分输入、双路 filter-free Class-D；3.6 V/8 Ω 典型可达约 720 mW/ch。 |
| 3.5 mm 耳机母座 | Same Sky `SJ-3523-SMT-TR` | 1 | 水平贴片 TRS | 3 导体立体声、1 A 标称，无内部检测开关。如要求插入自动关扬声器，改选带开关型号。 |
| 扬声器 | Same Sky `CMS-151125-18SP-X8` | 2 | 15×11 mm, 8 Ω, 1 W | 小型被动扬声器，需机壳腔体；用短线连接 PCB。 |
| 扬声器插座 | JST `SM02B-SRSS-TB(LF)(SN)` | 2 | SH, 1.0 mm pitch, 水平 | 每路 1 A；与 `SHR-02V-S` 线端壳对接。每个插座旁加 2.0 mm 间距焊盘。 |
| 滑动电位器 | Bourns `PTA1543-2010DPB103`* | 5 | 15 mm travel, 10 kΩ linear | 紧凑、专用于混音/合成器类应用。`*` 订货号需依旋钮高度、滑杆形状和供应商再冻结。 |
| 旋转编码器 | Bourns `PEC11R-4215F-S0024` | 6 | 12 mm, 24 detents, push switch | 成熟增量式器件，带按压；占用 18 路数字输入。 |
| OLED | Winstar `WEO012832D` | 1 | COG/FPC, 30×11.5×1.26 mm | 128×32、SSD1306、3 V、4-wire SPI；最终 FPC 版本与插座须成对冻结。 |
| 扩展 CAN-FD | TI `TCAN3413DDFR` | 1 | SOT-23-THN-8 | 3.3 V 供电、最高 8 Mb/s、支持热插拔与 standby，符合多模块控制总线需求。 |
| 调试口 | Tag-Connect `TC2030-IDC` 触点样式 | 1 | 6-pin no-header footprint | 节省面积；配合 RESET/BOOT 测试点。HPM 实际 JTAG 信号数需要在原理图阶段确认，必要时换 10-pin。 |

## 2. 器件分区建议

### 正面机械件

- 左侧约 32 mm 宽：5 个水平布置的 15 mm 行程滑杆，竖向堆叠。
- 右上：30×11.5 mm OLED 可视/玻璃区。
- 右下：2 列×3 行旋转编码器，中心间距初值 14–15 mm。
- 下边/侧边：Type-C、3.5 mm 耳机和电池/扬声器插座；最终位置由机壳开孔决定。

### 背面电路

- 中央：HPM6360 eLQFP144L + 去耦 + QSPI Flash；SDRAM 放置在相邻空间。主控本体约占 22×22 mm 引脚外廓，布置余量明显小于原 BGA 方案。
- USB 角落：Type-C、CC 控制、ESD 和 HPM6360 USB 引脚按最短通道排列。
- 模拟角落：codec、耳机座、Class-D 功放与扬声器插座，远离 boost SW 节点和 SDRAM/USB 边沿。
- 电源角落：BQ24074、TPS62825A、TPS61088，高 di/dt 回路面积最小化。
- 底面中部：24-pin POGO 触点；与金属支架/电池保留压缩行程和禁布区。

## 3. 关键选型结论

1. **USB 性能边界**：HPM6360 的“high-speed USB”对应 USB 2.0 HS 480 Mb/s，16-contact Type-C 座已足够；更换为 24-pin USB 3.x 座并不会使主控获得 5/10 Gb/s。
2. **音频链路**：TLV320AIC3204 直接驱动耳机，line out 送入 TPA2012D2 驱动两只 BTL 扬声器，避免用 MCU 内置 12-bit DAC 承担主音频输出。
3. **边充边用**：BQ24074 具有动态电源路径。它是线性充电器，大电流时的结温而不是芯片标称 1.5 A 决定实际可用充电电流。
4. **控件密度**：5 个 15 mm 滑杆 + 6 个 12 mm 编码器在 65 mm 方板上是“可尝试但机械高风险”，必须在画全套原理图前做 1:1 操作面排布。
5. **扬声器**：两只 15×11 mm 扬声器不应直接占用主 PCB 操作面；声压和低频性能主要受机壳腔体影响。
6. **主控焊接边界**：eLQFP144L 的 0.5 mm pitch 外围引脚可使用助焊剂和拖焊法返修，但封装中央裸露焊盘不能仅靠烙铁从板边焊接；首版仍建议钢网、热台或回流工艺。

## 4. 官方资料

- [HPMicro HPM6300 系列](https://www.hpmicro.com/en/product-center/microcontroller/hpm6300)
- [HPM6300 数据手册 Rev2.7（eLQFP144L 尺寸与订购规则）](https://www.hpmicro.com/Public/Uploads/uploadfile/files/20250205/HPM6300DSV27.pdf)
- [HPM6360EVK 用户手册](https://doc.hpmicro.com/Public/Uploads/uploadfile/files/20241024/HPM6360EVKUGV1.0.pdf)
- [BQ24074 PowerPath 充电器](https://www.ti.com/product/BQ24074)
- [TPS62825A 2 A Buck](https://www.ti.com/product/TPS62825A)
- [TPS61088 Boost](https://www.ti.com/product/TPS61088)
- [TUSB320LAI Type-C CC 控制器](https://www.ti.com/product/TUSB320LAI)
- [TPD2EUSB30 USB ESD](https://www.ti.com/product/TPD2EUSB30)
- [TLV320AIC3204 立体声 Codec](https://www.ti.com/product/TLV320AIC3204)
- [TPA2012D2 立体声 Class-D](https://www.ti.com/product/TPA2012D2)
- [TCAN3413 CAN-FD 收发器](https://www.ti.com/product/TCAN3413)
- [GCT USB4105 Type-C 接口](https://gct.co/connector/usb4105)
- [Same Sky SJ-3523-SMT-TR 3.5 mm 耳机座](https://www.sameskydevices.com/product/interconnect/connectors/audio-connectors/jacks/sj-3523-smt-tr)
- [Bourns PTA 滑动电位器](https://www.bourns.com/docs/Product-Datasheets/pta.pdf)
- [Bourns PEC11R 旋转编码器](https://www.bourns.com/docs/Product-Datasheets/PEC11R.pdf)
- [Winstar WEO012832D 0.91-inch OLED](https://www.winstar.com.tw/products/oled-module/graphic-oled-display/128x32-oled.html)
- [Same Sky CMS-151125-18SP-X8 扬声器](https://www.sameskydevices.com/product/audio/speakers/miniature-%2810-mm~40-mm%29/cms-151125-18sp-x8)
- [JST SH 1.0 mm 插座](https://www.jst.com/products/crimp-style-connectors-wire-to-board-type/sh-connector)
