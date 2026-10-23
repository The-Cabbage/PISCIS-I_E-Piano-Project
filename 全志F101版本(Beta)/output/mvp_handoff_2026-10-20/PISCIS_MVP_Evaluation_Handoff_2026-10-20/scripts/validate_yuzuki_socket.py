#!/usr/bin/env python3
"""Read-only cross-check of the source board, socket library, schematic and PCB.

Run with system Python 3. KiCad CLI regenerates netlists only inside reports/.
"""
import hashlib
import json
import math
import subprocess
from collections import Counter
from pathlib import Path

from kicad_sexpr import parse, children, child, prop

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'hardware/yuzuki_mvp'
CLI = '/Applications/KiCad/KiCad.app/Contents/MacOS/kicad-cli'


def absolute(f, p):
    a = child(f, 'at')
    x, y = map(float, a[1:3])
    theta = math.radians(float(a[3]) if len(a) > 3 else 0)
    px, py = map(float, child(p, 'at')[1:3])
    return (x + px*math.cos(theta) + py*math.sin(theta),
            y - px*math.sin(theta) + py*math.cos(theta))


def netlist(path, out):
    subprocess.run([CLI, 'sch', 'export', 'netlist', '-o', str(out), str(path)], check=True)
    result = {}
    for net in children(child(parse(out), 'nets'), 'net'):
        for node in children(net, 'node'):
            result[(child(node, 'ref')[1], child(node, 'pin')[1])] = child(net, 'name')[1].lstrip('/')
    return result


def main():
    meta = json.loads((OUT/'pinmap.json').read_text())
    sourcepcb = ROOT/meta['source_pcb']
    sourcesch = ROOT/meta['source_schematic']
    for p, key in [(sourcepcb, 'source_pcb_sha256'), (sourcesch, 'source_schematic_sha256')]:
        assert hashlib.sha256(p.read_bytes()).hexdigest() == meta[key], 'Source changed: '+str(p)
    source = parse(sourcepcb)
    source_nets = netlist(sourcesch, OUT/'reports/source.net')
    target_nets = netlist(ROOT/'PISCIS-I_F101.kicad_sch', OUT/'reports/socket.net')
    assert len(target_nets) == 40, f'Expected 40 connected J1 pins; got {len(target_nets)}'
    src_holes = {}
    for f in children(source, 'footprint'):
        ref = prop(f, 'Reference')
        if ref in ('H1','H2'):
            for p in children(f, 'pad'):
                if p[2:4] == ['thru_hole','circle']:
                    src_holes[(ref,p[1])] = absolute(f,p)
    origin = src_holes[('H1','1')]
    lib, fpname = meta['footprint'].split(':')
    fp = parse(ROOT/'hardware/libraries/yuzuki'/f'{lib}.pretty'/f'{fpname}.kicad_mod')
    libpads = {p[1]: p for p in children(fp, 'pad')}
    assert len(libpads) == len(children(fp,'pad')) == 40
    board = parse(ROOT/'PISCIS-I_F101.kicad_pcb')
    placed = [f for f in children(board, 'footprint') if prop(f,'Reference') == 'J1']
    assert len(placed) == 1
    placed = placed[0]
    assert placed[1] == meta['footprint']
    placedpads = {p[1]:p for p in children(placed,'pad')}
    assert len(placedpads) == 40
    bx,by = map(float, child(placed,'at')[1:3])
    max_error = 0
    for item in meta['pins']:
        n = str(item['pad']); key = (item['source_ref'],str(item['source_pin']))
        expected = tuple(src_holes[key][i] - origin[i] for i in (0,1))
        actual = tuple(map(float, child(libpads[n],'at')[1:3]))
        placedxy = absolute(placed,placedpads[n])
        for i in (0,1):
            max_error = max(max_error, abs(actual[i]-expected[i]), abs(placedxy[i]-(bx,by)[i]-expected[i]))
        assert source_nets[key] == target_nets[('J1',n)] == item['net']
        assert child(placedpads[n],'net')[-1] == item['net']
        assert child(libpads[n],'drill') == ['drill','1']
    assert max_error < 1e-6, f'Mechanical mismatch: {max_error} mm'
    sch = parse(ROOT/'PISCIS-I_F101.kicad_sch')
    instance = next(s for s in children(sch,'symbol') if prop(s,'Reference') == 'J1')
    assert child(placed,'path')[1] == '/'+child(sch,'uuid')[1]+'/'+child(instance,'uuid')[1]
    assert child(placed,'uuid')[1] == child(instance,'uuid')[1]
    assert prop(placed,'Assembly_Qty') == prop(instance,'Assembly_Qty')
    symbols = parse(ROOT/'hardware/libraries/yuzuki'/f'{lib}.kicad_sym')
    libsymbol = children(symbols,'symbol')[0]
    cached = children(child(sch,'lib_symbols'),'symbol')[0]
    assert libsymbol[2:] == cached[2:], 'Schematic cached symbol differs from local library'
    sympins = []
    for unit in children(libsymbol,'symbol'):
        sympins += children(unit,'pin')
    assert {child(p,'number')[1] for p in sympins} == set(libpads)
    for model in children(fp,'model'):
        assert Path(model[1].replace('${KIPRJMOD}',str(ROOT))).is_file()
    erc = json.loads((OUT/'reports/erc.json').read_text())
    violations = [v for sheet in erc['sheets'] for v in sheet['violations']]
    assert not [v for v in violations if v['severity']=='error']
    assert all(v['type']=='isolated_pin_label' for v in violations)
    drc = json.loads((OUT/'reports/drc.json').read_text())
    assert not drc['schematic_parity'], 'PCB/schematic parity mismatch'
    assert not drc['unconnected_items']
    assert all(v['type']=='invalid_outline' for v in drc['violations']), 'Unexpected footprint DRC issue'
    report = dict(result='PASS', source_holes_matched=40, electrical_pin_maps_matched=40,
                  maximum_coordinate_error_mm=round(max_error,9),
                  erc_errors=0, erc_warnings=dict(Counter(v['type'] for v in violations)),
                  pcb_schematic_parity_issues=len(drc['schematic_parity']),
                  pcb_unconnected_items=len(drc['unconnected_items']),
                  pcb_remaining_drc=dict(Counter(v['type'] for v in drc['violations'])),
                  scope='Socket interface only. Baseboard outline, expansion circuits and routing remain to be designed.')
    (OUT/'reports/validation.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))


if __name__ == '__main__':
    main()
