import fs from 'node:fs';
import path from 'node:path';

const base = path.join(process.cwd(), 'hardware', 'rev_a');
const files = fs.readdirSync(base).filter((f) => f.endsWith('.sch'));
const rows = [];
for (const file of files) {
  const text = fs.readFileSync(path.join(base, file), 'utf8');
  for (const match of text.matchAll(/\$Comp\n([\s\S]*?)\$EndComp/g)) {
    const block = match[1];
    const field = (n) => block.match(new RegExp(`^F ${n} "([^"]*)"`, 'm'))?.[1] ?? '';
    const ref = field(0), value = field(1), footprint = field(2), lcsc = field(4);
    if (ref) rows.push({board:file.startsWith('PISCIS-I_Control')?'CONTROL':'MAIN',sheet:file,ref,value,footprint,lcsc});
  }
}
rows.sort((a,b)=>a.board.localeCompare(b.board)||a.ref.localeCompare(b.ref,undefined,{numeric:true}));
const header = ['Board','Sheet','Reference','Value','Footprint','LCSC Part'];
const quote = (v) => `"${String(v).replaceAll('"','""')}"`;
const csv = [header, ...rows.map((r)=>[r.board,r.sheet,r.ref,r.value,r.footprint,r.lcsc])].map((r)=>r.map(quote).join(',')).join('\n')+'\n';
fs.writeFileSync(path.join(base,'BOM_RevA.csv'),csv);
console.log(`Wrote ${rows.length} BOM rows (${rows.filter(r=>r.board==='MAIN').length} main, ${rows.filter(r=>r.board==='CONTROL').length} control).`);
