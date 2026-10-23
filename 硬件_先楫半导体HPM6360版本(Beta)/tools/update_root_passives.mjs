import fs from 'node:fs';
import path from 'node:path';

const schematic = path.join(process.cwd(), 'PISCIS-I_HPM6360_Module.kicad_sch');

const resistor = (value, mpn) => ({
  value,
  footprint: 'Resistor_SMD:R_0603_1608Metric',
  manufacturer: 'Vishay',
  mpn,
  rating: '0603, 0.1 W, 1%, thick film',
  datasheet: 'https://www.vishay.com/docs/20035/dcrcwe3.pdf',
});

const capacitor = (value, footprint, mpn, rating) => ({
  value,
  footprint,
  manufacturer: 'Murata',
  mpn,
  rating,
  datasheet: `https://www.murata.com/en-global/products/productdetail?partno=${encodeURIComponent(mpn)}`,
});

const parts = {};

for (const ref of ['R1', 'R2', 'R4']) parts[ref] = resistor('100k', 'CRCW0603100KFKEA');
parts.R3 = resistor('22.1k', 'CRCW060322K1FKEA');
parts.R5 = resistor('2M', 'CRCW06032M00FKEA');
parts.R6 = resistor('33R', 'CRCW060333R0FKEA');
for (const ref of ['R7', 'R8', 'R9', 'R10', 'R11', 'R12', 'R13', 'R18', 'R19', 'R20']) {
  parts[ref] = resistor('10k', 'CRCW060310K0FKEA');
}
for (const ref of ['R14', 'R15']) parts[ref] = resistor('33R', 'CRCW060333R0FKEA');
for (const ref of ['R16', 'R17', 'R21', 'R22']) parts[ref] = resistor('5.1k', 'CRCW06035K10FKEA');

const c100n = (ref) => {
  parts[ref] = capacitor(
    '100nF',
    'Capacitor_SMD:C_0603_1608Metric',
    'GRM188R71H104KA93D',
    '100 nF, 50 V, X7R, 10%, 0603',
  );
};
const c1u = (ref) => {
  parts[ref] = capacitor(
    '1uF',
    'Capacitor_SMD:C_0603_1608Metric',
    'GRM188R71C105KA12D',
    '1 uF, 16 V, X7R, 10%, 0603',
  );
};
const c4u7 = (ref) => {
  parts[ref] = capacitor(
    '4.7uF',
    'Capacitor_SMD:C_0603_1608Metric',
    'GRM188R61A475KE15D',
    '4.7 uF, 10 V, X5R, 10%, 0603',
  );
};
const c10u = (ref) => {
  parts[ref] = capacitor(
    '10uF',
    'Capacitor_SMD:C_0805_2012Metric',
    'GRM21BR61A106KE19L',
    '10 uF, 10 V, X5R, 10%, 0805',
  );
};
const c22u = (ref) => {
  parts[ref] = capacitor(
    '22uF',
    'Capacitor_SMD:C_1206_3216Metric',
    'GRM31CR61C226ME15L',
    '22 uF, 16 V, X5R, 20%, 1206; sized for DC-bias margin',
  );
};
const cC0G = (ref, value, mpn) => {
  parts[ref] = capacitor(
    value,
    'Capacitor_SMD:C_0603_1608Metric',
    mpn,
    `${value.replace('pF', ' pF')}, 50 V, C0G/NP0, 5%, 0603`,
  );
};

for (const ref of ['C1', 'C2', 'C23', 'C24', 'C37']) c22u(ref);
for (const ref of ['C27', 'C28']) c10u(ref);
for (const ref of [
  'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9', 'C10', 'C11', 'C12', 'C13', 'C14',
  'C17', 'C18', 'C20', 'C25', 'C26', 'C29', 'C30', 'C32', 'C36', 'C38', 'C39',
]) c100n(ref);
for (const ref of ['C19', 'C21', 'C33']) c1u(ref);
for (const ref of ['C15', 'C16', 'C31', 'C40']) c4u7(ref);
cC0G('C22', '22pF', 'GRM1885C1H220JA01D');
cC0G('C34', '12pF', 'GRM1885C1H120JA01D');
cC0G('C35', '12pF', 'GRM1885C1H120JA01D');

parts.L1 = {
  value: '4.7uH',
  footprint: 'Inductor_SMD:L_Coilcraft_XAL4030-XXX',
  manufacturer: 'Coilcraft',
  mpn: 'XAL4030-472MEC',
  rating: '4.7 uH, shielded, Isat 4.6 A, DCR max 44.1 mOhm',
  datasheet: 'https://www.coilcraft.com/getmedia/6adcb47d-8b55-416c-976e-1e22e0d2848c/xal4000.pdf',
};
parts.L2 = {
  value: '2.2uH',
  footprint: 'Inductor_SMD:L_Coilcraft_XAL4030-XXX',
  manufacturer: 'Coilcraft',
  mpn: 'XAL4030-222MEC',
  rating: '2.2 uH, shielded, saturation current above 2.4 A, DCR below 30 mOhm',
  datasheet: 'https://www.coilcraft.com/getmedia/6adcb47d-8b55-416c-976e-1e22e0d2848c/xal4000.pdf',
};

const escapeValue = (value) => String(value).replaceAll('\\', '\\\\').replaceAll('"', '\\"');
const setProperty = (block, name, value) => {
  const expression = new RegExp(`(\\n\\t\\t\\(property "${name}" ")[^"]*(")`);
  if (!expression.test(block)) throw new Error(`Missing ${name} property`);
  return block.replace(expression, `$1${escapeValue(value)}$2`);
};

const addProperty = (block, name, value) => {
  if (new RegExp(`\\(property "${name}" "`).test(block)) return setProperty(block, name, value);
  const at = block.match(/\(property "Footprint" "[^"]*"\s*\n\s*\(at ([^)]+)\)/)?.[1];
  if (!at) throw new Error(`Cannot determine property position for ${name}`);
  const property = `\t\t(property "${name}" "${escapeValue(value)}"
\t\t\t(at ${at})
\t\t\t(hide yes)
\t\t\t(show_name no)
\t\t\t(do_not_autoplace no)
\t\t\t(effects
\t\t\t\t(font
\t\t\t\t\t(size 1.27 1.27)
\t\t\t\t)
\t\t\t)
\t\t)\n`;
  return block.replace('\t\t(property "Description" ', `${property}\t\t(property "Description" `);
};

let text = fs.readFileSync(schematic, 'utf8');
const starts = [...text.matchAll(/^\t\(symbol$/gm)].map((match) => match.index);
starts.push(text.length);
let output = '';
let cursor = 0;
const updated = new Set();

for (let index = 0; index < starts.length - 1; index += 1) {
  const start = starts[index];
  const end = starts[index + 1];
  output += text.slice(cursor, start);
  let block = text.slice(start, end);
  const ref = block.match(/\(property "Reference" "([RCL]\d+)"/)?.[1];
  if (ref && parts[ref]) {
    const part = parts[ref];
    block = setProperty(block, 'Value', part.value);
    block = setProperty(block, 'Footprint', part.footprint);
    block = setProperty(block, 'Datasheet', part.datasheet);
    block = setProperty(block, 'Description', `${part.manufacturer} ${part.mpn}; ${part.rating}`);
    block = addProperty(block, 'Manufacturer', part.manufacturer);
    block = addProperty(block, 'MPN', part.mpn);
    block = addProperty(block, 'Rating', part.rating);
    updated.add(ref);
  }
  output += block;
  cursor = end;
}
output += text.slice(cursor);

const missing = Object.keys(parts).filter((ref) => !updated.has(ref));
if (missing.length) throw new Error(`Passives not found in root schematic: ${missing.join(', ')}`);
if (updated.size !== 64) throw new Error(`Expected 64 R/C/L parts, updated ${updated.size}`);

fs.writeFileSync(schematic, output);
console.log(`Updated ${updated.size} root-schematic passives with values, footprints, ratings and MPNs.`);
