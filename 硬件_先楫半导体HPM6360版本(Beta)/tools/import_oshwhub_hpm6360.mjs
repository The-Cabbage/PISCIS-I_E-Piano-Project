import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const sourceFile = path.join(projectRoot, 'hardware', 'libraries', 'sources', 'oshwhub', 'HPM6360-测试板.epru');
const outputFile = path.join(projectRoot, 'hardware', 'libraries', 'footprints', 'PISCIS_HPM6360.pretty', 'HPM6360IPA2_EQFP144_20x20mm_P0.5mm_LS22mm_EP5x5mm_OSHWHub.kicad_mod');
const targetTitle = 'EQFP-144_L20.0-W20.0-P0.50-LS22.0-TL-EP5.0';
const milToMm = (value) => Number(value) * 0.0254;
const format = (value) => milToMm(value).toFixed(4);

const lines = fs.readFileSync(sourceFile, 'utf8').split(/\r?\n/);
let block = null;
for (let start = 0; start < lines.length; start += 1) {
  if (!lines[start].includes('"type":"DOCHEAD"')) continue;
  let end = lines.findIndex((line, index) => index > start && line.includes('"type":"DOCHEAD"'));
  if (end < 0) end = lines.length;
  const candidate = lines.slice(start, end);
  if (candidate.some((line) => line.includes(targetTitle))) {
    block = candidate;
    break;
  }
}
if (block == null) throw new Error(`Could not locate ${targetTitle} in ${sourceFile}`);

const pads = block
  .filter((line) => line.startsWith('{"type":"PAD"'))
  .map((line) => JSON.parse(line.split('||')[1].replace(/\|$/, '')))
  .sort((a, b) => Number(a.num) - Number(b.num));
if (pads.length !== 145 || pads.at(-1).num !== '145') throw new Error(`Expected pads 1–145, got ${pads.length}`);

const footprintName = 'HPM6360IPA2_EQFP144_20x20mm_P0.5mm_LS22mm_EP5x5mm_OSHWHub';
const padText = pads.map((pad) => {
  const shape = pad.num === '1' ? 'roundrect' : 'rect';
  const ratio = shape === 'roundrect' ? ' (roundrect_rratio 0.20)' : '';
  return `\t(pad "${pad.num}" smd ${shape} (at ${format(pad.centerX)} ${format(pad.centerY)} ${pad.relativeAngle ?? 0}) (size ${format(pad.defaultPad.width)} ${format(pad.defaultPad.height)}) (layers "F.Cu" "F.Paste" "F.Mask")${ratio})`;
}).join('\n');

const footprint = `(footprint "${footprintName}"
\t(version 20240108)
\t(generator "PISCIS epro2 footprint importer")
\t(layer "F.Cu")
\t(descr "Imported from OSHWHub HPM6360-测试板: ${targetTitle}; source document UUID 3ddab72c81724aa0ad5c5ea3c0b1ede1")
\t(tags "HPM6360 eLQFP144 EQFP144 20x20 P0.5 EP5x5 OSHWHub")
\t(property "Reference" "U" (at 0 -12 0) (layer "F.SilkS") (effects (font (size 1 1) (thickness 0.15))))
\t(property "Value" "HPM6360IPA2" (at 0 12 0) (layer "F.Fab") (effects (font (size 1 1) (thickness 0.15))))
\t(fp_rect (start -10 -10) (end 10 10) (stroke (width 0.12) (type default)) (fill none) (layer "F.Fab"))
\t(fp_circle (center -10.65 -9.5) (end -10.35 -9.5) (stroke (width 0.12) (type default)) (fill solid) (layer "F.SilkS"))
${padText}
)
`;

fs.writeFileSync(outputFile, footprint);
console.log(`Imported ${pads.length} pads into ${outputFile}`);
