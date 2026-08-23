from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_CELL_VERTICAL_ALIGNMENT
from docx.enum.style import WD_STYLE_TYPE
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.enum.section import WD_SECTION
from docx.enum.text import WD_BREAK
from datetime import date

OUT = "技术宅与深度爱好者高溢价酷炫产品研究_50件.docx"

products = [
 (1,"音乐创作硬件","Teenage Engineering OP–1 field","便携合成器/采样器","约 US$1,999","极简铝壳、彩色旋钮、磁带式交互","把复杂工作流压缩进可随身携带的“乐器对象”","电子音乐制作人、工业设计爱好者","高","https://teenage.engineering/store/op-1-field"),
 (2,"音乐创作硬件","Teenage Engineering OP–XY","便携音序器/合成器","约 US$2,299","黑色网格界面、动态图形、强节奏反馈","以“表演性操作”替代菜单感，适合视频展示","节拍制作人、现场表演者","高","https://teenage.engineering/store/op-xy"),
 (3,"音乐创作硬件","Teenage Engineering TP–7","数字录音机","US$1,499","电机驱动旋转盘、铝合金机身、触觉快进回放","把录音机的机械动作重新做成数字时代仪式","记者、播客、声音采集者","很高","https://teenage.engineering/store/tp-7"),
 (4,"音乐创作硬件","Teenage Engineering TX–6","超便携混音器/音频接口","约 US$1,199","微型推子、彩屏、密集精密布局","“掌中专业台”带来微缩机械奇观","移动音乐人、硬件控","很高","https://teenage.engineering/store/tx-6"),
 (5,"音乐创作硬件","Teenage Engineering CM–15","便携大振膜麦克风","US$1,199","超薄铝壳、内置支架、模块化接口","把录音棚设备变成时尚随身物件","创作者、播客、录音发烧友","很高","https://teenage.engineering/store/cm-15"),
 (6,"桌面控制与效率","Elgato Stream Deck Studio","广播级控制台","US$899.99","双触控条、32键、机架式横向布局","用可变图标和灯光让工作流实时可视化","直播导播、演播室、AV 技术人员","高","https://www.elgato.com/us/en/p/stream-deck-studio"),
 (7,"桌面控制与效率","Elgato Stream Deck + XL","桌面可编程控制器","US$349.99","大面积 LCD 键阵列 + 旋钮/触控","把软件操作实体化为“个人驾驶舱”","剪辑师、主播、效率玩家","中高","https://www.elgato.com/us/en/c/stream-deck"),
 (8,"桌面控制与效率","Elgato Stream Deck +","旋钮式创作控制器","US$199.99","LCD 键、四旋钮、触控条","参数变化有视觉与触觉双反馈","视频、音频、直播创作者","中高","https://www.elgato.com/us/en/p/stream-deck-plus-black"),
 (9,"桌面控制与效率","Elgato Stream Deck Neo","轻量自动化控制器","US$99.99","8 个 LCD 键、信息条、触控翻页","以低门槛把日常流程做成可炫耀的桌搭组件","桌搭新人、知识工作者","中","https://www.elgato.com/us/en/p/stream-deck-neo-black"),
 (10,"桌面控制与效率","Lofree Edge","超薄机械键盘","标价约 US$249","碳纤维/镁铝结构、低矮轮廓、悬浮感","轻薄与机械手感的冲突感本身就是卖点","键盘玩家、移动办公用户","中高","https://www.lofree.co/collections/all-products"),
 (11,"PC 展示与模块化","HYTE Y70 touch infinite","带触屏全景机箱","US$399.99","转角 2.5K 触屏、三面玻璃、硬件展柜化","把内部硬件和系统信息共同变成橱窗内容","装机玩家、二次元联名用户","高","https://hyte.com/store/y70"),
 (12,"PC 展示与模块化","HYTE Y60 LCD DIY Kit","机箱显示屏改装件","US$150","把转角玻璃位变为纵向 LCD","低成本制造强烈的“赛博升级”前后对比","DIY 装机与改造玩家","高","https://hyte.com/pc-accessories/case-components"),
 (13,"PC 展示与模块化","HYTE Keeb TKL","透明机械键盘","约 US$199.99","透明外壳、环绕 RGB、磁吸滚轮","桌面灯效和输入设备合并为展示中心","RGB 桌搭、机械键盘玩家","中高","https://hyte.com/store"),
 (14,"PC 展示与模块化","Teenage Engineering computer–1","扁平包装 ITX 机箱","约 US$149","用户折弯组装、橙色喷涂、显露铆接结构","装配过程成为所有权仪式，留下亲手完成的痕迹","工业设计与 SFF 玩家","高","https://teenage.engineering/store/computer-1"),
 (15,"PC 展示与模块化","Nothing Phone (3)","设计导向智能手机","标价 US$799","透明层次、Glyph Matrix 点阵屏、实体化后盖叙事","将背面从被忽略区域变为身份展示界面","设计敏感型科技用户","中高","https://us.nothing.tech/collections/shop-all"),
 (16,"沉浸式灯光","Nanoleaf Shapes Hexagons","模块化墙面灯","5片 US$119.99","拼接几何、触控、音乐同步、屏幕镜像","用户不是买灯，而是在持续构建一面发光作品","游戏房、桌搭、智能家居玩家","中高","https://nanoleaf.me/en-us/collections/nanoleaf-shapes"),
 (17,"沉浸式灯光","Nanoleaf Lines","模块化线性灯条","9条标价约 US$199.99","60°几何连接、背投光、动态图案","线性语言比灯泡更像空间 UI","游戏玩家、直播间、空间改造者","高","https://nanoleaf.me/en-us/collections/shop-all-home-and-lighting"),
 (18,"沉浸式灯光","Nanoleaf Skylight","模块化智能顶灯","3片标价 US$199.99","像素化天窗、顶面扩展、动态色彩","把通常隐形的顶灯变为房间主视觉","智能家居深度用户","高","https://nanoleaf.me/en-US/products/ceiling-lights/skylight/smarter-kit-4-pack/"),
 (19,"沉浸式灯光","Nanoleaf 4D Screen Mirror","电视屏幕同步灯","标价 US$99.99","摄像头实时取色、40+ 色区、四档沉浸","效果一眼可见，极适合短视频传播","影音与主机游戏用户","中高","https://nanoleaf.me/en-us/products/nanoleaf-4d"),
 (20,"沉浸式灯光","Nanoleaf Pegboard Desk Dock","发光桌面洞洞板","US$59.99","收纳、充电与 RGB 点阵背景合一","实用功能为炫酷展示提供正当性","桌搭与小空间用户","中","https://nanoleaf.me/en-US/products/"),
 (21,"精品音频","Bang & Olufsen Beoplay H100","旗舰降噪耳机","US$2,200–2,300","钛驱动、玻璃触控面、可维护模块与皮革","把消费电子做成可长期持有的珠宝级物件","高端音频、差旅与奢侈品用户","很高","https://www.bang-olufsen.com/en/us/headphones/beoplay-h100"),
 (22,"精品音频","Bang & Olufsen Beosound A9","家居设计音箱","US$4,550–4,650","圆盘织物面、木腿、可换外观","无需隐藏音箱，主动把它变成家具雕塑","室内设计与高端音频用户","很高","https://www.bang-olufsen.com/en/us/speakers/beosound-a9"),
 (23,"精品音频","Bang & Olufsen Beosound A5","便携高端音箱","约 US$1,650–1,850","提篮轮廓、木/织物材质、模块化寿命叙事","野餐篮式亲和形态软化高科技感","生活方式与音频用户","很高","https://www.bang-olufsen.com/en/us"),
 (24,"精品音频","Bang & Olufsen Beosound 2","落地式无线音箱","约 US$4,000","锥形铝体、360°出声、顶部触控","形态高度辨识，像未来主义家居器物","建筑师、设计藏家、音频用户","很高","https://www.bang-olufsen.com/en/us/gifts"),
 (25,"精品音频","Bang & Olufsen Beolab 8","可配置高端扬声器","约 US$9,000/对起","可换外饰、波束控制、支架多形态","“可配置”让音频系统像高级家具定制","高净值音响与家居用户","很高","https://www.bang-olufsen.com/en/us/gifts"),
 (26,"影像器材","Leica Q3 Monochrom","全画幅黑白定焦相机","€6,750 建议价","纯黑机身、取消红标、仅记录黑白亮度","主动舍弃彩色功能，强化纯粹与圈层身份","黑白摄影师、徕卡收藏者","很高","https://leica-camera.com/en-US/photography/cameras/q/q3-monochrom"),
 (27,"影像器材","Leica SL3","专业全画幅无反","US$6,995 发布价","德国制造、极简按键、界面随方向旋转","把专业性表达为克制、精密与触觉一致性","职业摄影师、系统玩家","高","https://leica-camera.com/en-US/press/new-leica-sl3"),
 (28,"影像器材","Leica SL3-S","高速全画幅混合相机","€5,190 建议价","紧凑金属机身、专业视频接口、清晰 UI","不靠外露结构炫技，而靠“工具纯度”建立溢价","视频创作者、专业摄影师","高","https://leica-camera.com/en-US/press/new-leica-sl3-s"),
 (29,"影像器材","Hasselblad X2D 100C","一亿像素中画幅相机","约 US$7,399","斯堪的纳维亚极简、顶部状态屏、橙色快门","高像质与慢节奏拍摄仪式绑定","商业/风光摄影与器材收藏者","很高","https://www.hasselblad.com/x-system/x2d-100c/"),
 (30,"影像器材","Hasselblad 907X & CFV 100C","模块化中画幅相机","约 US$8,199","方盒模块、腰平观看、兼容经典 V 系统","通过历史接口兼容把怀旧与未来连接起来","哈苏系统玩家、收藏者","很高","https://www.hasselblad.com/x-system/907x-cfv-100c/"),
 (31,"精品咖啡器具","Weber Workshops EG-1 Mk.3","单剂量平刀磨豆机","US$4,100","外露大刀盘结构、航天感支架、低转速","让研磨过程和机械精度成为台面表演","精品咖啡与器械发烧友","极高","https://weberworkshops.com/collections/highlighted-products"),
 (32,"精品咖啡器具","Weber Workshops The Key Mk.2","锥刀电动磨豆机","US$2,050","83mm 锥刀、极窄直立体量、金属与木饰","以“不可能的小体积”承载超规格部件","意式咖啡深度玩家","极高","https://weberworkshops.com/products/key-grinder"),
 (33,"精品咖啡器具","Weber Workshops MOONRAKER","行星式布粉器","US$285–485","行星齿轮轨迹、精密针阵列、旋转动作","把一个小步骤升级为机械腕表式仪式","意式萃取与比赛型玩家","极高","https://weberworkshops.com/products/moonraker"),
 (34,"精品咖啡器具","Acaia Lunar","专业意式咖啡秤","US$250","紧凑铝体、流速/计时显示、App 联动","把称重数据实时化，强化可重复实验感","咖啡师、参数党","高","https://acaia.co/collections/coffee-scales"),
 (35,"精品咖啡器具","Acaia Orbit","单剂量磨豆机","约 US$1,650–1,900（地区差异）","极简直筒、App 控速、自动停机","软件可调参数让磨豆机成为实验平台","咖啡参数党、家用吧台用户","高","https://acaia.co/collections/all"),
 (36,"精品咖啡器具","Fellow Ode Brew Grinder Gen 2","手冲专用磨豆机","US$399.95","低矮几何、磁吸接粉杯、敲粉拨片","用安静、洁净和触觉细节包装专业体验","手冲咖啡与家居设计用户","中高","https://fellowproducts.com/collections/elevate-your-aiden-experience/products/ode-brew-grinder-gen-2"),
 (37,"精品咖啡器具","Fellow Aiden","精密滴滤咖啡机","约 US$399.95","方正黑体、彩屏引导、自动冲煮曲线","把手冲知识封装为可观看、可复现的程序","想要参数但不想手动操作的用户","中高","https://fellowproducts.com/products/aiden-precision-coffee-maker"),
 (38,"精品咖啡器具","La Marzocco Linea Mini","家用双锅炉意式机","约 US$6,000+","商用机缩小、桨式操作、鲜明配色","把咖啡馆操作台浓缩为家中主视觉","家庭咖啡吧、品牌忠实用户","很高","https://www.lamarzoccousa.com/home-products/espresso-machines/linea-mini/"),
 (39,"精品咖啡器具","La Marzocco GS3","旗舰家用意式机","约 US$7,500+","暴露式冲煮头、压力曲线控制、商用结构","高价来自可见结构、可控参数与品牌血统叠加","极深度意式玩家","很高","https://www.lamarzoccousa.com/home-products/espresso-machines/gs3/"),
 (40,"精品咖啡器具","La Marzocco Pico","家用磨豆机","约 US$995","短豆仓、旋钮式调节、与咖啡机配色成套","系统化外观鼓励同品牌整套购买","家用意式与套装用户","高","https://www.lamarzoccousa.com/home-products/coffee-grinders/pico/"),
 (41,"复古游戏与触觉玩具","Analogue Pocket Aluminum Edition","FPGA 复古掌机","发布价约 US$499.99","CNC 铝壳、高分辨率竖屏、实体卡带","现代精密制造为旧媒介重新赋予收藏价值","复古游戏、掌机、卡带收藏者","很高","https://www.analogue.co/pocket"),
 (42,"复古游戏与触觉玩具","Panic Playdate","曲柄掌机","US$229","黄色机身、机械曲柄、每周游戏机制","一个夸张而可演示的输入件定义整台产品","独立游戏与交互设计爱好者","高","https://play.date/"),
 (43,"复古游戏与触觉玩具","AYANEO FLIP DS","双屏 Windows 掌机","约 US$699 起","翻盖双屏、全键盘级复杂布局、RGB","用硬件形态复活双屏掌机记忆并叠加 PC 性能","掌机收藏者、模拟器玩家","高","https://www.ayaneo.com/product/AYANEO-FLIP-DS.html"),
 (44,"复古游戏与触觉玩具","Razer Kitsune","无摇杆街机控制器","US$299.99","超薄全按钮、光学开关、Chroma 灯效","删除传统摇杆形成鲜明技术立场与竞技身份","格斗游戏玩家","高","https://www.razer.com/console-controllers/razer-kitsune"),
 (45,"复古游戏与触觉玩具","Fanatec Podium Wheel Base DD2","直驱模拟赛车基座","约 US$1,499.95","碳纤维面板、OLED、强扭矩直驱","真实力反馈将虚拟驾驶变成身体可感的机械事件","模拟赛车深度玩家","高","https://fanatec.com/us-en/racing-wheels-direct-drive-bases/direct-drive-bases/podium-wheel-base-dd2"),
 (46,"骑行训练与安全科技","Garmin Tacx NEO 3M","旗舰直驱骑行台","US$1,999.99 建议价","多向位移、虚拟路面、动态灯光/阻力反馈","把室内训练从数据任务变为机械沉浸体验","公路骑行与铁三训练者","高","https://www.garmin.com/en-US/newsroom/press-release/sports-fitness/garmin-unveils-the-tacx-neo-3m-a-powerful-indoor-smart-trainer-with-multidirectional-movement/"),
 (47,"骑行训练与安全科技","Garmin Edge 1050","旗舰骑行码表","约 US$699.99","高亮彩屏、路线/爬坡可视化、骑行动态","把大量训练数据转化为车把上的实时指挥界面","数据型公路/山地骑手","中高","https://www.garmin.com/en-US/p/1196129"),
 (48,"骑行训练与安全科技","Garmin Varia RCT715","雷达尾灯摄像机","约 US$399.99","后向雷达、录像、尾灯三合一","将看不见的后方风险变成可见预警","重视安全与数据的公路骑手","高","https://www.garmin.com/en-US/p/721258"),
 (49,"骑行训练与安全科技","Wahoo KICKR MOVE","前后浮动直驱骑行台","约 US$1,599.99","内置前后位移、逼真惯性、App 生态","运动轨迹可见可感，缓解固定骑行台的机械僵硬","室内骑行与 Zwift 玩家","高","https://www.wahoofitness.com/devices/indoor-cycling/bike-trainers/kickr-move-buy"),
 (50,"骑行训练与安全科技","Wahoo KICKR BIKE SHIFT","智能室内自行车","约 US$2,999.99","无外露飞轮、虚拟变速、整车参数调节","把训练设备塑造成极简未来机械","高预算室内骑行用户","高","https://www.wahoofitness.com/devices/indoor-cycling/smart-bikes/kickr-bike-shift-buy"),
]

sources = [
 ("Teenage Engineering Field System / CM–15 官方商店","https://teenage.engineering/store/field-system"),
 ("Elgato Stream Deck 产品集合","https://www.elgato.com/us/en/c/stream-deck"),
 ("HYTE Y 系列价格说明","https://hyte.com/nl/blog/update-yseries-price"),
 ("HYTE 商店与机箱配件","https://hyte.com/store"),
 ("Nothing 美国全产品页","https://us.nothing.tech/collections/shop-all"),
 ("Lofree 全产品页","https://www.lofree.co/collections/all-products"),
 ("Nanoleaf 全产品与灯光集合","https://nanoleaf.me/en-US/products/"),
 ("Bang & Olufsen 产品与礼赠集合","https://www.bang-olufsen.com/en/us/gifts"),
 ("Leica Q3 Monochrom 官方发布","https://leica-camera.com/en-US/press/leica-q3-monochrom"),
 ("Leica SL3 官方发布","https://leica-camera.com/en-US/press/new-leica-sl3"),
 ("Leica SL3-S 官方发布","https://leica-camera.com/en-US/press/new-leica-sl3-s"),
 ("Weber Workshops 精选产品","https://weberworkshops.com/collections/highlighted-products"),
 ("Acaia 咖啡秤集合","https://acaia.co/collections/coffee-scales"),
 ("Fellow 磨豆机集合","https://fellowproducts.com/collections/grinder"),
 ("Garmin Tacx NEO 3M 官方发布","https://www.garmin.com/en-US/newsroom/press-release/sports-fitness/garmin-unveils-the-tacx-neo-3m-a-powerful-indoor-smart-trainer-with-multidirectional-movement/"),
]

BLUE = "17324D"; TEAL="00A6A6"; PALE="EAF4F4"; DARK="19242E"; MUTED="637282"; GOLD="D49B2A"; WHITE="FFFFFF"; GRID="C9D4DB"

def shade(cell, fill):
    tcPr=cell._tc.get_or_add_tcPr(); shd=tcPr.find(qn('w:shd'))
    if shd is None: shd=OxmlElement('w:shd'); tcPr.append(shd)
    shd.set(qn('w:fill'), fill)

def margins(cell, top=80, start=110, bottom=80, end=110):
    tc=cell._tc.get_or_add_tcPr(); tcMar=tc.first_child_found_in('w:tcMar')
    if tcMar is None: tcMar=OxmlElement('w:tcMar'); tc.append(tcMar)
    for tag,val in [('top',top),('start',start),('bottom',bottom),('end',end)]:
        x=tcMar.find(qn('w:'+tag))
        if x is None: x=OxmlElement('w:'+tag); tcMar.append(x)
        x.set(qn('w:w'),str(val)); x.set(qn('w:type'),'dxa')

def set_font(run, size=10.5, bold=False, color=DARK, name='Aptos'):
    run.font.name=name; run._element.get_or_add_rPr().rFonts.set(qn('w:eastAsia'),'Microsoft YaHei')
    run.font.size=Pt(size); run.bold=bold; run.font.color.rgb=RGBColor.from_string(color)

def add_link(p, text, url, size=8.5):
    part=p.part; rid=part.relate_to(url,'http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink',is_external=True)
    h=OxmlElement('w:hyperlink'); h.set(qn('r:id'),rid); r=OxmlElement('w:r'); rPr=OxmlElement('w:rPr')
    c=OxmlElement('w:color'); c.set(qn('w:val'),TEAL); rPr.append(c); u=OxmlElement('w:u'); u.set(qn('w:val'),'single'); rPr.append(u)
    sz=OxmlElement('w:sz'); sz.set(qn('w:val'),str(int(size*2))); rPr.append(sz); r.append(rPr)
    t=OxmlElement('w:t'); t.text=text; r.append(t); h.append(r); p._p.append(h)

doc=Document(); sec=doc.sections[0]
sec.page_width=Inches(8.5); sec.page_height=Inches(11); sec.top_margin=Inches(.8); sec.bottom_margin=Inches(.75); sec.left_margin=Inches(.8); sec.right_margin=Inches(.8)
styles=doc.styles
normal=styles['Normal']; normal.font.name='Aptos'; normal._element.rPr.rFonts.set(qn('w:eastAsia'),'Microsoft YaHei'); normal.font.size=Pt(10.5); normal.font.color.rgb=RGBColor.from_string(DARK)
normal.paragraph_format.space_after=Pt(6); normal.paragraph_format.line_spacing=1.18
for nm,sz,bef,aft,col in [('Title',30,0,10,BLUE),('Subtitle',13,0,18,MUTED),('Heading 1',18,18,8,BLUE),('Heading 2',13,12,5,TEAL),('Heading 3',11,8,3,BLUE)]:
    s=styles[nm]; s.font.name='Aptos Display'; s._element.rPr.rFonts.set(qn('w:eastAsia'),'Microsoft YaHei'); s.font.size=Pt(sz); s.font.bold=(nm!='Subtitle'); s.font.color.rgb=RGBColor.from_string(col); s.paragraph_format.space_before=Pt(bef); s.paragraph_format.space_after=Pt(aft); s.paragraph_format.keep_with_next=True

# header/footer
hp=sec.header.paragraphs[0]; hp.text='PISCIS-I 机会空间研究  /  2026.08'; hp.alignment=WD_ALIGN_PARAGRAPH.RIGHT
for r in hp.runs: set_font(r,8,False,MUTED)
fp=sec.footer.paragraphs[0]; fp.alignment=WD_ALIGN_PARAGRAPH.CENTER
run=fp.add_run('深度爱好者高溢价产品样本库  •  '); set_font(run,8,False,MUTED)
fld=OxmlElement('w:fldSimple'); fld.set(qn('w:instr'),'PAGE'); fp._p.append(fld)

# cover
p=doc.add_paragraph(); p.paragraph_format.space_before=Pt(92); r=p.add_run('MARKET SCAN'); set_font(r,10,True,GOLD)
p=doc.add_paragraph(style='Title'); p.add_run('面向技术宅与深度爱好者的\n高溢价“酷炫产品”研究')
p=doc.add_paragraph(style='Subtitle'); p.add_run('跨 10 个场景、50 件产品的设计语言、视觉效果与溢价机制样本库')
p=doc.add_paragraph(); p.paragraph_format.space_before=Pt(28); p.paragraph_format.space_after=Pt(20)
r=p.add_run('核心问题  '); set_font(r,10,True,TEAL); r=p.add_run('当基础功能已高度成熟，什么样的设计与体验仍能让深度爱好者主动支付显著溢价？'); set_font(r,13,True,BLUE)
t=doc.add_table(rows=1,cols=3); t.alignment=WD_TABLE_ALIGNMENT.CENTER; t.autofit=False
for i,(a,b) in enumerate([('50','具体产品'),('10','爱好者场景'),('7','溢价机制')]):
    c=t.cell(0,i); c.width=Inches(2.22); shade(c,PALE); margins(c,170,150,160,150); c.vertical_alignment=WD_CELL_VERTICAL_ALIGNMENT.CENTER
    p=c.paragraphs[0]; p.alignment=WD_ALIGN_PARAGRAPH.CENTER; rr=p.add_run(a+'\n'); set_font(rr,22,True,TEAL); rr=p.add_run(b); set_font(rr,9,True,BLUE)
p=doc.add_paragraph(); p.paragraph_format.space_before=Pt(28); r=p.add_run('研究日期：2026-08-23  |  价格以官方公开美元/欧元标价为主，地区、税费和促销会造成差异'); set_font(r,8.5,False,MUTED)
doc.add_page_break()

doc.add_heading('1. 研究口径与结论摘要',1)
doc.add_heading('类目定义',2)
doc.add_paragraph('本研究关注的不是泛奢侈品，而是“对某一技术、工具或体验有知识门槛的深度用户”愿意高价购买的产品。入选样本通常同时满足三项：面向明确圈层；价格显著高于完成基础功能所需；设计风格、动态反馈、机械动作或可展示性是核心购买理由之一。')
doc.add_heading('一句话结论',2)
p=doc.add_paragraph(); shade_dummy=None
r=p.add_run('高溢价并不来自“更多功能”本身，而来自把专业能力做成可看见、可触摸、可表演、可收藏、可持续扩展的身份载体。'); set_font(r,14,True,BLUE)
doc.add_heading('七种反复出现的溢价机制',2)
mechanisms=[('1  可视化专业性','屏幕、状态灯、外露结构和实时数据让能力立即可见。'),('2  操作仪式化','旋钮、曲柄、拨杆、磁吸、旋转盘把步骤变成令人愉悦的动作。'),('3  反常规形态','双屏、纯黑白相机、无摇杆控制器、转角触屏机箱形成鲜明立场。'),('4  材料与精密感','CNC 铝、玻璃、皮革、木材与高精度装配承担“可信的昂贵感”。'),('5  模块与生态锁定','扩展面板、配套外饰、附件和软件配置，使一次购买变成长期系统建设。'),('6  圈层身份编码','懂参数、懂历史、懂工作流的人才能完整理解产品价值，形成“内行暗号”。'),('7  适合被展示和传播','效果可在 3 秒内被短视频、桌搭照片或线下演示理解。')]
for h,d in mechanisms:
    p=doc.add_paragraph(style='Heading 3'); p.add_run(h)
    p=doc.add_paragraph(d); p.paragraph_format.left_indent=Inches(.18)

doc.add_heading('样本分布',2)
counts={}
for x in products: counts[x[1]]=counts.get(x[1],0)+1
tbl=doc.add_table(rows=1,cols=3); tbl.alignment=WD_TABLE_ALIGNMENT.CENTER; tbl.autofit=False
headers=['场景','件数','典型效果']
for i,h in enumerate(headers): c=tbl.cell(0,i); c.text=h; shade(c,BLUE); margins(c); [set_font(r,9,True,WHITE) for r in c.paragraphs[0].runs]
effects={'音乐创作硬件':'彩屏 + 旋钮 + 表演性操作','桌面控制与效率':'可编程图标 + 触觉控制','PC 展示与模块化':'透明、RGB、屏幕化','沉浸式灯光':'空间同步与动态色彩','精品音频':'家居雕塑 + 高级材质','影像器材':'工具纯度 + 系统血统','精品咖啡器具':'机械仪式 + 参数控制','复古游戏与触觉玩具':'独特输入与怀旧媒介','骑行训练与安全科技':'身体反馈 + 实时数据'}
for k,v in counts.items():
    row=tbl.add_row().cells
    for i,txt in enumerate([k,str(v),effects[k]]): row[i].text=txt; margins(row[i]); row[i].vertical_alignment=WD_CELL_VERTICAL_ALIGNMENT.CENTER; [set_font(r,9,False,DARK) for r in row[i].paragraphs[0].runs]
for row in tbl.rows:
    row.cells[0].width=Inches(2.1); row.cells[1].width=Inches(.7); row.cells[2].width=Inches(3.95)

doc.add_page_break(); doc.add_heading('2. 50 件产品样本库',1)
doc.add_paragraph('阅读方式：价格为研究时点公开标价或近期建议价；“溢价强度”是综合品类均价、材料、品牌与功能替代品后的定性判断，不等于毛利率。每件产品附官方来源，可用于后续复核。')

current=None
for n,cat,name,typ,price,visual,logic,aud,prem,url in products:
    if cat!=current:
        if current is not None: doc.add_paragraph().paragraph_format.space_after=Pt(1)
        doc.add_heading(cat,2); current=cat
    p=doc.add_paragraph(); p.paragraph_format.keep_with_next=True; p.paragraph_format.space_before=Pt(7); p.paragraph_format.space_after=Pt(2)
    r=p.add_run(f'{n:02d}  {name}'); set_font(r,11.5,True,BLUE)
    r=p.add_run(f'  ·  {typ}  ·  {price}  ·  溢价 {prem}'); set_font(r,9,False,MUTED)
    p=doc.add_paragraph(); p.paragraph_format.left_indent=Inches(.25); p.paragraph_format.space_after=Pt(1)
    r=p.add_run('酷炫表达：'); set_font(r,9.2,True,TEAL); r=p.add_run(visual); set_font(r,9.2)
    p=doc.add_paragraph(); p.paragraph_format.left_indent=Inches(.25); p.paragraph_format.space_after=Pt(1)
    r=p.add_run('溢价逻辑：'); set_font(r,9.2,True,TEAL); r=p.add_run(logic); set_font(r,9.2)
    p=doc.add_paragraph(); p.paragraph_format.left_indent=Inches(.25); p.paragraph_format.space_after=Pt(4)
    r=p.add_run('核心人群：'); set_font(r,8.7,True,MUTED); r=p.add_run(aud+'  |  '); set_font(r,8.7,False,MUTED); add_link(p,'官方来源',url,8.7)

doc.add_page_break(); doc.add_heading('3. 跨品类洞察：可迁移的产品策略',1)
insights=[
('把“内部状态”变成外部戏剧','最有传播力的产品会把原本隐藏的数据、结构或动作显性化：转角屏展示主机状态，灯光跟随画面变化，骑行台让虚拟坡度变成身体感受。设计机会不一定是增加功能，而是把既有功能演出来。'),
('昂贵感需要同时满足近看与远看','远看要有轮廓记忆点，近看要有材料、阻尼、缝隙与反馈的证据。只有远看炫酷容易沦为廉价 RGB；只有近看精密则难以在社交媒体传播。'),
('专业限制反而能制造身份','Q3 Monochrom 只拍黑白、Kitsune 删除摇杆、Ode 不做意式研磨。主动限制功能能清楚表达“为谁而做”，让购买成为价值观选择。'),
('围绕核心物件构建可持续扩展的舞台','Nanoleaf 面板、咖啡工具、机箱屏幕和摄影附件都允许用户持续添置。高溢价产品往往不是孤品，而是一个逐步完成、永远可以继续优化的个人系统。'),
('动作是最便宜也最有效的奢侈感载体','旋钮阻尼、磁吸定位、拨杆行程、转盘惯性、曲柄反馈不需要大屏，却能让用户每次使用都重新确认价值。'),
('产品要给爱好者留下“可讲述参数”','83mm 刀盘、60MP 黑白传感器、2.5K 转角屏、32 个 LCD 键等参数既是技术事实，也是用户向同好解释购买的社交货币。'),
]
for i,(h,d) in enumerate(insights,1):
    p=doc.add_paragraph(style='Heading 2'); p.add_run(f'{i}. {h}')
    doc.add_paragraph(d)

doc.add_heading('面向新产品概念的机会检查表',2)
checks=['目标人群是否拥有可被命名的深度兴趣与既有术语体系？','产品在 3 秒视频中是否有一个无需解释的视觉/动态钩子？','是否存在一个每天都会重复、并可被仪式化的关键动作？','高价是否有材料、结构、性能或寿命方面的“可触摸证据”？','是否提供足够多的参数、配置或配件让用户继续研究？','是否能在桌面、房间、工作台或社交照片中成为视觉中心？','是否有明确的“不为所有人服务”的专业立场？','是否能形成主产品 + 配件 + 软件 + 内容/社区的复利生态？']
for x in checks:
    p=doc.add_paragraph(style='List Bullet'); p.add_run(x)

doc.add_heading('4. 方法、限制与来源',1)
doc.add_heading('方法说明',2)
doc.add_paragraph('样本采用目的性抽样：先从“技术深度、设计显著、效果可展示、价格明显高于基础替代品”四个维度识别候选，再跨场景平衡，避免样本被耳机、键盘或灯光单一品类占满。公开信息优先采用品牌官方产品页、官方商店或官方发布稿。')
doc.add_heading('重要限制',2)
doc.add_paragraph('本报告是机会扫描，不是严格的价格指数或销量排名。价格可能因地区、税费、配置、促销、停产和汇率变化；“高溢价”与“溢价强度”为研究判断，不代表品牌披露的成本或毛利。建议在立项前对重点候选再次核验中国市场到手价、销量、二手保值率与用户评论。')
doc.add_heading('主要公开来源（研究时点访问）',2)
for label,url in sources:
    p=doc.add_paragraph(style='List Bullet'); add_link(p,label,url,9)

doc.add_heading('附：优先值得进一步拆解的 8 个标杆',2)
doc.add_paragraph('若后续要为电子乐器、桌面设备或智能硬件做概念启发，建议优先深拆：Teenage Engineering TP–7（机械动作数字化）、HYTE Y70 touch infinite（硬件展柜化）、Nanoleaf Shapes（模块扩展）、B&O Beosound A9（家具化）、Leica Q3 Monochrom（专业限制）、Weber EG-1（精密机械戏剧）、Playdate（单一交互钩子）、Garmin Tacx NEO 3M（身体反馈）。')

props=doc.core_properties; props.title='面向技术宅与深度爱好者的高溢价酷炫产品研究'; props.subject='50件跨品类产品市场扫描'; props.author='OpenAI Codex'; props.keywords='高溢价, 技术宅, 发烧友, 设计, 酷炫效果, 市场研究'
doc.save(OUT)
print(OUT)
