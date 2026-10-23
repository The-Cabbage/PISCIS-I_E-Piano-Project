# 紧凑控制件：STEP 模型与选型门禁

本清单服务于 70 mm × 70 mm 面板内的 6 个旋转编码器和 1 个三段开关。模型由原厂 CAD 下载包中直接提取；模型用于装配/干涉评估，不替代正式封装图与来料实测。

| 功能 | 推荐 MPN | 数量 | 机械外形 / 操作方向 | 选型结论 | STEP 文件 |
|---|---:|---:|---|---|---|
| 旋转编码器（最低 Z，首选） | ALPS Alpine `EC05E1220202` | 6 | **5 mm 尺寸，SMT / 回流焊，侧向操作，安装高度 4.5 mm**；12 detent / 12 pulse | 最适合纵向空间紧张的版本。须将 6 个控制孔布在机壳侧壁，或用传动/外伸旋钮实现操作。 | `ALPS_EC05E1220202_side.step` |
| 旋转编码器（顶面操作备选） | ALPS Alpine `EC05E1220401` | 6 | **5 mm 尺寸，SMT / 回流焊，顶面操作**；12 detent / 12 pulse | 同为 5 mm 级贴片结构；适合必须从顶面直接旋转的面板，但应先由 STEP 与面板/旋钮模型确认总高度。 | `ALPS_EC05E1220401_top.step` |
| 旋转编码器（较大旧备选） | ALPS Alpine `EC09E1524418` | 6 | 9 mm 尺寸，垂直（顶面）操作；20 mm 平轴；30 detent / 15 pulse；带按压开关 | 面积、轴长均不如 EC05E；仅在明确需要按压功能时保留。 | `ALPS_EC09E1524418.step` |
| 三段开关 | ALPS Alpine `SSSS811501` | 1 | 1P3T，水平拨动，SMT，9.7 × 2.6 × 1.4 mm | 推荐置于板边并在外壳开侧向拨动槽；它几乎不占 Z 向高度。若必须顶面拨动，应改用同系列垂直操作版本并重新下载/核对模型。 | `ALPS_SSSS811501.step` |
| OLED FPC 连接器 | Hirose `TF31-30S-0.5SH(800)` | 1 | 30P，0.5 mm pitch，**底接触**，SMT/ZIF，适配 0.30 mm FPC；19.3 × 5.7 × 2.0 mm | 与 30P/0.5 mm/0.30 mm 底接触 OLED FPC 需求一致；FPC 触点必须朝 PCB 面插入。 | `Hirose_TF31-30S-0.5SH-800.step` |

## 布局预算

使用 EC05E 时，6 枚可沿两个侧边或三组双联布置；元件本体仅为 5 mm 级，PCB 面积不再是限制项。若使用顶面操作版本，建议先以 **12 mm 旋钮外径、14 mm 中心距**做初始 3 × 2 阵列，编码器/旋钮区域约 40 mm × 26 mm，并为 OLED、Type-C 和固定孔保留边缘带。

建议给每个顶面编码器预留至少 7 mm 的本体禁布直径，旋钮则按实际外径另行留机械间隙。此预算只针对元件本体；最终面板还必须在导入 STEP 后以旋钮、屏幕玻璃、Type-C 外壳和电池高度的真实模型复核。

## Z 向风险

- `SSSS811501` 本体安装高度仅 1.4 mm；真正的外壳高度由拨块、面板槽与手指空间决定。
- `EC05E1220202` 的安装高度为 4.5 mm，且操作轴沿 PCB 平面方向，是本项目的低 Z 首选。
- `EC05E1220401` 虽为 5 mm SMT 件，但从顶面操作仍需要旋钮和面板开孔的高度空间；不能将“5 mm 尺寸”理解为总 Z 高度。

## 来源与追溯

- EC05E1220202 产品页与原始 3D CAD 包：<https://tech.alpsalpine.com/e/products/detail/EC05E1220202/>；下载于 2026-09-19。
- EC05E1220401 产品页与原始 3D CAD 包：<https://tech.alpsalpine.com/e/products/detail/EC05E1220401/>；下载于 2026-09-19。
- EC09E1524418 产品页与原始 3D CAD 包：<https://tech.alpsalpine.com/e/products/detail/EC09E1524418/>；下载于 2026-09-19。
- SSSS811501 产品页与原始 3D CAD 包：<https://tech.alpsalpine.com/e/products/detail/SSSS811501/>；下载于 2026-09-19。
- STEP 原文件归档目录：`hardware/libraries/3dmodels/PISCIS_Control.3dshapes/`。运行 `node tools/generate_private_library.mjs` 后会复制到 `private/PISCIS_Private.3dshapes/`。

TF31 官方来源：<https://www.hirose.com/en/product/p/CL0480-0179-6-00>；其原厂 STEP 下载包于 2026-09-19 获取。

原厂产品页的供货状态/最小订货量应在下单前再次核验；本清单并未替代采购确认。
