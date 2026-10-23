import fs from 'node:fs';
import path from 'node:path';

const base = path.join(process.cwd(), 'hardware', 'rev_a');
const mainSheets = ['01_MCU.sch','02_Power.sch','03_Memory.sch','04_Storage.sch','05_USB_Host.sch','06_Audio.sch','07_Control.sch','08_Debug.sch'];
const requiredNets = ['SD_DQ0','SD_DQ15','SD_A12','SD_CLK_SRC','QSPI_IO3','SD_DAT3','USBH_DP','I2S0_MCLK','HP_DETECT_N','AMP_SHDN','CTRL_SPI_SCK','ENC_DATA','JTAG_TCK'];
const requiredParts = ['HPM6360IPA2','W9825G6KH-6I','GD25Q128ESIGR','FUSB303BTMX','MT9700-N','ES8388','PJ-320D-B','PAM8302A','ESD5304D-10/TR','ESD5311N-2/TR'];
const all = mainSheets.map((f) => fs.readFileSync(path.join(base, f), 'utf8')).join('\n');

const errors = [];
for (const file of [...mainSheets, 'PISCIS-I_HPM6360_RevA.sch', 'PISCIS-I_Control_8x8.sch']) {
  const text = fs.readFileSync(path.join(base, file), 'utf8');
  const starts = (text.match(/^\$Comp$/gm) ?? []).length;
  const ends = (text.match(/^\$EndComp$/gm) ?? []).length;
  if (starts !== ends) errors.push(`${file}: component blocks ${starts}/${ends}`);
  if (!text.endsWith('$EndSCHEMATC\n')) errors.push(`${file}: missing schematic terminator`);
}
for (const net of requiredNets) if (!all.includes(`\n${net}\n`)) errors.push(`missing net ${net}`);
for (const part of requiredParts) if (!all.includes(part)) errors.push(`missing part ${part}`);
for (const marker of ['C97572','C2928891','C365736','C2884940','C371129','C895444','C42441843','C153722','C153721']) if (!all.includes(marker)) errors.push(`missing sourcing marker ${marker}`);

const refs = [...all.matchAll(/^F 0 "([A-Z]+[0-9]+)"/gm)].map((m) => m[1]);
const duplicateRefs = [...new Set(refs.filter((r, i) => refs.indexOf(r) !== i))];
if (duplicateRefs.length) errors.push(`duplicate main-board refs: ${duplicateRefs.join(', ')}`);

const control = fs.readFileSync(path.join(base, 'PISCIS-I_Control_8x8.sch'), 'utf8');
for (let i=0;i<8;i++) {
  for (const net of [`ADC_CH${i}`,`ENC${i}_A`,`ENC${i}_B`]) if (!control.includes(`\n${net}\n`)) errors.push(`control board missing ${net}`);
}

const pcb = fs.readFileSync(path.join(base, 'PISCIS-I_HPM6360_RevA.kicad_pcb'), 'utf8');
for (const layer of ['"F.Cu"','"In1.Cu"','"In2.Cu"','"In3.Cu"','"In4.Cu"','"B.Cu"']) if (!pcb.includes(layer)) errors.push(`PCB missing ${layer}`);
const footprintCount = (pcb.match(/^\s*\(footprint /gm) ?? []).length;
if (footprintCount !== refs.length) errors.push(`main PCB/schematic footprint mismatch: ${footprintCount}/${refs.length}`);

const controlPcb = fs.readFileSync(path.join(base, 'PISCIS-I_Control_8x8.kicad_pcb'), 'utf8');
const controlFootprintCount = (controlPcb.match(/^\s*\(footprint /gm) ?? []).length;
const controlRefs = [...control.matchAll(/^F 0 "([A-Z]+[0-9]+)"/gm)].map((m) => m[1]);
if (controlFootprintCount !== controlRefs.length) errors.push(`control PCB/schematic footprint mismatch: ${controlFootprintCount}/${controlRefs.length}`);

if (errors.length) {
  console.error(errors.join('\n'));
  process.exit(1);
}
console.log(`Rev.A source check passed: ${mainSheets.length} hierarchy sheets, ${refs.length} main-board symbols/footprints, ${controlRefs.length} control-board symbols/footprints.`);
