#!/usr/bin/env python3
"""Build the socket interface from the supplied Yuziki_1 KiCad source.

Requires KiCad's bundled Python (pcbnew). Run from any working directory.
This generator rewrites the socket-only target schematic/PCB: do not rerun after
adding expansion circuits. Use validate_yuzuki_socket.py for read-only checks.
"""
import hashlib
import argparse
import json
import math
import subprocess
import uuid
from pathlib import Path

import pcbnew
from kicad_sexpr import parse, children, child, prop

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / 'Yuziki_1'
OUT = ROOT / 'hardware/yuzuki_mvp'
LIBDIR = ROOT / 'hardware/libraries/yuzuki'
LIB = 'PISCIS_Yuzuki_MVP'
NAME = 'Yuzuki_2x20_Female_Socket'
FP = 'Yuzuki_Socket_2x1x20_P2.54mm_Row17.926mm'
CLI = '/Applications/KiCad/KiCad.app/Contents/MacOS/kicad-cli'
PROJECT = 'PISCIS-I_F101'
SCH = ROOT / (PROJECT + '.kicad_sch')
PCB = ROOT / (PROJECT + '.kicad_pcb')
SRCPCB = next(SRC.glob('*.kicad_pcb'))
SRCSCH = next(SRC.glob('*.kicad_sch'))


def uid(key):
    return str(uuid.uuid5(uuid.NAMESPACE_URL, 'piscis-yuzuki-mvp/' + key))


def q(s):
    return json.dumps(str(s), ensure_ascii=False)


def effects(size=1.0, justify='', thick=None):
    t = f' (thickness {thick})' if thick else ''
    j = f' (justify {justify})' if justify else ''
    return f'(effects (font (size {size} {size}){t}){j})'


def sch_prop(key, value, x, y, hidden=False):
    return f'(property {q(key)} {q(value)} (at {x} {y} 0) ' + \
        ('(hide yes) ' if hidden else '') + effects(1.27) + ')'


def fp_line(x1, y1, x2, y2, layer='F.SilkS', width=0.12):
    return f'(fp_line (start {x1:.6f} {y1:.6f}) (end {x2:.6f} {y2:.6f}) (stroke (width {width}) (type default)) (layer {q(layer)}))'


def fp_rect(x1, y1, x2, y2, layer='F.Fab', width=0.1):
    return f'(fp_rect (start {x1:.6f} {y1:.6f}) (end {x2:.6f} {y2:.6f}) (stroke (width {width}) (type default)) (fill none) (layer {q(layer)}))'


def fp_text(text, x, y, layer='F.SilkS', size=0.8, angle=0):
    return f'(fp_text user {q(text)} (at {x:.6f} {y:.6f} {angle}) (layer {q(layer)}) {effects(size, thick=0.12)})'


def sch_text(text, x, y, size=1.27):
    return f'(text {q(text)} (at {x} {y} 0) {effects(size,"left")} (uuid {q(uid("text/" + text))}))'


def absolute(f, p):
    at = child(f, 'at')
    x, y = map(float, at[1:3])
    theta = math.radians(float(at[3]) if len(at) > 3 else 0)
    px, py = map(float, child(p, 'at')[1:3])
    return [round(x + px * math.cos(theta) + py * math.sin(theta), 6),
            round(y - px * math.sin(theta) + py * math.cos(theta), 6)]


def main():
    # Preserve labels and electrical assignments from KiCad's actual netlist.
    source_net = OUT / 'reports/source.net'
    subprocess.run([CLI, 'sch', 'export', 'netlist', '-o', str(source_net), str(SRCSCH)], check=True)
    nets = {}
    for net in children(child(parse(source_net), 'nets'), 'net'):
        for node in children(net, 'node'):
            r = child(node, 'ref')[1]
            if r in ('H1', 'H2'):
                nets[(r, int(child(node, 'pin')[1]))] = child(net, 'name')[1].lstrip('/')
    source = parse(SRCPCB)
    headers = {prop(f, 'Reference'): f for f in children(source, 'footprint')
               if prop(f, 'Reference') in ('H1', 'H2')}
    holes = {}
    for ref, f in headers.items():
        # Imported edge rectangles share pad numbers with the round THT holes.
        for p in children(f, 'pad'):
            if p[2:4] == ['thru_hole', 'circle']:
                holes[(ref, int(p[1]))] = absolute(f, p)
        assert len([k for k in holes if k[0] == ref]) == 20
    origin = holes[('H1', 1)]
    pinmap = []
    for ref in ('H1', 'H2'):
        for n in range(1, 21):
            xy = holes[(ref, n)]
            pad = n + (20 if ref == 'H2' else 0)
            assert (ref, n) in nets
            # Cross-check every duplicate source pad's assigned net.
            source_nets = {child(p, 'net')[-1] for p in children(headers[ref], 'pad')
                           if p[1] == str(n) and child(p, 'net')}
            assert source_nets == {nets[(ref, n)]}, (ref, n, source_nets)
            pinmap.append(dict(pad=pad, source_ref=ref, source_pin=n,
                               net=nets[(ref, n)], source_xy_mm=xy,
                               socket_xy_mm=[round(xy[i]-origin[i],6) for i in (0,1)]))
    bypad = {p['pad']: p for p in pinmap}
    row = bypad[40]['socket_xy_mm'][0]
    offset = bypad[40]['socket_xy_mm'][1]
    assert abs(row - 17.926) < 1e-6
    assert abs(offset - 0.01) < 1e-6

    # Passive contacts: connector interface, no implied drivers or PWR_FLAGs.
    symbol = [f'(symbol {q(NAME)} (pin_names (offset 1.016)) (exclude_from_sim no) (in_bom yes) (on_board yes)',
              sch_prop('Reference', 'J', 0, 38.1),
              sch_prop('Value', NAME, 0, 34.29),
              sch_prop('Footprint', LIB+':'+FP, 0, 0, True),
              sch_prop('Datasheet', '', 0, 0, True),
              sch_prop('Description', 'Pair of 1x20 female sockets; exact Yuziki_1 hole layout; H1=1..20, H2=21..40', 0, 0, True),
              f'(symbol {q(NAME+"_0_1")}',
              '(rectangle (start -27.94 29.21) (end 27.94 -29.21) (stroke (width 0.254) (type default)) (fill (type background)))',
              '(polyline (pts (xy 0 26.67) (xy 0 -26.67)) (stroke (width 0.1524) (type default)) (fill (type none)))',
              f'(text "H1 / LEFT" (at -13.97 26.67 0) {effects(1.27)})',
              f'(text "H2 / RIGHT" (at 13.97 26.67 0) {effects(1.27)})',
              f'(text "USB-C END" (at 0 31.75 0) {effects(1.0)})',
              ')', f'(symbol {q(NAME+"_1_1")}']
    for p in pinmap:
        n = p['source_pin']
        left = p['source_ref'] == 'H1'
        y = 24.13 - (n-1)*2.54 if left else -24.13 + (n-1)*2.54
        x, ang = (-33.02, 0) if left else (33.02, 180)
        name = f'{p["source_ref"]}.{n}_{p["net"]}'
        symbol.append(f'(pin passive line (at {x} {y:.4f} {ang}) (length 5.08) (name {q(name)} {effects(0.95)}) (number {q(p["pad"])} {effects(0.95)}))')
    symbol += [')', ')']
    symbol = '\n'.join(symbol)
    (LIBDIR/(LIB+'.kicad_sym')).write_text(f'(kicad_symbol_lib (version 20241209) (generator "piscis_yuzuki_socket")\n{symbol}\n)\n')

    footprint = [f'(footprint {q(FP)} (version 20260206) (generator "pcbnew") (layer "F.Cu")',
                 '(descr "Exact Yuziki_1 female socket assembly: 2x 1x20 P2.54mm, row 17.926mm, H2 top offset +0.010mm. Front/top view; USB-C up. Drill 1.0mm, pad 1.7mm per KiCad generic socket.")',
                 '(tags "Yuzuki Yuziki F101 female socket carrier 2x20")',
                 '(attr through_hole)',
                 f'(property "Reference" "REF**" (at 8.963 -6 0) (layer "F.SilkS") {effects(1,thick=0.15)})',
                 f'(property "Value" {q(NAME)} (at 8.963 54 0) (layer "F.Fab") {effects(1,thick=0.15)})',
                 f'(property "Description" "2 x 1x20 P2.54mm female sockets; H1=1..20, H2=21..40" (at 8.963 57 0) (layer "F.Fab") (hide yes) {effects(1,thick=0.15)})',
                 f'(property "Assembly_Qty" "2 x 1x20 vertical female socket" (at 8.963 55.5 0) (layer "F.Fab") (hide yes) {effects(1,thick=0.15)})',
                 fp_rect(-1.8, -1.7, 19.75, 49.9, 'F.CrtYd', 0.05)]
    for rx, top, ref in [(0,0,'H1'), (row, offset, 'H2')]:
        footprint += [fp_rect(rx-1.27,top-1.27,rx+1.27,top+49.53),
                      fp_rect(rx-1.38,top-1.38,rx+1.38,top+49.64,'F.SilkS',0.12),
                      fp_text(ref,rx,top-2.4), fp_text('1' if ref=='H1' else '20', rx-2.2 if ref=='H1' else rx+2.2,top)]
    for p in pinmap:
        x,y = p['socket_xy_mm']
        shape = 'rect' if p['source_pin']==1 else 'circle'
        footprint.append(f'(pad {q(p["pad"])} thru_hole {shape} (at {x} {y}) (size 1.7 1.7) (drill 1) (layers "*.Cu" "*.Mask") (remove_unused_layers no))')
        # Signal legend is inside the plastic rows; text is outside drill/pad area.
        lx = 4.4 if p['source_ref']=='H1' else row-4.4
        footprint.append(fp_text(p['net'],lx,y,'F.SilkS',0.8))
    # Real module outline and screw positions are assembly guides, not baseboard cuts.
    for g in source:
        if not isinstance(g,list) or not g or g[0] not in ('gr_line','gr_arc') or child(g,'layer') != ['layer','Edge.Cuts']:
            continue
        coords=[]
        for k in ('start','mid','end'):
            c=child(g,k)
            if c: coords.append(f'({k} {float(c[1])-origin[0]:.6f} {float(c[2])-origin[1]:.6f})')
        footprint.append(f'({g[0].replace("gr_","fp_")} {" ".join(coords)} (stroke (width 0.1) (type default)) (layer "Dwgs.User"))')
    for f in children(source,'footprint'):
        if ':Pad_e' in f[1]:
            h=child(f,'at'); x,y=float(h[1])-origin[0],float(h[2])-origin[1]
            footprint.append(f'(fp_circle (center {x:.6f} {y:.6f}) (end {x+1.05:.6f} {y:.6f}) (stroke (width 0.1) (type default)) (fill none) (layer "Dwgs.User"))')
    footprint += [fp_text('USB-C / TOP',8.963,-3.4,'F.SilkS',0.9),
                  fp_text('TF CARD / BOTTOM',8.963,51.7,'F.SilkS',0.8)]
    model='${KIPRJMOD}/hardware/libraries/yuzuki/PISCIS_Yuzuki_MVP.3dshapes/PinSocket_1x20_P2.54mm_Vertical.step'
    for x,y in [(0,0),(row,offset)]:
        footprint.append(f'(model {q(model)} (offset (xyz {x} {-y} 0)) (scale (xyz 1 1 1)) (rotate (xyz 0 0 0)))')
    footprint += ['(embedded_fonts no)',')']
    fp_path=LIBDIR/(LIB+'.pretty')/(FP+'.kicad_mod')
    fp_path.write_text('\n'.join(footprint)+'\n')

    rootid = child(parse(SCH),'uuid')[1]
    instanceid = uid('J1')
    cx,cy=148.59,86.36
    schematic=[f'(kicad_sch (version 20260306) (generator "eeschema") (generator_version "10.0") (uuid {q(rootid)}) (paper "A4")',
               '(title_block (title "PISCIS-I F101 - Yuzuki MVP Socket Interface") (date "2026-10-20") (rev "MVP-1") (company "PISCIS-I"))',
               '(lib_symbols '+symbol.replace(f'(symbol {q(NAME)}',f'(symbol {q(LIB+":"+NAME)}',1)+')',
               sch_text('YUZUKI DEVELOPMENT BOARD - PLUG-IN FEMALE SOCKET',45.72,25.4,1.8),
               sch_text('Top view of carrier. USB-C end up; TF-card end down. J1 = two 1x20 sockets.',45.72,33.02,1.1),
               f'(symbol (lib_id {q(LIB+":"+NAME)}) (at {cx} {cy} 0) (unit 1) (exclude_from_sim no) (in_bom yes) (on_board yes) (dnp no) (uuid {q(instanceid)})',
               sch_prop('Reference','J1',cx,cy-38.1),
               sch_prop('Value','Yuzuki_2x20_Female_Socket',cx,cy-34.29),
               sch_prop('Footprint',LIB+':'+FP,cx,cy,True),
               sch_prop('Datasheet','',cx,cy,True),
               sch_prop('Description','2 x 1x20 P2.54mm female sockets; H1=1..20, H2=21..40',cx,cy,True),
               sch_prop('Assembly_Qty','2 x 1x20 vertical female socket',cx,cy,True)]
    for p in pinmap:
        schematic.append(f'(pin {q(p["pad"])} (uuid {q(uid("pin/"+str(p["pad"])))}))')
    schematic += [f'(instances (project {q(PROJECT)} (path {q("/"+rootid)} (reference "J1") (unit 1))))',')']
    for p in pinmap:
        n=p['source_pin']; left=p['source_ref']=='H1'
        sy=24.13-(n-1)*2.54 if left else -24.13+(n-1)*2.54
        py=round(cy-sy,4); xpin=cx+(-33.02 if left else 33.02)
        xlab=xpin+(-17.78 if left else 17.78)
        angle=180 if left else 0
        just='right' if left else 'left'
        schematic += [f'(wire (pts (xy {xpin:.4f} {py}) (xy {xlab:.4f} {py})) (stroke (width 0) (type default)) (uuid {q(uid("wire/"+str(p["pad"])))}))',
                      f'(global_label {q(p["net"])} (shape passive) (at {xlab:.4f} {py} {angle}) {effects(1,just)} (uuid {q(uid("label/"+str(p["pad"])))}))']
    notes=[
        'PIN MAPPING: J1 pads 1..20 = source H1.1..H1.20; pads 21..40 = source H2.1..H2.20.',
        'All 40 contacts are exported as global nets with the original Yuziki_1 signal names.',
        'MECHANICAL: pitch 2.54 mm; row centres 17.926 mm; right row top +0.010 mm.',
        'Assembly: female sockets on carrier F.Cu. Development-board male pins face down.',
        'VBUS is the source-board USB supply rail; VCC-3V3 is its 3.3 V regulator rail.',
        'No power isolation or regulation is added here. Module outline / screw holes are drawing guides.',
        'Generic 1x20 socket: 1.0 mm finished drill, 1.7 mm pad. Confirm purchased socket fit and height.',
    ]
    for i,t in enumerate(notes): schematic.append(sch_text(t,45.72,132.08+i*5.08,1.05))
    schematic += ['(sheet_instances (path "/" (page "1")))','(embedded_fonts no)',')']
    SCH.write_text('\n'.join(schematic)+'\n')

    # The target board started empty; preserve settings, place one rigid assembly.
    board=pcbnew.LoadBoard(str(OUT/'originals/PISCIS-I_F101.empty.kicad_pcb'))
    fp=pcbnew.FootprintLoad(str(fp_path.parent), FP)
    assert fp and len(list(fp.Pads())) == 40
    fp.SetFPID(pcbnew.LIB_ID(LIB,FP))
    fp.SetReference('J1'); fp.SetValue(NAME)
    fp.SetUuid(pcbnew.KIID(instanceid))
    fp.SetPath(pcbnew.KIID_PATH('/'+rootid+'/'+instanceid))
    fp.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(100),pcbnew.FromMM(75)))
    fp.SetLocked(True)
    for p in pinmap:
        net=pcbnew.NETINFO_ITEM(board,p['net'])
        board.Add(net)
        pad=fp.FindPadByNumber(str(p['pad']))
        pad.SetNet(net)
    board.Add(fp)
    board.GetTitleBlock().SetTitle('PISCIS-I F101 - Yuzuki MVP Socket Interface')
    board.GetTitleBlock().SetDate('2026-10-20'); board.GetTitleBlock().SetRevision('MVP-1')
    pcbnew.SaveBoard(str(PCB),board)
    for table,kind,path in [('sym-lib-table','symbol',LIBDIR/(LIB+'.kicad_sym')),
                            ('fp-lib-table','footprint',LIBDIR/(LIB+'.pretty'))]:
        t=ROOT/table; s=t.read_text()
        if f'(name "{LIB}")' not in s:
            relative='${KIPRJMOD}/'+str(path.relative_to(ROOT))
            entry=f'  (lib (name "{LIB}")(type "KiCad")(uri "{relative}")(options "")(descr "Yuzuki MVP exact mating socket {kind} library"))\n'
            t.write_text(s.rstrip()[:-1]+entry+')\n')
    source_metadata=dict(source_schematic=str(SRCSCH.relative_to(ROOT)),source_pcb=str(SRCPCB.relative_to(ROOT)),
                         source_schematic_sha256=hashlib.sha256(SRCSCH.read_bytes()).hexdigest(),
                         source_pcb_sha256=hashlib.sha256(SRCPCB.read_bytes()).hexdigest(),
                         origin_source_H1_1_mm=origin,row_centres_mm=row,right_row_top_offset_mm=offset,
                         view='Carrier F.Cu top view, source module component side up; USB-C up; H1 left, H2 right.',
                         footprint=LIB+':'+FP,pins=pinmap)
    (OUT/'pinmap.json').write_text(json.dumps(source_metadata,ensure_ascii=False,indent=2)+'\n')
    print('Wrote socket symbol, footprint, schematic, placed PCB and source pinmap. All 40 source nets matched.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rebuild-socket-only', action='store_true', required=True,
                        help='explicitly rebuild the socket-stage schematic/PCB; do not use after adding expansion circuits')
    parser.parse_args()
    main()
