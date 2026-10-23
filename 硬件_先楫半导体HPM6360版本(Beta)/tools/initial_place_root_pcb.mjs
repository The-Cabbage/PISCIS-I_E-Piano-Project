import fs from 'node:fs';
import path from 'node:path';

const pcbPath = path.join(process.cwd(), 'PISCIS-I_HPM6360_Module.kicad_pcb');

// Placement baseline for the existing 70 x 70 mm rounded board whose usable
// outline spans X=94.5..164.5 mm and Y=20..90 mm.  Coordinates deliberately
// keep the four 3 mm corner holes clear and reserve edge space for connectors
// whose footprints are not yet assigned in the schematic.
const placement = {
  J1: [129.5, 23.675, 180],
  USBC1: [159.41, 61.0, 270],
  X1: [144.0, 49.5, 90],
  U1: [129.5, 53.5, 0],
  U2: [151.0, 59.5, 90],
  U3: [143.0, 77.5, 90],
  U4: [152.0, 74.5, 0],
  U5: [109.0, 55.0, 0],

  L1: [114.0, 61.5, 0],
  L2: [136.5, 77.5, 0],
  D1: [157.0, 80.5, 0],

  C1: [109.0, 60.0, 0],
  C2: [109.0, 64.0, 0],
  C3: [115.5, 47.0, 90],
  C4: [115.5, 52.0, 90],
  C5: [115.5, 57.5, 90],
  C6: [136.0, 67.0, 0],
  C7: [143.0, 60.5, 90],
  C8: [143.0, 53.5, 90],
  C9: [143.0, 44.0, 90],
  C10: [136.0, 40.0, 0],
  C11: [129.5, 40.0, 0],
  C12: [122.7, 40.0, 0],
  C13: [121.0, 67.2, 0],
  C14: [124.0, 67.2, 0],
  C15: [120.5, 69.4, 0],
  C16: [123.7, 69.4, 0],
  C17: [112.5, 49.5, 90],
  C18: [154.5, 58.0, 90],
  C19: [154.5, 61.4, 90],
  C20: [146.2, 60.0, 90],
  C21: [145.5, 64.0, 90],
  C22: [145.5, 72.0, 0],
  C23: [131.5, 76.0, 0],
  C24: [131.5, 80.0, 0],
  C25: [134.0, 74.0, 0],
  C26: [134.0, 72.0, 0],
  C27: [146.5, 77.8, 90],
  C28: [146.5, 81.4, 90],
  C29: [149.0, 81.2, 90],
  C30: [143.0, 57.0, 90],
  C31: [146.2, 56.0, 90],
  C32: [127.0, 67.2, 0],
  C33: [154.0, 55.5, 0],
  C34: [147.0, 47.5, 90],
  C35: [147.0, 51.5, 90],
  C36: [133.2, 68.9, 0],
  C37: [151.0, 84.5, 0],
  C38: [130.0, 67.2, 0],
  C39: [117.0, 57.0, 90],
  C40: [136.4, 68.9, 0],

  R1: [151.0, 55.5, 0],
  R2: [138.5, 72.0, 0],
  R3: [142.0, 72.0, 0],
  R4: [142.5, 75.0, 0],
  R5: [150.0, 52.0, 90],
  R6: [126.8, 68.9, 0],
  R7: [130.0, 68.9, 0],
  R8: [111.5, 43.5, 0],
  R9: [115.0, 43.5, 0],
  R10: [117.2, 72.0, 0],
  R11: [120.4, 72.0, 0],
  R12: [123.6, 72.0, 0],
  R13: [126.8, 72.0, 0],
  R14: [142.5, 68.7, 0],
  R15: [146.0, 68.7, 0],
  R16: [155.0, 71.5, 0],
  R17: [158.5, 71.5, 0],
  R18: [105.5, 50.0, 0],
  R19: [109.0, 50.0, 0],
  R20: [111.5, 45.5, 0],
  R21: [157.0, 48.0, 90],
  R22: [157.0, 51.0, 90],

  TP1: [105.0, 84.0, 0],
  TP2: [109.0, 84.0, 0],
  TP3: [113.0, 84.0, 0],
  TP4: [117.0, 84.0, 0],
  TP5: [121.0, 84.0, 0],
};

function findTopLevelBlocks(text, keyword) {
  const blocks = [];
  const needle = `\n\t(${keyword}`;
  let searchFrom = 0;
  while (true) {
    const found = text.indexOf(needle, searchFrom);
    if (found < 0) break;
    const start = found + 1;
    let depth = 0;
    let quoted = false;
    let escaped = false;
    let end = -1;
    for (let index = start; index < text.length; index += 1) {
      const char = text[index];
      if (quoted) {
        if (escaped) escaped = false;
        else if (char === '\\') escaped = true;
        else if (char === '"') quoted = false;
        continue;
      }
      if (char === '"') quoted = true;
      else if (char === '(') depth += 1;
      else if (char === ')') {
        depth -= 1;
        if (depth === 0) {
          end = index + 1;
          break;
        }
      }
    }
    if (end < 0) throw new Error(`Unterminated ${keyword} block at ${start}`);
    blocks.push({ start, end, text: text.slice(start, end) });
    searchFrom = end;
  }
  return blocks;
}

let text = fs.readFileSync(pcbPath, 'utf8');
const footprints = findTopLevelBlocks(text, 'footprint');
const replacements = [];
const placed = new Set();

for (const block of footprints) {
  const ref = block.text.match(/\(property "Reference" "([^"]+)"/)?.[1];
  if (!ref || !placement[ref]) continue;
  const [x, y, rotation] = placement[ref];
  const atPattern = /^(\t\t\(at )[^)]+(\))$/m;
  if (!atPattern.test(block.text)) throw new Error(`Could not find position for ${ref}`);
  const updated = block.text.replace(
    atPattern,
    (_match, prefix, suffix) =>
      `${prefix}${x.toFixed(3)} ${y.toFixed(3)} ${rotation}${suffix}`,
  );
  replacements.push({ start: block.start, end: block.end, text: updated });
  placed.add(ref);
}

const missing = Object.keys(placement).filter((ref) => !placed.has(ref));
if (missing.length) throw new Error(`Placement references missing from PCB: ${missing.join(', ')}`);

const obsoleteUuids = new Set([
  '2db290df-7dd5-4056-8ea4-b159b27b0a74', // superseded 65 x 65 rectangular outline
  '0d54d48c-ce79-4800-a553-139deae46b30', // obsolete POGO comment box
  '2b91dc14-af38-43ca-8c88-08fb3020eb5f', // obsolete POGO note
]);
for (const keyword of ['gr_rect', 'gr_text']) {
  for (const block of findTopLevelBlocks(text, keyword)) {
    const uuid = block.text.match(/\(uuid "([^"]+)"\)/)?.[1];
    if (uuid && obsoleteUuids.has(uuid)) {
      replacements.push({ start: block.start, end: block.end, text: '' });
    }
  }
}

// Move the existing board identification onto the active rounded outline and
// correct its dimension annotation from the obsolete 65 mm value.
for (const block of findTopLevelBlocks(text, 'gr_text')) {
  if (!block.text.includes('a52a708f-ccea-42e3-af7b-d86e166158f5')) continue;
  let updated = block.text.replace(
    'PISCIS-I HPM6360 | REV A0 | 65x65 mm',
    'PISCIS-I HPM6360 | REV A0 | 70x70 mm',
  );
  updated = updated.replace(
    /^(\t\t\(at )[^)]+(\))$/m,
    (_match, prefix, suffix) => `${prefix}129.500 87.000 0${suffix}`,
  );
  replacements.push({ start: block.start, end: block.end, text: updated });
}

replacements.sort((a, b) => b.start - a.start);
for (const replacement of replacements) {
  text = text.slice(0, replacement.start) + replacement.text + text.slice(replacement.end);
}

fs.writeFileSync(pcbPath, text);
console.log(`Placed ${placed.size} footprints on the four-hole rounded board.`);
