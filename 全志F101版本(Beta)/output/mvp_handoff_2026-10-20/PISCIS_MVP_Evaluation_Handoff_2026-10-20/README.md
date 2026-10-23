# PISCIS-I 全志 F101 版本（Beta）

KiCad 10 项目入口：`PISCIS-I_F101.kicad_pro`。

## MVP 评估板交接入口（2026-10-20）

本轮以 YuzukiNeko F101、I²S ES8388 和 3.5mm 耳机输出，配合 4Pin OLED、五维摇杆和 MT6701 转盘，评估声音生成、DSP、相对参数调节和单层菜单反馈；暂不集成霍尔键盘、电池管理、USB Host 扩展和完整卡片系统。

给协作者 Codex 的详细说明、当前证据边界和开工提示见[交接说明](docs/mvp_handoff/README.md)，器件手册及重点页码见[Datasheet 索引](docs/mvp_handoff/DATASHEETS.md)，范围来源见[局部决策](DECISIONS.md)。

**当前是未完成的电路草稿：**已放置 J1、U1、SW1、J2～J5 共 7 个器件，外围尚未连线，PCB 无走线。旧母座阶段检查不代表当前整板通过；本次检查与文件哈希见[审计记录](docs/mvp_handoff/reports/file_audit_2026-10-20.json)。

项目本地库通过根目录的 `sym-lib-table` 和 `fp-lib-table` 注册：

- 符号库：`PISCIS_Private`
- 封装库：`PISCIS_Private`
- 文件位置：`hardware/libraries/private/`

私有库由 HPM6360 Beta 工程在 2026-10-04 复制，两个工程此后彼此独立，修改不会自动同步。

## Yuzuki MVP 插接接口（2026-10-20）

已依据 `Yuziki_1/` 中开发板的实际孔位和 40 针网络，在本项目原理图及 PCB 中加入 J1 双母座整体接口；新增项目本地库 `PISCIS_Yuzuki_MVP`。

插接方向、针脚对照、孔位尺寸及历史检查结果见 [母座设计说明](hardware/yuzuki_mvp/README.md)。母座阶段完成后，当前文件已增加外围器件放置和一个板框矩形；这些变化尚未形成完整电路或冻结机械设计。

## 五维开关与 ES8388 库资源

项目本地库 `PISCIS_Controls_Audio` 收录 C202415 / ALPS SKRHADE010 与 ES8388 的符号、封装和本地 STEP/WRL 模型。搜索名称、来源、封装版本差异及文件检查见 [器件库说明](hardware/libraries/controls_audio/README.md)。当前 SW1 使用该本地库，U1 则使用标准 `Audio:ES8388`，尚未改成本地符号；接手时需核对其参考脚 ERC 类型和模型依赖。
