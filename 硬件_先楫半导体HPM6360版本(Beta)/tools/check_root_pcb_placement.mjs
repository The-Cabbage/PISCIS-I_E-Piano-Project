import fs from 'node:fs';

const boardPath = process.argv[2] ?? 'PISCIS-I_HPM6360_Module.kicad_pcb';
const text = fs.readFileSync(boardPath, 'utf8');

function findBlocks(source, keyword) {
  const blocks = [];
  const needle = `(${keyword}`;
  let searchFrom = 0;
  while (true) {
    const start = source.indexOf(needle, searchFrom);
    if (start < 0) break;
    let depth = 0;
    let quoted = false;
    let escaped = false;
    let end = -1;
    for (let index = start; index < source.length; index += 1) {
      const char = source[index];
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
    if (end < 0) throw new Error(`Unterminated ${keyword} block`);
    blocks.push(source.slice(start, end));
    searchFrom = end;
  }
  return blocks;
}

function courtyardBounds(footprint) {
  const reference = footprint.match(/\(property "Reference" "([^"]+)"/)?.[1];
  const position = footprint.match(/^\t\t\(at ([^ )]+) ([^ )]+)(?: ([^ )]+))?\)$/m);
  if (!reference || !position) return null;
  const originX = Number(position[1]);
  const originY = Number(position[2]);
  const angle = Number(position[3] ?? 0) * Math.PI / 180;
  const cosine = Math.cos(angle);
  const sine = Math.sin(angle);
  const points = [];

  for (const keyword of ['fp_line', 'fp_rect', 'fp_circle', 'fp_arc', 'fp_poly']) {
    for (const graphic of findBlocks(footprint, keyword)) {
      if (!graphic.includes('(layer "F.CrtYd")')) continue;
      const coordinate = /\((?:start|end|mid|center|xy)\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)/g;
      for (const match of graphic.matchAll(coordinate)) {
        const localX = Number(match[1]);
        const localY = Number(match[2]);
        points.push([
          originX + localX * cosine - localY * sine,
          originY + localX * sine + localY * cosine,
        ]);
      }
    }
  }
  if (!points.length) return null;
  return {
    reference,
    minX: Math.min(...points.map(([x]) => x)),
    maxX: Math.max(...points.map(([x]) => x)),
    minY: Math.min(...points.map(([, y]) => y)),
    maxY: Math.max(...points.map(([, y]) => y)),
  };
}

const footprints = findBlocks(text, 'footprint');
const bounds = footprints.map(courtyardBounds).filter(Boolean);
const missingCourtyard = footprints.length - bounds.length;
const overlaps = [];

for (let first = 0; first < bounds.length; first += 1) {
  for (let second = first + 1; second < bounds.length; second += 1) {
    const a = bounds[first];
    const b = bounds[second];
    const overlapX = Math.min(a.maxX, b.maxX) - Math.max(a.minX, b.minX);
    const overlapY = Math.min(a.maxY, b.maxY) - Math.max(a.minY, b.minY);
    if (overlapX > 0.001 && overlapY > 0.001) {
      overlaps.push(`${a.reference}/${b.reference} (${overlapX.toFixed(3)} x ${overlapY.toFixed(3)} mm)`);
    }
  }
}

const boardLimits = { minX: 94.5, maxX: 164.5, minY: 20, maxY: 90 };
const edgeMountedReferences = new Set(['J1', 'USBC1']);
const outside = bounds.filter((box) =>
  !edgeMountedReferences.has(box.reference) &&
  (box.minX < boardLimits.minX || box.maxX > boardLimits.maxX ||
   box.minY < boardLimits.minY || box.maxY > boardLimits.maxY));
const edgeOverhangs = bounds.filter((box) =>
  edgeMountedReferences.has(box.reference) &&
  (box.minX < boardLimits.minX || box.maxX > boardLimits.maxX ||
   box.minY < boardLimits.minY || box.maxY > boardLimits.maxY));
const mountingHoles = [
  [97.353553, 22.853553],
  [161.646446, 22.853553],
  [97.353553, 87.146446],
  [161.646446, 87.146446],
];
const holeKeepoutRadius = 4.0;
const holeKeepoutHits = [];
for (const box of bounds) {
  for (let index = 0; index < mountingHoles.length; index += 1) {
    const [holeX, holeY] = mountingHoles[index];
    const deltaX = Math.max(box.minX - holeX, 0, holeX - box.maxX);
    const deltaY = Math.max(box.minY - holeY, 0, holeY - box.maxY);
    if (Math.hypot(deltaX, deltaY) < holeKeepoutRadius) {
      holeKeepoutHits.push(`${box.reference}/H${index + 1}`);
    }
  }
}

console.log(`Footprints: ${footprints.length}; courtyards checked: ${bounds.length}; missing: ${missingCourtyard}`);
console.log(`Courtyard AABB overlaps: ${overlaps.length}`);
for (const overlap of overlaps) console.log(`  ${overlap}`);
console.log(`Courtyards outside 70 x 70 mm board limits: ${outside.length}`);
for (const box of outside) console.log(`  ${box.reference}`);
console.log(`Expected edge-connector courtyard overhangs: ${edgeOverhangs.length}`);
for (const box of edgeOverhangs) console.log(`  ${box.reference}`);
console.log(`Courtyards inside 4.0 mm mounting-hole keepouts: ${holeKeepoutHits.length}`);
for (const hit of holeKeepoutHits) console.log(`  ${hit}`);

if (missingCourtyard || overlaps.length || outside.length || holeKeepoutHits.length) process.exitCode = 1;
