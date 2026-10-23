# 系统架构与工程索引

[项目入口](../README.md) · [产品定义](../运营与开发策略/PISCIS_1.0_PRD_2026-10-20.md) · [职责划分依据 D-002](../DECISIONS.md#d-002) · [键盘集成专题](topics/keyboard-chain.md)

本页维护整机职责、接口关系与源资料位置，不复制局部规格。产品侧架构取自 PRD 第 9 节，属于 Decided；文件中存在某器件或接口，不表示它已成为首代产品的冻结方案。

## 系统组成与责任

| 模块 | 产品定义中的责任 | 接口与依赖 |
| --- | --- | --- |
| 键盘机械与霍尔键盘 PCB | 握持与键程、磁铁运动；采集、校准、触发和力度估算 | 机械间隙/磁场影响模拟信号；键盘向主板发送演奏事件；校准需独立保护。 |
| 主控与音频 PCB / 官方固件 | 音源、音频处理、显示、系统状态、存储及外部连接 | 接收键盘事件，连接音频与实体控件，管理卡片及恢复。最终主控和资源预算 Open。 |
| 屏幕、转轮与五维摇杆 | 参数选择、相对增量调节与状态反馈 | 与固件参数、显示和声音响应一致；具体电气连接以选定工程为准。 |
| EP-1 / EO-1 乐器卡片 | 音源、参数、界面及美术作为整体 | 依赖主控性能、资源与加载机制；封装和运行接口 Open。 |
| 电脑端管理工具 | 卡片管理、官方更新及救援引导 | 与设备 USB 通信；应用形态、协议和平台支持 Open。 |
| 结构、供电与音频输出 | 支撑握持、可检修和独立演奏 | 连接器位置、噪声、负载与装配共同约束；电池、扬声器及电源拆板方案 Open。 |

以上是模块职责索引，产品范围和详细要求只在 PRD 维护。机械、电子、固件、软件中的任一改动若影响此链路，先检查直接相邻模块。

## 接口的权威位置与缺口

| 边界 | 现有定义 / 入口 | 适用范围与未决项 |
| --- | --- | --- |
| 机械→传感器 / PCB | [弹簧选型](../弹簧选型.md)、[键盘 STL](../Keyboard_2.stl)、[整机 STL](../PISCIS_Edition_5.stl) | 已有资料；对应样机、磁铁、装配公差及模型关系 Open，不从文件名推定冻结版本。 |
| 键盘→主板 | [Pogo 分支说明](../Keyboard_Scanning_ScaleUp_PogoPin/README.md)与[J2 修复](../Keyboard_Scanning_ScaleUp_PogoPin/output/j2-fix-2026-10-20/J2_Fix_CN.md) | 仅定义该分支连接；整机事件协议尚未定位。电气针序、USB 通道与应用事件协议是不同层，不能互相替代。 |
| Yuzuki 开发板→底板 | [母座接口、方向与针脚定义](../主机MVP评估板/全志F101版本(Beta)/hardware/yuzuki_mvp/README.md) | 仅母座阶段；未完成底板外围、板框与电源隔离，不代表主控定案。 |
| HPM Rev.A 主板→控制/音频等 | [Rev.A INTERFACES](../主机MVP评估板/硬件_先楫半导体HPM6360版本(Beta)/hardware/rev_a/INTERFACES.md)、[pinmux](../主机MVP评估板/硬件_先楫半导体HPM6360版本(Beta)/hardware/rev_a/pinmux.csv) | 仅用于该评估板；不可直接套用到首代掌上产品。 |
| 固件→音频/显示/存储/更新；电脑→设备 | [PRD 第 8–11 节](../运营与开发策略/PISCIS_1.0_PRD_2026-10-20.md) | 目前定位到职责要求，尚未定位整机正式接口实现及规范；确定后只在所属工程维护一份定义，由本表链接。 |

## 工程源资料与版本边界

当前整机工程组合 **Open，待补充**；本表登记已发现资料，不指定谁是“最新版”。既有源文件、局部接口和报告继续在原位置维护。

| 资料 / 分支 | 源入口 | 当前可确认的资料范围 |
| --- | --- | --- |
| 主机 MVP 评估板 | [独立模块入口](../主机MVP评估板/README.md) | 2026-10-23 迁入全志 F101 Beta 与 HPM6360 Beta；迁移前历史已固化归档，各工程版本边界沿用原说明。已登记为独立子模块。 |
| 主机正式 PCBA | [独立模块入口](../主机正式PCBA/README.md) | 2026-10-23 初始化，尚未迁入工程；产品适用版本、主控、接口与制造资料待补充。已登记为子模块，初始化提交已上传至私有远程。 |
| 键盘相关并存工程 | [Keyboard](../Keyboard/README.md)、[Scanning](../Keyboard_Scanning/README.md)、[ScaleUp](../Keyboard_Scanning_ScaleUp/README.md)、[PogoPin](../Keyboard_Scanning_ScaleUp_PogoPin/README.md) | 各有 KiCad 原理图、PCB 及私有库；选用版本、替代关系和实物验证状态未统一确认。库中可选器件不等同于板上实装器件。 |
| Pogo 键盘检查 | [2026-10-19 审阅](../Keyboard_Scanning_ScaleUp_PogoPin/output/review-2026-10-19/PCB_Review_CN.md)、[2026-10-20 J2 修复](../Keyboard_Scanning_ScaleUp_PogoPin/output/j2-fix-2026-10-20/J2_Fix_CN.md)、[U37 物料字段修正](../Keyboard_Scanning_ScaleUp_PogoPin/output/u37-bom-fix-2026-10-20/validation.json) | 后两项覆盖部分旧问题；旧审阅仍可作问题来源，不能把已修项继续当成现状，也不能据此宣布整板已通过。 |
| 全志 Beta / Yuzuki 母座 | [分支说明](../主机MVP评估板/全志F101版本(Beta)/README.md)、[KiCad 工程](<../主机MVP评估板/全志F101版本(Beta)/PISCIS-I_F101.kicad_pro>)、[既有检查输出](<../主机MVP评估板/全志F101版本(Beta)/hardware/yuzuki_mvp/reports/validation.json>) | 母座孔位/网络已有文件级检查；报告仍有板框错误。F101/F133 在产品定义稿中仍为候选，不能由文件夹命名认定最终主控。 |
| HPM6360 Beta | [分支说明](../主机MVP评估板/硬件_先楫半导体HPM6360版本(Beta)/README.md)、[Rev.A 验证边界](../主机MVP评估板/硬件_先楫半导体HPM6360版本(Beta)/hardware/rev_a/VALIDATION.md) | A0 与 Rev.A 并存。Rev.A 是未布线评估板，A0 需求、POGO 提案及旧任务清单不能当成 Rev.A 或整机规格。 |
| LED 点阵探索 | [工程说明](../LED_Matrix_Display/README.md) | 独立隐藏点阵评估资料；说明中尚无电路/布局，不替代 PRD 中屏幕要求。 |
| C++ / MIDI 探索 | [工程配置](../源代码/CMakeLists.txt)、[事件定义](../源代码/src/midievents.hpp)、[解析代码](../源代码/src/midi_parser.hpp) | 已有代码；未验证构建及目标板运行，也未认定为正式键盘协议。 |
| 浏览器音色与交互原型 | [原型入口](../音色模拟/index.html)、[源码](../音色模拟/src/main.jsx)、[运行配置](../音色模拟/package.json) | React / Three.js / Web Audio 原型；不代表目标硬件的音质、延迟、复音或卡片加载性能。 |
| 外观与结构参考 | [外观参考目录](../外观设计参考/)、上述 STL、[Pogo 分支 DXF](../Keyboard_Scanning_ScaleUp_PogoPin/PCB_Edition2.dxf) | 用于查找与对照，尚未建立统一装配基线；STL 不自动视为可编辑 CAD 或制造图。 |

## BOM 与源文件约定

- [Rev.A BOM CSV](../主机MVP评估板/硬件_先楫半导体HPM6360版本(Beta)/hardware/rev_a/BOM_RevA.csv)是[现有导出脚本](../主机MVP评估板/硬件_先楫半导体HPM6360版本(Beta)/tools/export_reva_bom.mjs)读取 `hardware/rev_a/*.sch` 的物料字段生成的派生文件；物料变更应回到对应版本的原理图/生成来源，再导出，不同时手工维护 CSV 和另一张总表。使用前核对源版本及生成链，不为整理文档重新生成工程。
- [A0 BOM 工作簿](../主机MVP评估板/硬件_先楫半导体HPM6360版本(Beta)/outputs/01a0a15e-6745-7f52-aa7f-aea0cbdf02fa/PISCIS-I_HPM6360_BOM_RevA0.xlsx)保留其 A0 范围；不是与 Rev.A CSV 人工同步的整机总 BOM。当前未指定整机量产 BOM。
- PCB、原理图、库、CAD 和代码保持原工程路径；评审输出只证明其记录的源版本。不要为整理文档运行会覆盖源文件的生成器。

新架构选择先写清适用范围、理由及验证需求，再记录到[决策页](../DECISIONS.md)。已有分支规格与产品目标出现差异时，保留两者的版本语境，并把需要收敛的问题加入[任务页](../BACKLOG.md)，不自动修改技术方案。
