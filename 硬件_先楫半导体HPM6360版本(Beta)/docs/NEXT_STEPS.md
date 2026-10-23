# 下一步工作计划

## 当前完成状态

- [x] 创建 KiCad 10 工程、原理图空文档和 PCB 空文档。
- [x] 建立 65 mm × 65 mm 板框和 6 层基线。
- [x] 建立项目本地封装库与 24-pin POGO 露铜触点封装。
- [x] 完成 A0 需求、架构 BOM 和扩展接口提案。
- [ ] 冻结机械排布和具体订货号。
- [ ] 绘制原理图、分配引脚、完成 ERC。
- [ ] 布局布线、信号/电源完整性审核、DRC 和 DFM。

## Phase 0：产品与机械冻结

1. 用 1:1 纸样或废 PCB 验证 5 个 15 mm 滑杆、6 个 12 mm 编码器和 30×11.5 mm OLED 的可操作性。
2. 确认 Type-C、3.5 mm 耳机、电池和两个 JST SH 的机壳出线方向。
3. 给出电池长宽高、容量、保护板、插座极性和 NTC 参数。
4. 完成扬声器腔体初设，确定是否需要耳机插入检测开关。
5. 决策 USB 只做 UFP（推荐 A0）还是需要 DRP/Host；如果需要 Host，增加 VBUS source 电源路径设计。
6. 决策扩展音频是活动底板聚合还是双口菊链；未决定前不冻结量产 POGO 标准。

**退出条件**：输出 DXF/STEP/尺寸图；所有人机件的孔位、高度、操作间隙与可采购订货号已签字。

## Phase 1：功耗和内存预算

1. 分别估算 HPM6360 480 MHz 满载、SDRAM、codec、OLED、USB HS、扬声器与 5V_EXP 的平均/峰值电流。
2. 按 USB Default/1.5 A/3 A 广告分别建立电流预算；BQ24074 最大输入只按 1.5 A 设计。
3. 使用最低电池电压和 1.0 A 5V_EXP 检查 boost 输入电流、电感饱和电池 C-rate。
4. 运行音色引擎内存原型，决定 W9825G6KH 是必须、可 DNP，还是替换为小封装 PSRAM。

**退出条件**：电源树标明每路电压、最大电流、启动顺序、效率和热损耗；电池/充电/扩展功率指标无歧义。

## Phase 2：原理图

按以下分层页绘制，避免单页超大原理图：

1. `00_System` — 顶层端口和电源树。
2. `01_HPM6360` — MCU bank 电源、内部 DCDC 外围、复位、BOOT、时钟、调试。
3. `02_Memory` — QSPI NOR 和 SDRAM/PSRAM。
4. `03_USB_Power` — Type-C、CC、ESD、BQ24074、电池、3V3 buck、5V boost/load switch。
5. `04_Audio` — TLV320AIC3204、3.5 mm TRS、TPA2012D2、两组扬声器输出。
6. `05_Controls_Display` — 5 滑杆、6 编码器、OLED、滤波/消抖。
7. `06_Expansion` — 24-pin POGO、TCAN3413、CAN 终端、TDM 串阻、I2C/UART/复位。

原理图检查点：

- 从最新 HPM6360 Data Manual/CAD 资源导入并双人复核 `HPM6360IPA2` 的 144 个引脚；不手工凭记忆绘制。
- 复用 HPM6360EVK 的 MCU 内核 DCDC、VPMC/VBAT、BOOT 和晶振外围的经验，对每项变更作记录。
- USB0、两组 I2S（本地 codec/扩展 TDM）、CAN、OLED SPI、5 ADC 与 18 编码器 GPIO 完成 Pinmux Tool 冲突检查。
- 所有电源 pin 有就近去耦；每组电源输入有测试点。
- 执行 ERC，对每个有意图的警告使用具体 no-connect/no-ERC 且记录理由。

**退出条件**：ERC 无未解释错误；pinmux 表、电源预算和原理图网名一致。

## Phase 3：封装与布局

1. 逐一用官方 land pattern 复核 HPM6360 eLQFP144L（含中央裸露焊盘）、USB4105、耳机座、滑杆、编码器、OLED FPC、JST SH 和 POGO。
2. 先摆机械件，再摆 MCU/内存，然后放 USB、音频和电源小回路。
3. 与板厂确认 6 层 stack-up、90 Ω USB 差分阻抗、0.5 mm pitch QFP 引脚逃线、阻焊桥和中央焊盘钢网分窗；本方案原则上不需要为 MCU 使用盲埋孔。
4. 布线优先级：MCU 电源/时钟 → USB HS → SDRAM → codec/TDM → 开关电源 → 其他 I/O。
5. 使用完整 GND 参考平面；不用“切地”代替合理分区和回流路径设计。
6. 生成 3D 视图并与机壳/电池/底板装配检查。

**退出条件**：DRC 无未处理错误；USB/SDRAM/TDM 布线审核通过；POGO 压缩区与所有机械件无干涉。

## Phase 4：固件与电路子板先行验证

1. 在 HPM6360EVK 上先验证 USB composite：USB-MIDI + vendor bulk，持续传输 30 min 无枚举丢失。
2. 用 codec EVM/飞线验证 48 kHz/24-bit I2S 和耳机输出；再连接立体声 Class-D 和 8 Ω扬声器。
3. 完成滑杆 ADC 滤波/校准、六编码器中断负载和 OLED 刷新的联合压力测试。
4. 用两节点和故障注入验证 CAN-FD 枚举、节点离线和总线恢复。
5. 构建 TDM loopback/PRBS 测试，覆盖 8-slot 和 16-slot 模式。

## Phase 5：首版样机测试

| 类别 | 必测项 |
|---|---|
| 上电 | USB/电池/边充边用切换，冷启动，低电压，拔插，欠压复位。 |
| 热 | 充电器 800 mA、MCU 满载、boost 1 A、双扬声器同时工作的热像。 |
| USB | HS 枚举、MIDI 时延、bulk 吞吐、ESD 后自恢复。 |
| 音频 | 底噪、THD+N、声道串扰、上下电 pop/click、耳机/扬声器 mute。 |
| UI | ADC 全行程、编码器快旋/同时按下、OLED 最大刷新率。 |
| 扩展 | 5V_EXP 过流/短路/热插拔、CAN-FD 多点、TDM 时钟裕量。 |
| 机械 | Type-C/耳机插拔受力、POGO 压缩、滑杆/编码器手感和底板对位。 |

## 高风险项和缓解

| 风险 | 影响 | 缓解 |
|---|---|---|
| 控件 + OLED 不能在 65 mm 内同时满足手感 | 重做机械/板框 | Phase 0 先做 1:1 排布；优先减少滑杆或改用 9 mm 编码器，而不是压缩轴间距。 |
| HPM6360 eLQFP144L 引脚或中央裸露焊盘定义错误 | 无法启动、散热不良或损坏芯片 | 只使用最新官方 Data Manual/CAD，确认 Figure 26 中适用于 HPM6360 的 D2/E2 裸露焊盘尺寸，并与 EVK/参考工程交叉复核。 |
| 线性充电器热降流 | 充电速度不达标 | 首版从 500–800 mA 开始，加大散热铜；若必须 >1 A 持续充电，改用开关充电 PowerPath。 |
| boost/Class-D 干扰 codec/ADC | 底噪和控件抖动 | 6 层连续地、功率回路最小化、FPWM/频率规划、模拟分区和实测波形。 |
| 被动并联多个 TDM 上行源 | 总线争用/硬件损伤 | 使用活动聚合底板或双口菊链；在架构冻结前不释放量产接口。 |
