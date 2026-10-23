# 五维开关与音频 Codec 器件库

2026-10-20 收集，适用于本目录所属的全志 Beta KiCad 工程。库名 `PISCIS_Controls_Audio` 已在项目根目录的符号/封装库表中注册；在原理图编辑器按 **A**，搜索下表符号即可。此次只收集库资源，未向现有原理图或 PCB 添加器件，也不表示器件已完成实物验证或整机选型冻结。

| 器件 | 可搜索的符号 | 已绑定封装 | 模型 |
|---|---|---|---|
| ALPS SKRHADE010，嘉立创 C202415 | `PISCIS_Controls_Audio:SKRHADE010` | `SW-SMD_SKRHADE010` | EasyEDA 配套 WRL/STEP；另附 ALPS 原厂 STEP |
| ES8388，C365736，KiCad 符号版本 | `PISCIS_Controls_Audio:ES8388` | `QFN-28-1EP_4x4mm_P0.45mm_EP2.6x2.6mm` | 已绑定本地 STEP |
| ES8388，EasyEDA 转换版本 | `PISCIS_Controls_Audio:ES8388_C365736_EasyEDA` | `QFN-28_L4.0-W4.0-P0.45-BL-EP` | EasyEDA 配套 WRL/STEP |

## 来源与转换说明

**C202415 符号、焊盘几何，以及两颗器件的 EasyEDA 配套模型，来源为“嘉立创EDA/EasyEDA官方库”：[嘉立创EDA](https://lceda.cn/) · [EasyEDA](https://easyeda.com)。** 原始记录保留在 [C202415.json](../../../easyeda_components/raw/C202415.json) 和 [C365736.json](../../../easyeda_components/raw/C365736.json)；转换工具为 [easyeda2kicad.py](https://github.com/uPesy/easyeda2kicad.py) 1.0.1。未调整版本的转换结果保存在 `sources/`，供追溯。

- C202415 型号对应关系：[嘉立创产品页](https://jlcpcb.com/partdetail/ALPSALPINE-SKRHADE010/C202415)。
- ALPS 尺寸/电路图及 CAD：[原厂产品页](https://tech.alpsalpine.com/e/products/detail/SKRHADE010/)。原厂 CAD ZIP 与使用说明保存在 `sources/`；未改动的原厂 STEP 为 [SKRHADE010_ALPS_original.step](PISCIS_Controls_Audio.3dshapes/SKRHADE010_ALPS_original.step)，未直接绑定到旋转后的 EasyEDA 封装，使用时需自行对齐坐标。
- `ES8388` 符号及 2.6 mm EP 封装来自本机 KiCad 10.0.6 官方库：[KiCad 符号库](https://gitlab.com/kicad/libraries/kicad-symbols)、[封装库](https://gitlab.com/kicad/libraries/kicad-footprints)。该组文件适用 [KiCad 库许可及例外](https://www.kicad.org/libraries/license/)。仅调整本地绑定、物料字段及参考输出引脚类型，未改变针号。
- [ES8388 数据手册 Rev5.0](sources/ES8388_DS.pdf) 为立创托管的原厂文件；[原始下载地址](https://datasheet.lcsc.com/datasheet/pdf/b5b69a0d4875954a40e8f8a3f42483af.pdf?productCode=C365736)。

## 使用前需区分的细节

**SKRHADE010 是四方向加中央按压开关。** 原厂电路图使用 A/B/C/D 标识方向：1=A，2=中央按压，3=C，4=B，5=公共端，6=D。方向与屏幕“上/下/左/右”的关系取决于实际安装旋转，不在此库中固定。7/8 保留为库中的固定焊脚，不自动与 5 脚短接；8 个符号端子均设为 passive。

原始 EasyEDA 转换将两个定位孔写成金属化通孔，且孔径为 0.90/1.20 mm。本地使用版已改为 **NPTH**，孔径按 [ALPS SKRH 目录](sources/SKRH_catalog.pdf) Drawing No.4 的名义尺寸设为 **0.75/1.05 mm**；其余焊盘位置保留来源几何，未进行实物装配验证。

**ES8388 两个封装版本不能按名称混用。** KiCad 版本中央铜焊盘为 2.6×2.6 mm；EasyEDA 版本为 2.4×2.4 mm，另有针脚 1 朝向差异。两套符号分别绑定各自封装；原厂手册第 35 页给出的裸露金属尺寸范围为 2.5–2.7 mm，这与 PCB 铜焊盘/钢网开口尺寸是不同定义。

`ES8388` 版保留官方符号的 ERC 类型，并按手册第 6 页将 10/19/20 三个参考输出脚修正为 power_out；9/25 为 NC，29 为 EPAD。未在符号内自动将 EPAD 接地。EasyEDA 转换版保留原来的 unspecified 电气类型，使用时不能据此宣称 ERC 已覆盖输入/输出冲突。

ES8388 的官方 KiCad 模型链接在本机缺少对应文件；本库使用已下载的 EasyEDA STEP，并将 KiCad 版本中的模型旋转设为 90°，已在顶视渲染中核对其 1 脚圆点与封装标记一致。所有使用版封装的模型路径均为 `${KIPRJMOD}` 相对路径。

## 实际完成的文件检查

检查条件：KiCad CLI/pcbnew 10.0.6，2026-10-20；使用临时独立检查板，未重建或修改项目母座电路。

- 三个符号均由 KiCad 成功导出 SVG，三个封装均成功载入。
- 摇杆 8 个端子、两个 ES8388 版本各 29 个编号焊盘均与符号编号匹配；KiCad 封装的无编号钢网开口不计入电气针脚。
- 两个定位孔确认为 NPTH；所有绑定模型路径指向存在的本地文件。
- 临时检查板已完成 3D 渲染；[顶视预览](previews/models-top.png) 从左到右为 KiCad ES8388、EasyEDA ES8388、SKRHADE010。
- 来源、变更说明、文件哈希及检查记录见 [manifest.json](manifest.json)。这些检查证明文件可加载及绑定关系，不代替焊接、机械配合或音频电路验证。
