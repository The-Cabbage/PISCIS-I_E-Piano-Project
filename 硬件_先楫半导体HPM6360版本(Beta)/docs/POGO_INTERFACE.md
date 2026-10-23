# PISCIS Modular Bus 24-Pin POGO 接口 v0.1

- 状态：提案，用于原理图和底板架构评审
- 信号方向基准：本 HPM6360 控制板为 Host
- 机械定义：2 列 × 12 行，2.00 mm pitch，模块底层露铜触点

## 1. 设计目标

1. 用一个紧凑接口完成扩展模块供电、枚举、MIDI/参数事件和多通道数字音频传输。
2. 控制平面可多点扩展；对不同模块的上下电顺序和热插拔有可定义行为。
3. 支持活动底板星形级联；并为未来双接口 IN/OUT 菊链保留相同逻辑信号集。

## 2. 电气层次

- **Power plane**：5V_EXP 主电源 + 3V3_AUX 枚举/小信号辅助电源。
- **Control plane**：CAN-FD 承载参数、MIDI 事件、模块描述和状态；I2C/UART 只用于短距离维护/出厂。
- **Audio plane**：Host 作时钟主机的 48 kHz/24-bit TDM，默认 8 时隙，预留 16 时隙能力。

CAN-FD 适合多点控制。TDM 的多个推挽上行数据输出不能被动并联：多个音源模块同时上行时，底板必须实现时隙聚合/隔离，或者改为每个模块两套 IN/OUT 接口的有向菊链。

## 3. 24-Pin 定义

视图约定：从模块底部铜面正视，Pin 1 在左上角；左列为奇数，右列为偶数，从上到下递增。`Direction` 均相对于 Host。

| Pin | Net | Direction | 类型 | 定义 |
|---:|---|---|---|---|
| 1 | GND | Power | Ground | 接口地。 |
| 2 | GND | Power | Ground | 接口地。 |
| 3 | 5V_EXP | Output | Power | 扩展主电源触点 A。 |
| 4 | 5V_EXP | Output | Power | 扩展主电源触点 B。 |
| 5 | 3V3_AUX | Output | Power | 枚举/ID 辅助电源，总负载上限 100 mA。 |
| 6 | PWR_EN | Output | 3.3 V CMOS | 扩展模块逻辑上电允许；默认下拉。 |
| 7 | CAN_H | Bidirectional | CAN-FD | CAN 差分高线。 |
| 8 | CAN_L | Bidirectional | CAN-FD | CAN 差分低线。 |
| 9 | GND | Power | Ground | CAN/ID 回流。 |
| 10 | MOD_ID_ADC | Input | Analog 0–3.3 V | 模块电阻 ID；底板/模块不得外加超出电源轨的电压。 |
| 11 | TDM_BCLK | Output | 3.3 V clock | TDM 位时钟；源端预留 22–33 Ω 串联终端。 |
| 12 | GND | Power | Ground | TDM_BCLK 回流。 |
| 13 | TDM_FS | Output | 3.3 V clock | 48 kHz 帧同步。 |
| 14 | GND | Power | Ground | TDM_FS 回流。 |
| 15 | TDM_MCLK | Output | 3.3 V clock | 音频主时钟，初定 12.288 MHz；源端串阻。 |
| 16 | GND | Power | Ground | TDM_MCLK 回流。 |
| 17 | TDM_TX_HOST | Output | 3.3 V data | Host 到模块的下行 TDM 数据。 |
| 18 | TDM_RX_HOST | Input | 3.3 V data | 模块/活动底板到 Host 的上行 TDM 数据。 |
| 19 | I2C_SCL_SVC | Bidirectional | Open-drain | 400 kHz 以下维护/枚举时钟，Host 侧上拉。 |
| 20 | I2C_SDA_SVC | Bidirectional | Open-drain | 400 kHz 以下维护/枚举数据，Host 侧上拉。 |
| 21 | UART_TX_HOST | Output | 3.3 V UART | 维护串口 Host TX，默认 1 Mbaud 以下。 |
| 22 | UART_RX_HOST | Input | 3.3 V UART | 维护串口 Host RX。 |
| 23 | MOD_IRQ_N | Input | Open-drain | 模块中断/枚举请求，Host 侧上拉。 |
| 24 | MOD_RESET_N | Output | 3.3 V CMOS | 模块复位，低有效；默认保持低电平直至电源稳定。 |

## 4. 功率和保护约束

- 5V_EXP 两个触点合计目标 1.0 A 连续，由 1.5 A 自保护 load switch 限流。这是系统限值，不得仅根据 POGO 触点的理论额定设计。
- Host 是 5V_EXP 唯一默认电源源。扩展模块不得反馈 5V_EXP/3V3_AUX；如扩展有自身电源，必须用理想二极管/负载开关隔离。
- PWR_EN 拉高前，底板只可从 3V3_AUX 取小电流用于 ID/枚举；大负载电源通过模块本地 load switch 启动。
- 电源与 CAN 端口要求 ESD/TVS、反接/反灌和短路测试。

## 5. 总线参数初值

### CAN-FD

- 仲裁段 1 Mbit/s，数据段 5 Mbit/s；在线长度和 EMC 测试允许前不默认使用 8 Mbit/s。
- 120 Ω 终端只在物理总线两端启用；本板预留可控/跳线终端，默认 DNP。
- 枚举时每个模块用唯一硬件 ID + 临时节点号；MIDI 事件、参数更新和设备描述用不同 CAN ID 范围。

### TDM audio

- 48 kHz 帧率，24-bit 采样，默认 8 slots 时 BCLK = 12.288 MHz。
- 保留 16 slots，对应 BCLK = 24.576 MHz；需在底板最长路径上通过时序/SI 仿真后启用。
- Host 统一输出 MCLK/BCLK/FS，模块不得反向驱动三条时钟线。
- 活动底板负责将多模块上行音频映射到不同时隙；在未验证驱动器高阻行为前，禁止多个 TDM_TX 直连。

## 6. 机械与 PCB 规则

- 本工程已创建 `PISCIS_Local:Pogo_Interface_2x12_P2.00mm_Bottom`。触点为 1.40 mm 直径的 B.Cu + B.Mask 开窗，不生成锡膏层。
- 触点周围至少保留 1.0 mm 禁布/机械压缩区；禁止在触点内打过孔。
- 底板上的弹针选型必须覆盖公差后的压缩行程，建议标称工作点在总行程 40–60%。
- 需另设两个机械定位柱/孔，不允许仅依靠 24 根弹针承担 XY 定位。
- 若用户可反复拆装，量产板评估选择性硬金；否则首版 ENIG 可用于功能样机。

## 7. 上电与枚举顺序

1. Host 关断 5V_EXP，MOD_RESET_N 和 PWR_EN 保持低。
2. Host 打开 3V3_AUX，读取 MOD_ID_ADC，检查 CAN 总线静默。
3. Host 打开 5V_EXP 负载开关，检查过流/电压。
4. Host 拉高 PWR_EN，延时后释放 MOD_RESET_N。
5. 模块通过 CAN-FD 上报类型、版本、功耗、音频能力和唯一 ID。
6. Host/活动底板分配 TDM slots，在所有模块确认后开启音频数据。

## 8. 量产冻结前必须解决

- 选定活动底板还是双口菊链，并用两个真实音源模块验证上行音频无冲突。
- 确定 POGO 弹针具体品牌、头型、行程、单针电流和寿命，再冻结焊盘直径/节距。
- 完成 5V_EXP 负载突变、热插拔、短路、反馈、ESD 和 CAN 终端错误测试。
- 确认只有一个节点向 I2C_SCL/SDA 提供上拉，总线电容在 400 kHz 指标内。
