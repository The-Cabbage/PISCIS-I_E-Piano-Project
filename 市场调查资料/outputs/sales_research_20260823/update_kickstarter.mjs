import fs from "node:fs/promises";
import { FileBlob, SpreadsheetFile } from "@oai/artifact-tool";

const root = "/Users/admin/Desktop/Code/PISCIS-I_E-Piano_Project/市场调查资料";
const path = `${root}/outputs/sales_research_20260823/竞品销售与宣发情况.xlsx`;
const input = await FileBlob.load(path);
const wb = await SpreadsheetFile.importXlsx(input);
const ks = wb.worksheets.add("Kickstarter Pipeline");
const navy="#14213D", blue="#2F6BFF", paleBlue="#EAF1FF", paleYellow="#FFF4CC", light="#D0D5DD";
const rows = [
 ["WonderSynth","六旋钮合成器 / GeekStyle Toy","Pledge Manager","USD",32794,15000,88,"2026-06-02","2026-07-02","2026-08-16","首批目标 2026-08-31","https://www.kickstarter.com/projects/wonderworksdsgn/wondersynth-the-synthesizer-simplified"],
 ["Tembo","家庭化游戏乐器","官网预购 / 履约","USD",2165315,50000,5347,"2026-03-11","2026-04-21","2026-07-27","以预购页/更新为准","https://www.kickstarter.com/projects/musicalbeings/tembo-a-new-musical-instrument-for-playful-music-making"],
 ["Melo-D","生成式 AI 吉他","官网预购 / 履约","HKD",1482369,40000,461,"2026-05-19","2026-07-06","2026-08-04","以官网/更新为准","https://www.kickstarter.com/projects/1676689999/melo-d-play-from-the-first-touch-worlds-first-ai-guitar/"],
 ["Banan-a-Synth","导电触控口袋合成器 / STEM","生产履约","USD",117964,4011,1580,"2026-04-08","2026-05-08","2026-07-31","早鸟 2026-12；常规 2027-02","https://www.kickstarter.com/projects/microkits/bananasynth"],
 ["Drumboy Pro","便携鼓机 / 开源硬件","生产履约","USD",192717,1000,578,"2026-03-15","2026-04-24","2026-08-08","原计划 2026-08","https://www.kickstarter.com/projects/randomwaves/drumboy-pro-beatmaking-redefined"],
 ["EH-01","紧凑实验采样合成器","小批量生产","AUD",43595,22000,143,"2026-06-23","2026-07-28","2026-08-11","以项目更新为准","https://www.kickstarter.com/projects/1397757174/eh-01-a-compact-experimental-sampler-synth"],
 ["Nérée","单板触觉合成器工作站","已进入生产","EUR",5840,3125,37,"2026-03-05","2026-03-30","2026-07-28","以项目更新为准","https://www.kickstarter.com/projects/neree/neree-sound-shaped-on-a-single-intuitive-board"]
];
ks.showGridLines=false;
ks.getRange("A1:N2").merge(); ks.getRange("A1").values=[["Kickstarter — Portable / AI Music Hardware Pipeline"]];
ks.getRange("A1:N2").format={fill:navy,font:{color:"#FFFFFF",bold:true,size:19},verticalAlignment:"center"};
ks.getRange("A3:N3").merge(); ks.getRange("A3").values=[["Snapshot 2026-08-23 · No verified highly relevant live campaign; rows are Pledge Manager, post-campaign preorder, or fulfillment-stage projects. Currencies are not converted."]];
ks.getRange("A3:N3").format={fill:paleBlue,font:{color:navy,italic:true},wrapText:true};
ks.getRange("A5:N5").values=[["Project","Product Type","Lifecycle Status","Currency","Pledged","Goal","Backers","Funding Start","Funding End","Last Update","Delivery / Fulfillment","Project URL","Funding %","Avg Pledge"]];
ks.getRange("A5:N5").format={fill:blue,font:{color:"#FFFFFF",bold:true},wrapText:true};
ks.getRange("A6:L12").values=rows;
ks.getRange("M6").formulas=[["=IFERROR(E6/F6,\"\")"]]; ks.getRange("M6:M12").fillDown();
ks.getRange("N6").formulas=[["=IFERROR(E6/G6,\"\")"]]; ks.getRange("N6:N12").fillDown();
ks.getRange("A6:N12").format={verticalAlignment:"top",wrapText:true,borders:{insideHorizontal:{style:"thin",color:light}}};
ks.getRange("D6:D12").format={fill:paleYellow,font:{bold:true}};
ks.getRange("E6:G12").format.numberFormat="#,##0"; ks.getRange("M6:M12").format.numberFormat="0.0%"; ks.getRange("N6:N12").format.numberFormat="#,##0.00";
ks.getRange("A14:N15").merge(); ks.getRange("A14").values=[["Average pledge = pledged ÷ backers; it is not unit price. Funding % can be inflated by a deliberately low goal. Pledged totals are not net revenue and exclude later preorder sales."]];
ks.getRange("A14:N15").format={fill:paleYellow,font:{color:"#7A4E00"},wrapText:true,verticalAlignment:"center"};
const widths=[22,28,23,11,16,15,12,15,15,15,29,46,14,16];
for(let i=0;i<widths.length;i++) ks.getRangeByIndexes(0,i,15,1).format.columnWidth=widths[i];
ks.getRange("5:5").format.rowHeight=38; ks.getRange("6:12").format.rowHeight=52; ks.freezePanes.freezeRows(5); ks.freezePanes.freezeColumns(1);
const check=await wb.inspect({kind:"match",searchTerm:"#REF!|#DIV/0!|#VALUE!|#NAME\\?|#N/A",options:{useRegex:true,maxResults:100},summary:"formula error scan"}); console.log(check.ndjson);
const preview=await wb.render({sheetName:"Kickstarter Pipeline",range:"A1:N15",scale:1.1,format:"png"});
await fs.writeFile(`${root}/outputs/sales_research_20260823/preview_Kickstarter_Pipeline.png`,new Uint8Array(await preview.arrayBuffer()));
const out=await SpreadsheetFile.exportXlsx(wb); await out.save(path); console.log(path);
