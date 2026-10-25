# SJ1-3523N 替换资料（2026-10-24）

型号：Same Sky（原 CUI）SJ1-3523N；3.5 mm TRS、三触点、无切换开关、直插式。
官方资料：https://www.sameskydevices.com/product/resource/sj1-352xn.pdf
数据手册版本：2024-09-12。第 2 页 PIN 表直接定义 1=sleeve，2=tip，3=ring。

KiCad 符号：Connector_Audio:AudioJack3（标准库，电气脚号为 S/T/R）。
值：SJ1-3523N。
封装：PISCIS_Private:AudioJack_SameSky_SJ1-3523N_Horizontal。
S = 厂家 1 脚 = GND；T = 厂家 2 脚 = HP_L_CAND；R = 厂家 3 脚 = HP_R_CAND。

封装依据：本机 KiCad 10 标准 Jack_3.5mm_CUI_SJ1-3523N_Horizontal。
保留焊盘/定位孔中心和模型变换；按照厂家第 2 页推荐 PCB 图，将三个电气端子槽孔调整为 1.5×0.8 mm，铜盘调整为 2.5×1.8 mm（对应横/竖方向）。
五个非金属化定位孔沿用厂家推荐 1.2 mm；未安装开关触点对应的孔位按原库保留。
STEP 模型复制自同名 KiCad 标准模型，并改为项目相对路径。未做实物插接或外壳净空验证。

用户授权后已完成 J5 的原理图、PCB 封装、板边槽口和左右声道连接。原有走线及其他器件位置保留。
原 PJ-327A-Y 脚位表不适用于本型号；替换时应删除旧 J5 符号并重新连接三个 S/T/R 引脚。

机械注意：本封装带 Edge.Cuts 开口槽，需放在板边并将外框与封装两端（局部 X=±6.5，Y=4.5）接合，不能放在板内后直接生产。独立测试板和当前工程均已按此闭合外框。
