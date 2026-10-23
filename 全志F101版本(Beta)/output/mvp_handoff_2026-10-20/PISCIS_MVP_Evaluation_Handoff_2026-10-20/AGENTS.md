# F101 MVP 接手约定

先读 [README.md](README.md)、[交接说明](docs/mvp_handoff/README.md)、[系统架构原文快照](docs/mvp_handoff/context/engineering__architecture.md.txt)及直接相关的接口、库 README 和验证记录；涉及产品边界时核对[PRD 原文快照](docs/mvp_handoff/context/运营与开发策略__PISCIS_1.0_PRD_2026-10-20.md.txt)。上层[协作约定原文](docs/mvp_handoff/context/AGENTS.md.txt)同样适用。

- 先检查当前 Git 状态与目标文件，保留用户未提交修改；解压包不是 Git 仓库历史，不对整个上层工作区自动暂存、提交、合并或重建。
- 本轮用户确定的技术方案与边界以交接说明第 2 节为准，不擅自替换主控/Codec，不添加已排除模块。MT6701/OLED 不收集 Datasheet，不代表取消功能。
- 修改工程前核对所属版本及直接使用方。针脚、电气、机械和时序定义各只维护一份权威来源，其他文档链接引用。
- 当前是 7 器件的未完成电路；母座历史 PASS 不能用于当前整板。Confirmed 必须写清版本、条件和实测证据；文件检查、推测与计划分别标明。
- 未知参数写“待补充”，不编造需求、元件值、排期、测试阈值或完成状态。
- 不运行 `scripts/build_yuzuki_socket.py`，它会覆盖现有外围放置。`validate_yuzuki_socket.py` 会重写旧报告并读取既有 ERC/DRC，不能当作当前全板只读验证。
- 设计生成、导出报告和只读检查先分清副作用。本次交接资料任务没有授权重建原理图/PCB；后续具体工程修改按用户届时授权执行。
- 优先更新已有文档；重要本地范围变更记在 [DECISIONS.md](DECISIONS.md)，普通修改只更新直接相关内容，不批量整理整个知识库。
- 内部文档使用相对链接并核对目标/锚点；上层 `.txt` 是原文快照，路径仍属原工作区，不把其历史任务当成本轮完成状态。
- 打包前核对磁盘文件与报告版本；包内 `MANIFEST.json` 只证明快照完整性，不证明电路、制造或实物验收通过。
