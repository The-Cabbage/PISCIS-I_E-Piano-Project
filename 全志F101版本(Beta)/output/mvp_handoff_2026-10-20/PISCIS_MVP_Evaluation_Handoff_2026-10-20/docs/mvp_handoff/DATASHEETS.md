# Datasheet 与重点阅读页码

[交接说明](README.md) · [机器可读来源清单](datasheets/SOURCES.json)

本索引于 2026-10-20 核对。**“PDF 页”从文件第一页起按 1 计数，是阅读器跳转用的页序；“正文页”是页面实际印刷的页码。**页码仅适用于清单中 SHA-256 对应的文件。F101 两份 Draft 文档的部分目录定位存在偏差，下面按实际页面内容与页脚定位，不能简单照抄目录。

用户已补充：**MT6701 和 OLED 不需要收集 Datasheet**，本包不附这两类手册，也不借参考手册替它们确定型号或接口。

## 1. 本地文件、来源与版本

| 对象 | 离线文件 | 已核对版本 / 总页数 | 来源及适用边界 |
| --- | --- | --- | --- |
| F101 芯片 | [F101 Datasheet](datasheets/F101_Datasheet_V0.10_Draft.pdf) | Rev 0.10，2026-03-09，Draft；90 页 | 全志署名原厂文档，[WhyCan 托管原文件](https://whycan.com/files/allwinner/f10x/a/F101_Datasheet_V0.10_Draft_Version.pdf)。涵盖系列型号，不能直接据此认定手中板卡型号及实装配置 |
| F101 寄存器与开发 | [F101 User Manual](datasheets/F101_User_Manual_V0.10_Draft.pdf) | Rev 0.10，2026-01-26，Initial Draft；777 页 | 全志署名原厂文档，[WhyCan 托管原文件](https://whycan.com/files/allwinner/f10x/a/F101_User%20Manual_V0.10_Draft_Version.pdf)。补充 I²S、DMA、CCU、TWI、GPIO 的编程细节，不是本工程已验证 BSP |
| ES8388 / 当前 U1 | [ES8388_DS.pdf](../../hardware/libraries/controls_audio/sources/ES8388_DS.pdf) | Revision 5.0，2018-07；36 页 | 既有本地文件；[立创托管的 Everest 原厂 PDF](https://datasheet.lcsc.com/datasheet/pdf/b5b69a0d4875954a40e8f8a3f42483af.pdf?productCode=C365736)，沿用库资产所绑定版本 |
| ALPS SKRHADE010 / SW1 | [SKRH_catalog.pdf](../../hardware/libraries/controls_audio/sources/SKRH_catalog.pdf) | SKRH Series，Update 2510；2 页 | [ALPS 原厂目录 PDF](https://tech.alpsalpine.com/cms.media/product_catalog_mu_02_skrh_en_34aeb8c2e3.pdf)、[精确型号产品页](https://tech.alpsalpine.com/e/products/detail/SKRHADE010/)。系列图不应与其他 Drawing 编号混用 |
| 首韩 PJ-327A-Y / J5 | [PJ-327A-Y_datasheet.pdf](../../hardware/libraries/private/sources/C17701692/PJ-327A-Y_datasheet.pdf) | 首张图纸日期 2020-06-24、版本 A；合计 5 页 | 首韩原厂图纸/承认书，[既有下载来源](https://atta.szlcsc.com/upload/public/pdf/source/20251231/08D5BD65F2341DA0E899FFDF9FFB0326.pdf)。适用于首韩 C17701692，不把 HX 同系列参考模型视作同厂精确 CAD |

原有三份 PDF 保留在各器件库的权威来源目录，本次没有复制成另一套同名文件。新增的 F101 两份 PDF 放在本页相邻 `datasheets/` 中；所有文件均随交接包离线提供。来源、文件长度、版本及哈希以 [SOURCES.json](datasheets/SOURCES.json) 为准。

## 2. 接手时先看的最短路线

| 工作 | 先读哪些 PDF 页 | 需要带着回答的问题 |
| --- | --- | --- |
| 分配开发板总线 | F101 DS **34～36、48～50** | I²S、TWI、GPIO 是否互斥？实际 I/O 电源域及输入电平是否匹配？ |
| 画 Codec 外围 | ES8388 **5～7、31～35** | 电源/参考脚/地/EPAD 如何处理？耳机输出与线路输出外围有什么不同？封装与负载是否匹配？ |
| 定时钟和初始化 | ES8388 **7～11、13～16、22～28**；F101 UM **387～393** | 两端主从、时钟比、格式与输出路由如何保持一致？ |
| 接五维开关 | SKRH **1～2，尤其第 2 页 Drawing No.4** | 公共端、中央键、方向端与固定脚如何区分？安装面视图如何映射到 PCB？ |
| 接耳机座 | PJ-327A-Y **1、3～5** | 厂商端子编号与真实插头接触关系是否一致？定位、外形、插拔与焊接条件是否核对？ |
| 排查实时音频 | F101 UM **263～269、387～414**，CCU 按下表追查 | DMA/FIFO、数据格式与时钟是否正确？欠载来自哪里？ |

“先读页”是定位顺序，不构成已完成设计或测试的证明。

## 3. F101 Datasheet：电气与复用

文件：[90 页 Draft](datasheets/F101_Datasheet_V0.10_Draft.pdf)。重点章节和实际页脚对应如下。

| PDF 页 | 正文页 | 章节 / 图表 | 在本项目中的用途 |
| --- | --- | --- | --- |
| **15** | 14 | §2 Ordering Information | 核对系列后缀与内存差异，再对照实物及原始开发板设计 |
| **22** | 21 | §3.7.2 I2S/PCM | 确认数字音频接口能力；不以片内模拟 DAC 的说明替代外部 ES8388 链路 |
| **29～32** | 28～31 | §4.1 引脚、电源域及复位状态 | 检查引出 GPIO 所属供电域、默认状态与直接外接风险 |
| **34～35** | 33～34 | §4.2.4，Table 4-16 Port D | I²S 与 TWI/显示/UART 等功能的复用冲突；PD6～PD9 的候选 I²S 组在第 34 页 |
| **35～36** | 34～35 | §4.2.5，Table 4-17 Port E | 评估控制总线、调试与其他 GPIO 的选择；结合开发板实际占用 |
| **39** | 38 | §4.3.8 I2S/PCM 信号说明 | 对照 MCLK、LRCK、BCLK、DOUT/DIN 方向，避免把数据输入输出接反 |
| **44** | 43 | §4.3.17 TWI | 核对 TWI 信号方向与命名 |
| **48～50** | 47～49 | §5.2～5.4；Tables 5-1～5-3 | 分清绝对最大值、推荐工作条件和 GPIO 逻辑阈值/上拉；不得把最大额定值作为正常设计点 |
| **70～71** | 69～70 | §5.10.9 TWI Interface Timing | 需要冻结控制总线速率时核对时序及条件 |
| **71～74** | 70～73 | §5.10.10 I2S/PCM Interface Timing | 核对音频输入/输出时序、外设关系与布局相关条件 |

设计时以正文表格及脚注为准。该文档是系列 Draft，实际板卡的供电连接和软件配置仍需独立核对。

## 4. F101 User Manual：实际开发需要补读的部分

文件：[777 页 Draft](datasheets/F101_User_Manual_V0.10_Draft.pdf)。这份文件属于寄存器参考手册，单独列出，避免误称为第二版 Datasheet。

| PDF 页 | 正文页 | 章节 | 接手用途 |
| --- | --- | --- | --- |
| **28～31** | 27～30 | §4 System Boot / FEL | 了解启动与恢复机制；实际开发板入口、工具和镜像步骤仍待验证 |
| **32～42** | 31～41 | §5.1 CCU 功能及编程说明 | 核对音频时钟来源、时钟使能与复位；相关寄存器按章节继续检索 |
| **263～269** | 262～268 | §12.1 DMAC 总览、请求及描述符 | 理解 I²S 请求、传输描述符和缓冲组织；不是 DMA 驱动已移植的证据 |
| **387～390** | 386～389 | §15.2.1～15.2.4 | I²S 总览、外部信号与数据时序 |
| **391～392** | 390～391 | §15.2.5 | 系统设置、声道与 DMA 设置流程 |
| **392～414** | 391～413 | §15.2.6～15.2.7 | 寄存器列表及控制、格式、FIFO/中断、时钟分频与通道映射定义 |
| **485～494** | 484～493 | §16.4.1～16.4.5 | 控制总线的传输和初始化；按实际采用的驱动模式继续读寄存器部分 |
| **539～548** | 538～547 | §16.7 GPIO 功能及寄存器列表 | pinmux、电气配置、输入和中断；具体端口寄存器继续按本章定位 |

注意目录中 §15.2 等章节给出的页号与实际正文不一致。例如 §15.2 起始页实际是 **PDF 第 387 页、正文第 386 页**；上表来自正文核对。

## 5. ES8388：硬件和驱动共同必读

文件：[Revision 5.0](../../hardware/libraries/controls_audio/sources/ES8388_DS.pdf)。本文件 PDF 页与正文页相同。

| PDF / 正文页 | 内容 | 本轮重点 |
| --- | --- | --- |
| **4** | Block Diagram | 理解输出路由与混音关系，明确本轮播放路径 |
| **5～6** | 28-pin QFN 及 Pin Descriptions | 核对电源/地、参考输出、音频/控制口、NC 和底部焊盘；与当前符号缓存逐脚核对 |
| **7** | Typical Application Circuit；Clock Modes | 去耦、参考、耳机输出耦合与布局注记；区分耳机/线路输出条件；核对从模式时钟比 |
| **8～9** | 时钟比续表；控制接口开始 | 配置主从模式前核对允许组合，不能只按一个常见 MCLK 值推算 |
| **9～11** | Micro-controller Configuration Interface | 控制模式、地址、读写时序和两线接口表格 |
| **11～13** | Register Summary | 作为寄存器入口，不直接把默认值拼成初始化序列 |
| **13～16** | 芯片、电源、低功耗与主模式控制 | 启动/停止、输出使能和时钟配置；与实际硬件时序共同核对 |
| **22～24** | DAC 接口格式、采样率、静音及音量 | I²S 格式与位宽、采样率配置、数字音量和静音控制 |
| **26～28** | 输出混音、控制及输出音量 | 选择实际 L/R 输出对，避免路由错误或输出级仍关闭 |
| **29～30** | Digital Audio Interface | 双声道对齐、时钟与有效位的时序图 |
| **30～34** | 电气与时序规格 | 供电、负载/测试条件、音频性能及音频/控制口时序；芯片典型性能不是本板实测结果 |
| **35** | Package Information | 1 脚方向、外形和裸露金属尺寸；与本地两个 EP 版本的铜焊盘定义分开看 |

本地库的封装差异与参考脚类型说明见 [controls_audio README](../../hardware/libraries/controls_audio/README.md)。当前实例绑定差异见[交接说明](README.md)，不能假定换库已经完成。

## 6. ALPS SKRHADE010：第 2 页必须看准图号

文件：[SKRH 系列目录](../../hardware/libraries/controls_audio/sources/SKRH_catalog.pdf)。页面未单独印刷连续页码，本节使用 PDF 页序。

- **PDF 第 1 页：**规格和型号表；先定位精确型号 SKRHADE010 与其 Drawing No.4，不把相近高度/型号混用。
- **PDF 第 2 页，底部 Drawing No.4：**尺寸、安装焊盘及 Circuit Diagram；重点核对从安装面看的方向、公共端、中心键、A/B/C/D、定位柱/孔与视图关系。中上部 Drawing No.2 / No.3 不是本型号的图。

本地库对定位孔做过 NPTH 调整，原因及尺寸依据已在[原库说明](../../hardware/libraries/controls_audio/README.md)维护；实物配合和按装配朝向映射屏幕方向仍待验证。

## 7. 首韩 PJ-327A-Y：以首张工程图为连接依据

文件：[5 页图纸与承认书](../../hardware/libraries/private/sources/C17701692/PJ-327A-Y_datasheet.pdf)。PDF 第 1 页图框自身标注为 1/1，后续承认书另有页码体系，因此统一按 PDF 页序跳转。

- **PDF 第 1 页：优先阅读。**外形、定位柱、五焊盘位置、端子编号、开关关系、安装面视图与版本。图纸未直接列出 T/R/S 命名，应结合实物接触关系核对，不据参考 3D 外观猜端子。
- **PDF 第 3 页：**使用条件、额定与电气规格。
- **PDF 第 4 页：**耐压及机械插拔/耐久等条件。
- **PDF 第 5 页：**环境、焊接等试验/条件，供采购装配与验证时追查。它们是厂家规定，不能转写成本工程已经完成的试验。

J5 的参考模型来源与尺寸差异见 [private README](../../hardware/libraries/private/README.md)，不得用 HX 同名模型取代首韩工程图。

## 8. 外部延伸入口

[YuzukiNeko 维护者仓库](https://github.com/YuzukiHD/YuzukiNeko)可用于追查开发板版本、硬件与软件入口；本包母座仍以所附 `Yuziki_1` 源文件为基线，访问上游不是自动升级许可。全志旧在线文档入口已迁移，可从[新的官方文档平台](https://developers.allwinnertech.com/docs)继续查资料；本次离线手册来自上表明确标注的镜像，未把镜像宣称为原厂最新正式发布。
