from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_CELL_VERTICAL_ALIGNMENT
from docx.enum.section import WD_SECTION
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.enum.style import WD_STYLE_TYPE
from docx.enum.text import WD_BREAK
from math import sqrt

OUT = "电子钢琴市场调研与小样本统计实务指南.docx"
BLUE = "24557A"; DARK = "17324D"; LIGHT = "EAF1F7"; GRAY = "5C6773"; PALE = "F4F6F8"

def font(run, size=10.5, bold=False, color="222222", name="Arial Unicode MS"):
    run.font.name=name; rfonts=run._element.get_or_add_rPr().rFonts
    for key in ('ascii','hAnsi','eastAsia','cs'): rfonts.set(qn('w:'+key),'Arial Unicode MS')
    run.font.size=Pt(size); run.bold=bold; run.font.color.rgb=RGBColor.from_string(color)

def shade(cell, fill):
    tcPr=cell._tc.get_or_add_tcPr(); shd=tcPr.find(qn('w:shd'))
    if shd is None: shd=OxmlElement('w:shd'); tcPr.append(shd)
    shd.set(qn('w:fill'),fill)

def cell_margins(cell, top=90, start=120, bottom=90, end=120):
    tc=cell._tc; tcPr=tc.get_or_add_tcPr(); tcMar=tcPr.first_child_found_in('w:tcMar')
    if tcMar is None: tcMar=OxmlElement('w:tcMar'); tcPr.append(tcMar)
    for m,v in [('top',top),('start',start),('bottom',bottom),('end',end)]:
        e=tcMar.find(qn('w:'+m))
        if e is None: e=OxmlElement('w:'+m); tcMar.append(e)
        e.set(qn('w:w'),str(v)); e.set(qn('w:type'),'dxa')

def set_repeat_header(row):
    trPr=row._tr.get_or_add_trPr(); e=OxmlElement('w:tblHeader'); e.set(qn('w:val'),'true'); trPr.append(e)

def table(doc, headers, rows, widths):
    t=doc.add_table(rows=1, cols=len(headers)); t.alignment=WD_TABLE_ALIGNMENT.CENTER; t.autofit=False
    tPr=t._tbl.tblPr; tblW=tPr.find(qn('w:tblW')); tblW.set(qn('w:w'),'9360'); tblW.set(qn('w:type'),'dxa')
    ind=OxmlElement('w:tblInd'); ind.set(qn('w:w'),'120'); ind.set(qn('w:type'),'dxa'); tPr.append(ind)
    grid=t._tbl.tblGrid
    for child in list(grid): grid.remove(child)
    for w in widths:
        gc=OxmlElement('w:gridCol'); gc.set(qn('w:w'),str(w)); grid.append(gc)
    for j,h in enumerate(headers):
        c=t.rows[0].cells[j]; c.width=Inches(widths[j]/1440); shade(c,BLUE); cell_margins(c)
        p=c.paragraphs[0]; p.alignment=WD_ALIGN_PARAGRAPH.CENTER; font(p.add_run(h),9.2,True,"FFFFFF")
    set_repeat_header(t.rows[0])
    for i,row in enumerate(rows):
        cells=t.add_row().cells
        for j,val in enumerate(row):
            c=cells[j]; c.width=Inches(widths[j]/1440); c.vertical_alignment=WD_CELL_VERTICAL_ALIGNMENT.CENTER; cell_margins(c)
            if i%2: shade(c,PALE)
            p=c.paragraphs[0]; p.paragraph_format.space_after=Pt(0); p.alignment=WD_ALIGN_PARAGRAPH.CENTER if j==0 else WD_ALIGN_PARAGRAPH.LEFT
            font(p.add_run(str(val)),9.1,False)
    doc.add_paragraph().paragraph_format.space_after=Pt(1)
    return t

def bullet(doc, text, style='List Bullet'):
    p=doc.add_paragraph(style=style); p.paragraph_format.space_after=Pt(3); font(p.add_run(text),10.3); return p

def para(doc, text, boldlead=None):
    p=doc.add_paragraph(); p.paragraph_format.space_after=Pt(6); p.paragraph_format.line_spacing=1.18
    if boldlead and text.startswith(boldlead):
        font(p.add_run(boldlead),10.5,True,DARK); font(p.add_run(text[len(boldlead):]),10.5)
    else: font(p.add_run(text),10.5)
    return p

def callout(doc, title, text):
    t=doc.add_table(rows=1,cols=1); t.alignment=WD_TABLE_ALIGNMENT.CENTER; t.autofit=False
    c=t.cell(0,0); shade(c,LIGHT); cell_margins(c,150,180,150,180)
    p=c.paragraphs[0]; p.paragraph_format.space_after=Pt(4); font(p.add_run(title),11,True,BLUE)
    p=c.add_paragraph(); p.paragraph_format.space_after=Pt(0); font(p.add_run(text),10.2,False,DARK)
    doc.add_paragraph().paragraph_format.space_after=Pt(1)

def heading(doc, text, level=1):
    p=doc.add_paragraph(text,style=f'Heading {level}'); return p

doc=Document(); sec=doc.sections[0]
sec.page_width=Inches(8.5); sec.page_height=Inches(11); sec.top_margin=sec.bottom_margin=sec.left_margin=sec.right_margin=Inches(1)
sec.header_distance=sec.footer_distance=Inches(.492)
styles=doc.styles
normal=styles['Normal']; normal.font.name='Arial Unicode MS';
for key in ('ascii','hAnsi','eastAsia','cs'): normal._element.rPr.rFonts.set(qn('w:'+key),'Arial Unicode MS')
normal.font.size=Pt(10.5)
normal.paragraph_format.space_after=Pt(6); normal.paragraph_format.line_spacing=1.18
for name,size,before,after,color in [('Heading 1',16,16,8,BLUE),('Heading 2',13,12,6,BLUE),('Heading 3',11.5,8,4,DARK)]:
    s=styles[name]; s.font.name='Arial Unicode MS'
    for key in ('ascii','hAnsi','eastAsia','cs'): s._element.rPr.rFonts.set(qn('w:'+key),'Arial Unicode MS')
    s.font.size=Pt(size); s.font.bold=True; s.font.color.rgb=RGBColor.from_string(color); s.paragraph_format.space_before=Pt(before); s.paragraph_format.space_after=Pt(after); s.paragraph_format.keep_with_next=True
for name in ['List Bullet','List Number']:
    s=styles[name]; s.font.name='Arial Unicode MS'
    for key in ('ascii','hAnsi','eastAsia','cs'): s._element.rPr.rFonts.set(qn('w:'+key),'Arial Unicode MS')
    s.font.size=Pt(10.3); s.paragraph_format.left_indent=Inches(.5); s.paragraph_format.first_line_indent=Inches(-.25); s.paragraph_format.space_after=Pt(4); s.paragraph_format.line_spacing=1.167

# Header/footer
hp=sec.header.paragraphs[0]; font(hp.add_run('PISCIS-I 电子钢琴项目｜市场调研方法指南'),8.5,False,GRAY)
fp=sec.footer.paragraphs[0]; fp.alignment=WD_ALIGN_PARAGRAPH.RIGHT
font(fp.add_run('第 '),8.5,False,GRAY); fld=OxmlElement('w:fldSimple'); fld.set(qn('w:instr'),'PAGE'); fp._p.append(fld)

# Editorial cover
p=doc.add_paragraph(); p.paragraph_format.space_before=Pt(90); p.alignment=WD_ALIGN_PARAGRAPH.CENTER; font(p.add_run('市场调研与小样本统计'),25,True,DARK)
p=doc.add_paragraph(); p.alignment=WD_ALIGN_PARAGRAPH.CENTER; p.paragraph_format.space_after=Pt(18); font(p.add_run('电子钢琴项目实务指南'),17,False,BLUE)
p=doc.add_paragraph(); p.alignment=WD_ALIGN_PARAGRAPH.CENTER; font(p.add_run('从问题定义、抽样与访谈，到可信区间、精确检验和决策表达'),11,False,GRAY)
p=doc.add_paragraph(); p.alignment=WD_ALIGN_PARAGRAPH.CENTER; p.paragraph_format.space_before=Pt(26); font(p.add_run('版本：2026年8月｜适用于资源有限的探索性与验证性调研'),9.5,False,GRAY)
callout(doc,'核心原则','小样本不等于“不可信”，但它要求更好的抽样、更少且更明确的问题、更透明的不确定性，以及更克制的结论。统计方法无法补救严重的样本偏差。')
doc.add_page_break()

heading(doc,'执行摘要',1)
para(doc,'本指南用于支持电子钢琴产品定位、功能优先级、价格、渠道与目标用户选择。推荐采用“定性探索 → 小规模定量验证 → 行为数据复核”的三角验证路径，并把每一项统计分析绑定到一个具体商业决策。')
heading(doc,'在资源有限时，优先做对这六件事',2)
for x in ['定义清楚目标总体与决策问题，不把“方便找到的人”误当成整个市场。','用分层配额覆盖关键人群；宁可每个关键层有少量真实样本，也不要大量同质样本。','减少核心指标数量，预先确定主问题、主指标和最小有意义差异。','优先报告点估计、效应大小和区间，不把 p<0.05 当作唯一结论。','对比例使用 Wilson 或精确二项区间；对小列联表使用 Fisher 精确检验。','用访谈、问卷、评论/销量和小型行为实验相互验证；不一致时先解释差异。']: bullet(doc,x)
callout(doc,'一句话策略','把有限样本集中到最关键的用户层和最关键的决策上；用区间表达“可能范围”，用重复证据表达“方向稳定性”。')

heading(doc,'1. 从商业决策开始，而不是从问卷开始',1)
para(doc,'先写出这次调研结束后必须做出的决定。例如：选择入门型还是便携型定位；是否保留88键重锤；首发价位；优先进入电商、琴行或培训机构。')
table(doc,['决策','可验证假设','核心指标','建议证据'],[
('目标人群','租房青年比家庭用户更重视便携与耳机练习','功能重要性差、使用场景占比','访谈＋分层问卷'),
('产品配置','手感对购买意愿的影响高于附加智能功能','效应大小、选择率','概念测试＋选择实验'),
('价格','目标用户可接受价格集中在某一区间','价格阈值、购买概率','价格敏感度＋真实点击/订金'),
('渠道','教师/琴行推荐显著影响初学者决策','来源占比、转化差异','问卷＋渠道访谈＋行为数据')], [1500,2800,2200,2860])
heading(doc,'把假设写成可否证形式',2)
para(doc,'不建议：“用户喜欢便携电子钢琴。”建议：“在未来12个月有购买计划的租房用户中，至少60%将便携性评为4或5分，且该比例高于有固定琴房用户至少15个百分点。”')

heading(doc,'2. 研究设计：三角验证',1)
table(doc,['阶段','建议规模','目标','输出'],[
('定性探索','12–20名用户＋3–5名从业者','发现语言、场景、痛点与未知因素','假设、问卷选项、用户旅程'),
('定量验证','50–150份有效样本','估计方向、区间与群体差异','比例/均值区间、效应大小'),
('行为复核','2–4个概念或页面；每版尽量≥30次有效行为','验证“说”与“做”是否一致','点击、留资、预约、订金率'),
('二次证据','10–15款竞品、数百条评论可编码抽样','校准产品与市场语境','功能/价格矩阵、主题频率')], [1500,1800,3000,3060])
para(doc,'定性样本的停止标准可以采用“主题趋于饱和”：连续若干次访谈不再出现新的关键痛点或购买机制。它用于发现机制，不用于估计市场比例。')

heading(doc,'3. 小样本最关键的是抽样，而不是算法',1)
heading(doc,'3.1 明确总体、抽样框与入选条件',2)
for x in ['目标总体示例：未来12个月计划购买2,000–10,000元电子钢琴的中国城市消费者。','抽样单位：个人，而不是一次点击或一条评论；同一人重复回答必须去重。','提前记录排除规则：答题时长异常、逻辑冲突、重复设备、未通过筛选题。','记录来源渠道；不同渠道的用户结构可能完全不同，不应无权重混合。']: bullet(doc,x)
heading(doc,'3.2 使用分层配额',2)
para(doc,'先依据最可能影响决策的2–3个变量分层，例如专业程度（初学/进阶）、居住形态（租房/固定住房）、购买阶段（已购/计划购买）。层数不要过多，否则每格样本过少。')
table(doc,['人群层','最低探索配额','重点问题'],[
('初学者/家长','20–30','价格、教学支持、品牌信任'),('进阶用户','15–25','键感、音色、扩展性'),('租房/空间受限','15–25','尺寸、重量、耳机、收纳'),('教师/琴行从业者','5–10（定性为主）','推荐逻辑、退换货、售后')],[2200,2200,4960])
callout(doc,'不要把配额样本伪装成概率样本','便利抽样加配额能改善覆盖，但通常仍不能严格代表总体。报告中应写清招募渠道，并把总体外推描述为“方向性证据”或“在本样本中”。')

heading(doc,'4. 样本量与精度：先问“要多精确”',1)
para(doc,'估计比例时，最保守情形 p=0.5 下，95%近似误差界约为 1.96√[0.25/n]。这不是样本质量保证，只描述简单随机抽样假设下的抽样误差。')
rows=[]
for n in [30,50,80,100,150,200]: rows.append((n,f'±{1.96*sqrt(.25/n)*100:.1f}个百分点','适合方向判断' if n<80 else ('可做有限分组' if n<150 else '可做少量预设比较')))
table(doc,['有效样本 n','最保守95%误差界','合理用途'],rows,[1800,2600,4960])
para(doc,'若总体规模 N 已知且抽样比例 n/N 较高，可使用有限总体修正：标准误乘以 √[(N−n)/(N−1)]。它只适用于接近无放回概率抽样的情形，不能用来修正便利样本偏差。')
heading(doc,'资源分配建议',2)
for x in ['只有30–50份：不做复杂用户聚类；聚焦2–3个主指标，给出区间，结合访谈与行为证据。','约80–120份：可做少量预设的两组比较，但避免把样本切成很多小格。','约150–200份：可做有限的回归或分层比较；预测变量数量仍需克制，并做稳健性检查。']: bullet(doc,x)

heading(doc,'5. 小样本统计工具箱',1)
table(doc,['问题','首选方法','为什么','注意事项'],[
('一个比例是多少','Wilson 95%区间；极小计数可报精确二项区间','比普通 Wald 区间在小 n 或比例靠近0/1时更可靠','同时报告 x/n，不只报百分比'),
('两类人的选择率是否不同','Fisher精确检验＋比例差区间','小单元格时不依赖卡方近似','报告差值与区间，不只报p值'),
('两组评分是否不同','Welch t检验＋均值差区间；严重偏态可用置换检验','Welch不要求等方差；置换法假设透明','Likert单题宜同时报分布/中位数'),
('同一人比较两个概念','配对t检验或符号/置换检验','利用个体内比较降低噪声','随机化展示顺序，防顺序效应'),
('复杂统计量的不确定性','Bootstrap BCa或置换法','可减少对解析分布公式的依赖','不能创造新信息；极小n与极端分布要谨慎'),
('希望结合历史/专家知识','贝叶斯模型＋信息性先验＋敏感性分析','可显式整合既有证据','默认宽先验不一定改善小样本；必须解释先验')],[1600,2200,2500,3060])
heading(doc,'5.1 为什么优先报告区间和效应大小',2)
para(doc,'小样本经常出现“差异看起来大但未显著”，或“p值跨过0.05”。应报告差异有多大、可信范围有多宽，以及该范围是否包含商业上重要的效果。美国统计协会强调，p值不衡量效果大小或商业重要性，也不能单独作为证据强度。')
heading(doc,'5.2 Bootstrap 的正确定位',2)
para(doc,'Bootstrap 是从现有样本重复有放回抽样，用来近似统计量的抽样分布。它适合均值差、回归系数等解析区间不方便的统计量，但原始样本若未覆盖总体的重要类型，重复抽样也不会补出这些类型。小样本中，普通百分位区间可能偏窄；应优先考虑 BCa、studentized/Bootstrap-t 或与置换检验交叉核对。')
heading(doc,'5.3 贝叶斯方法',2)
para(doc,'贝叶斯方法可将历史研究、既有销售数据或专家知识编码为先验，并输出“某效果超过商业阈值的概率”。小样本时结果对先验敏感，因此至少报告弱信息先验与更保守先验下的敏感性分析；不要把“用了贝叶斯”当作自动可靠。')

heading(doc,'6. 问卷与测量设计',1)
for x in ['先预测试5–10人，要求受访者边答边解释，检查术语和选项是否被正确理解。','一题只测一个概念；“音色和手感是否满意”必须拆开。','态度题采用一致的5或7点量表，并完整展示端点含义。','优先问最近一次真实行为，再问未来意愿；把购买意愿与点击、留资或订金数据区分。','随机化概念/功能展示顺序；功能列表过长时采用分组或选择任务。','记录“不知道/未体验”，不要强迫受访者给虚假评分。']: bullet(doc,x)
heading(doc,'建议的核心变量',2)
table(doc,['模块','变量','建议编码'],[
('筛选','购买时间窗口、预算、决策参与度','分类变量'),('背景','琴龄、专业程度、住房、城市、收入','预设分组'),('行为','练习频率、场景、渠道、最近购买','次数/分类'),('需求','手感、音色、便携、连接、教学','1–5重要性＋强制排序'),('概念','购买意愿、首选方案、放弃原因','量表＋单选＋开放题'),('价格','便宜/划算/贵/太贵阈值','连续金额')],[1600,3000,4760])

heading(doc,'7. 分析流程与可信度检查',1)
for x in ['数据清理：依据预先规则去重、排除无效答卷；保留清理日志。','描述样本：报告各层人数、来源、缺失与排除数量。','主分析：只先分析预设主指标；报告 x/n、均值/中位数、效应大小和95%区间。','分层检查：查看结论在关键层与招募渠道中方向是否一致。','稳健性分析：更换合理方法（如 Wilson/精确区间、Welch/置换），观察结论是否改变。','三角验证：将问卷结论与访谈主题、竞品数据、行为实验对照。','结论分级：稳定证据、方向性证据、证据不足；为每类给出下一步行动。']: bullet(doc,x, 'List Number')
callout(doc,'停止规则','若区间同时包含“值得做”和“不值得做”的效果，不应硬下结论。下一步可以追加样本、提高测量精度、采用配对设计，或把决策改成小范围试点。')

heading(doc,'8. 电子钢琴项目建议方案',1)
table(doc,['阶段','具体执行','判断标准'],[
('第1周：二手研究','整理10–15款竞品的价格、键盘、重量、音色、连接与评论主题','形成竞品矩阵与10条待验证假设'),
('第2周：访谈','12–15名目标用户＋5名教师/销售；覆盖初学、进阶、租房','连续访谈新增关键主题显著减少'),
('第3周：预测试','10人认知访谈；修订问卷与2–3个概念','题意一致、完成时间≤10分钟'),
('第4–5周：问卷','目标80–120份；按核心用户层设配额并记录渠道','各关键层达到最低配额；无单一渠道垄断'),
('第5周：行为测试','2个落地页或概念；观察点击/留资/预约','方向与问卷一致，或能解释不一致'),
('第6周：决策报告','主指标区间、效应大小、限制、行动建议','每项建议均可追溯到至少两类证据')],[1500,5060,2800])
heading(doc,'建议只保留五个主决策指标',2)
for x in ['目标人群中概念首选率及 Wilson 95%区间。','相对现有产品的购买意愿差值及区间。','关键功能的强制选择率，而不只是“都很重要”的评分。','可接受价格区间与不同价格下的购买概率。','点击、留资、预约或小额订金等行为转化指标。']: bullet(doc,x)

heading(doc,'9. 结果表达模板',1)
callout(doc,'推荐句式','在本次通过【渠道】招募的 n=【】名目标用户中，【x/n】选择方案A，Wilson 95%区间为【L%, U%】。相对方案B的差异为【d】个百分点（95%区间【】）。该方向在【关键层】中一致/不一致。由于【样本来源与规模限制】，本结果用于【方向判断/试点决策】，不用于精确推断全国市场占比。')
heading(doc,'证据等级',2)
table(doc,['等级','判定','行动'],[
('A 稳定','多种证据一致；区间不跨越商业阈值；关键层方向稳定','可进入产品/渠道试点'),('B 方向性','方向大体一致，但区间较宽或部分层不一致','小范围验证，避免大额承诺'),('C 不足','区间覆盖相反决策；样本偏差明显；证据冲突','追加针对性样本或重新设计测量')],[1500,5060,2800])

heading(doc,'10. 常见误区',1)
for x in ['“样本达到30就自动服从正态”——错误；还取决于变量分布、统计量与异常值。','“Bootstrap把30份变成10,000份”——错误；它只是重复利用同30份的信息。','“不显著就没有差异”——错误；也可能是区间太宽、检验力不足。','“显著就值得做”——错误；必须看效果大小、成本与商业阈值。','“配额等于随机抽样”——错误；它改善结构，但未消除自选择与渠道偏差。','“复杂回归能控制一切”——错误；小样本中过多变量会造成不稳定与过拟合。']: bullet(doc,x)

heading(doc,'11. 参考文献与延伸阅读',1)
refs=[
('Cochran, W. G. (1977). Sampling Techniques (3rd ed.). Wiley.','抽样设计、分层抽样、有限总体修正的经典教材。','https://books.google.com/books/about/Sampling_Techniques.html?id=8Y4QAQAAIAAJ'),
('NIST/SEMATECH. Confidence intervals for a binomial proportion.','介绍 Wilson、Agresti–Coull 与小样本精确二项区间。','https://itl.nist.gov/div898/handbook/prc/section2/prc241.htm'),
('Brown, L. D., Cai, T. T., & DasGupta, A. (2001). Interval Estimation for a Binomial Proportion. Statistical Science, 16(2), 101–133.','系统比较比例区间的覆盖性质，支持避免简单 Wald 区间。','https://doi.org/10.1214/ss/1009213286'),
('Agresti, A., & Coull, B. A. (1998). Approximate Is Better than “Exact” for Interval Estimation of Binomial Proportions. The American Statistician, 52(2), 119–126.','比例区间的实用替代方案。','https://doi.org/10.1080/00031305.1998.10480550'),
('Efron, B., & Tibshirani, R. J. (1993). An Introduction to the Bootstrap. Chapman & Hall/CRC.','Bootstrap 的经典系统教材。','https://doi.org/10.1201/9780429246593'),
('Horowitz, J. L. (2019). Bootstrap Methods in Econometrics. Annual Review of Economics, 11, 193–224.','Bootstrap 的用途、改进与局限。','https://doi.org/10.1146/annurev-economics-080218-025651'),
('Wood, M. (2005/2016 PMC version). What Teachers Should Know About the Bootstrap.','指出普通百分位 Bootstrap 在小样本中的限制。','https://pmc.ncbi.nlm.nih.gov/articles/PMC4784504/'),
('Wasserstein, R. L., & Lazar, N. A. (2016). The ASA Statement on p-Values. The American Statistician, 70(2), 129–133.','解释为何不能只凭 p 值决策。','https://doi.org/10.1080/00031305.2016.1154108'),
('Schoot et al./Smid et al. (2020). Bayesian Versus Frequentist Estimation for SEM in Small Samples: A Systematic Review.','说明小样本贝叶斯方法依赖合理的信息性先验，默认宽先验可能产生偏差。','https://doi.org/10.1080/10705511.2019.1577140'),
('WHO STEPS Surveillance Manual.','包含调查抽样、样本量与有限总体修正等实务材料。','https://www.who.int/docs/default-source/ncds/ncd-surveillance/steps/steps-manual.pdf')]
for i,(cite,note,url) in enumerate(refs,1):
    p=doc.add_paragraph(); p.paragraph_format.left_indent=Inches(.22); p.paragraph_format.first_line_indent=Inches(-.22); p.paragraph_format.space_after=Pt(7)
    font(p.add_run(f'{i}. {cite} '),9.5,True,DARK); font(p.add_run(note+' '),9.5,False)
    font(p.add_run(url),8.5,False,BLUE)

doc.add_page_break()
heading(doc,'附录：最小可行分析清单',1)
for x in ['已写明总体、样本来源、入选与排除规则。','已设关键分层配额并报告每层实际 n。','已预先指定不超过3–5个主指标。','比例采用 Wilson/精确区间；小列联表采用 Fisher 精确检验。','报告效应大小、区间、原始分子/分母，不只报告 p 值。','对 Bootstrap 与贝叶斯结论做限制或敏感性说明。','至少两类证据相互验证。','报告不能外推的对象和下一步追加验证条件。']: bullet(doc,'☐ '+x)

doc.core_properties.title='市场调研与小样本统计：电子钢琴项目实务指南'
doc.core_properties.subject='市场调研、抽样、小样本统计、电子钢琴'
doc.core_properties.author='OpenAI Codex'
doc.save(OUT)
print(OUT)
