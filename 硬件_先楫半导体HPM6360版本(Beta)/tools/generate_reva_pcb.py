#!/usr/bin/env python3
"""Generate the Rev.A six-layer placement baseline with KiCad's pcbnew API."""
from pathlib import Path
import csv
import pcbnew

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "hardware" / "rev_a" / "PISCIS-I_HPM6360_RevA.kicad_pcb"
CONTROL_OUT = ROOT / "hardware" / "rev_a" / "PISCIS-I_Control_8x8.kicad_pcb"
BOM = ROOT / "hardware" / "rev_a" / "BOM_RevA.csv"
PRIV = ROOT / "hardware" / "libraries" / "private" / "PISCIS_Private.pretty"
STD = Path("/Applications/KiCad/KiCad.app/Contents/SharedSupport/footprints")

board = pcbnew.BOARD()
board.SetCopperLayerCount(6)

def mm(v): return pcbnew.FromMM(v)
def pos(x, y): return pcbnew.VECTOR2I(mm(x), mm(y))

def edge(x1, y1, x2, y2):
    item = pcbnew.PCB_SHAPE(board)
    item.SetShape(pcbnew.SHAPE_T_SEGMENT)
    item.SetStart(pos(x1, y1)); item.SetEnd(pos(x2, y2))
    item.SetLayer(pcbnew.Edge_Cuts); item.SetWidth(mm(0.05))
    board.Add(item)

# Spacious engineering board. Placement is intentionally partitioned for probing.
for segment in [(0,0,130,0),(130,0,130,90),(130,90,0,90),(0,90,0,0)]: edge(*segment)

libraries = {
    "PISCIS_Private": PRIV,
    "Package_SO": STD / "Package_SO.pretty",
    "Connector_Card": STD / "Connector_Card.pretty",
    "Connector_USB": STD / "Connector_USB.pretty",
    "Connector_JST": STD / "Connector_JST.pretty",
    "Connector_IDC": STD / "Connector_IDC.pretty",
    "Connector_PinHeader_2.54mm": STD / "Connector_PinHeader_2.54mm.pretty",
    "Connector_Tag-Connect": STD / "Connector.pretty",
    "Resistor_SMD": STD / "Resistor_SMD.pretty",
    "Capacitor_SMD": STD / "Capacitor_SMD.pretty",
    "Inductor_SMD": STD / "Inductor_SMD.pretty",
    "TestPoint": STD / "TestPoint.pretty",
    "Fuse": STD / "Fuse.pretty",
}

def add(ref, value, lib, name, x, y, rotation=0):
    footprint = pcbnew.FootprintLoad(str(libraries[lib]), name)
    if footprint is None:
        raise RuntimeError(f"Cannot load {lib}:{name}")
    footprint.SetReference(ref); footprint.SetValue(value)
    footprint.SetPosition(pos(x, y)); footprint.SetOrientationDegrees(rotation)
    board.Add(footprint)
    return footprint

# Digital core / memory region.
add("U1","HPM6360IPA2","PISCIS_Private","HPM6360IPA2_eLQFP144L_20x20mm_P0.5mm_EP5x5mm",58,43)
add("U4","W9825G6KH-6I C97572","PISCIS_Private","TSOP-54_L22.2-W10.2-P0.80-LS11.8-BL",30,43,90)
add("U5","GD25Q128ESIGR C2928891","PISCIS_Private","SOP-8_L5.3-W5.3-P1.27-LS8.0-BL",60,23)
add("Y1","24MHz C70590","PISCIS_Private","CRYSTAL-SMD_4P-L3.2-W2.5-BL",44,27)
add("Y2","32.768k C371129","PISCIS_Private","CRYSTAL-SMD_4P-L2.5-W2.0-BL",74,27)

# Power-only USB-C and power conversion on the left edge.
add("J1","USB-C POWER ONLY","PISCIS_Private","USB-C_SMD-TYPE-C-31-M-12_1",8,76,90)
add("U2","FUSB303BTMX C895444","PISCIS_Private","X2QFN-12_L1.6-W1.6-P0.40-BL",19,73)
add("U3","SM8082AAAC 3V3","PISCIS_Private","SOT-23-5_L3.0-W1.7-P0.95-LS2.8-BL",31,73)
add("L2","3V3 BUCK","Inductor_SMD","L_1210_3225Metric_Pad1.42x2.65mm_HandSolder",39,73)

# microSD and control expansion along lower edge.
add("J2","microSD SDIO","Connector_Card","microSD_HC_Hirose_DM3AT-SF-PEJM5",60,76,180)
add("J6","CTRL EXP 2x8","Connector_IDC","IDC-Header_2x08_P2.54mm_Vertical",91,70)

# Host USB upper edge.
add("J3","USB-A HOST MIDI","Connector_USB","USB_A_Stewart_SS-52100-001_Horizontal",65,7,180)
add("U6","MT9700-N HOST","PISCIS_Private","SOT-23-5_L2.9-W1.6-P0.95-LS2.9-BL",78,12)

# Analog island at the right edge.
add("U7","ES8388 C365736","PISCIS_Private","QFN-28_L4.0-W4.0-P0.45-BL-EP",101,40)
add("U8","PAM8302A","Package_SO","MSOP-8_3x3mm_P0.65mm",113,43)
add("J4","PJ-320D-B C2884940","PISCIS_Private","AUDIO-SMD_XKB_PJ-320D-B",124,28,90)
add("J5","4R 3W SPEAKER","PISCIS_Private","JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal",123,54,90)

# Service interface and reserved probing area.
add("J7","JTAG","Connector_Tag-Connect","Tag-Connect_TC2050-IDC-NL_2x05_P1.27mm_Vertical",103,12)
add("J8","SERVICE","Connector_PinHeader_2.54mm","PinHeader_2x08_P2.54mm_Vertical",116,68)

# Add every remaining BOM footprint.  These are deliberate unrouted placement
# candidates grouped by functional sheet; they prevent the baseline PCB from
# silently omitting passives, protection parts, switches or test pads.
fixed_refs = {fp.GetReference() for fp in board.GetFootprints()}
zones = {
    "01_MCU.sch": (42, 20, 82, 66),
    "02_Power.sch": (5, 61, 43, 87),
    "03_Memory.sch": (12, 19, 43, 64),
    "04_Storage.sch": (45, 66, 80, 87),
    "05_USB_Host.sch": (43, 3, 88, 18),
    "06_Audio.sch": (88, 21, 127, 63),
    "07_Control.sch": (81, 64, 110, 87),
    "08_Debug.sch": (90, 3, 127, 20),
}
zone_index = {name: 0 for name in zones}

def grid_position(sheet):
    x0, y0, x1, y1 = zones[sheet]
    step = 4.2
    cols = max(1, int((x1 - x0) / step))
    idx = zone_index[sheet]; zone_index[sheet] += 1
    return x0 + (idx % cols) * step, y0 + (idx // cols) * step

with BOM.open(newline="", encoding="utf-8") as handle:
    for row in csv.DictReader(handle):
        if row["Board"] != "MAIN" or row["Reference"] in fixed_refs:
            continue
        nickname, footprint_name = row["Footprint"].split(":", 1)
        x, y = grid_position(row["Sheet"])
        add(row["Reference"], row["Value"], nickname, footprint_name, x, y)

def label(text, x, y, layer=pcbnew.F_SilkS, size=1.5):
    t = pcbnew.PCB_TEXT(board)
    t.SetText(text); t.SetPosition(pos(x,y)); t.SetLayer(layer)
    t.SetTextSize(pcbnew.VECTOR2I(mm(size),mm(size)))
    t.SetTextThickness(mm(0.25)); board.Add(t)

label("DIGITAL / SDRAM",43,59)
label("POWER",17,86)
label("AUDIO - KEEP SWITCHING NODES OUT",91,61,pcbnew.F_SilkS,1.2)
label("USB HOST",55,16)
label("REV.A ENGINEERING PROTOTYPE - NOT ROUTED",41,88,pcbnew.F_SilkS,1.2)

pcbnew.SaveBoard(str(OUT), board)
print(f"Wrote {OUT} with {len(board.GetFootprints())} footprints")

# Two-layer control daughterboard placement baseline.  Potentiometers and
# encoders are cabled to keyed 3-pin headers along opposite edges so mechanics
# can be changed without respinning the main board.
control = pcbnew.BOARD()
control.SetCopperLayerCount(2)

def control_edge(x1, y1, x2, y2):
    item = pcbnew.PCB_SHAPE(control)
    item.SetShape(pcbnew.SHAPE_T_SEGMENT)
    item.SetStart(pos(x1, y1)); item.SetEnd(pos(x2, y2))
    item.SetLayer(pcbnew.Edge_Cuts); item.SetWidth(mm(0.05)); control.Add(item)

for segment in [(0,0,130,0),(130,0,130,80),(130,80,0,80),(0,80,0,0)]: control_edge(*segment)

def add_control(ref, value, lib, name, x, y, rotation=0):
    footprint = pcbnew.FootprintLoad(str(libraries[lib]), name)
    if footprint is None: raise RuntimeError(f"Cannot load {lib}:{name}")
    footprint.SetReference(ref); footprint.SetValue(value)
    footprint.SetPosition(pos(x, y)); footprint.SetOrientationDegrees(rotation)
    control.Add(footprint); return footprint

control_fixed = {
    "J1": ("Connector_IDC", "IDC-Header_2x08_P2.54mm_Vertical", 8, 40, 90),
    "U1": ("Package_SO", "SOIC-16W_7.5x10.3mm_P1.27mm", 42, 42, 0),
    "U2": ("Package_SO", "SOIC-16_3.9x9.9mm_P1.27mm", 66, 35, 0),
    "U3": ("Package_SO", "SOIC-16_3.9x9.9mm_P1.27mm", 83, 35, 0),
}
control_rows = []
with BOM.open(newline="", encoding="utf-8") as handle:
    control_rows = [r for r in csv.DictReader(handle) if r["Board"] == "CONTROL"]
by_ref = {r["Reference"]: r for r in control_rows}
for ref, (lib, name, x, y, rot) in control_fixed.items():
    add_control(ref, by_ref[ref]["Value"], lib, name, x, y, rot)
for i in range(8):
    row = by_ref[f"JP{i+1}"]; lib, name = row["Footprint"].split(":", 1)
    add_control(row["Reference"], row["Value"], lib, name, 24 + i*12.5, 72, 180)
    row = by_ref[f"JE{i+1}"]; lib, name = row["Footprint"].split(":", 1)
    add_control(row["Reference"], row["Value"], lib, name, 24 + i*12.5, 8, 0)

placed = {fp.GetReference() for fp in control.GetFootprints()}
passive_index = 0
passive_rows = [20, 27, 54, 61]
for row in control_rows:
    if row["Reference"] in placed: continue
    lib, name = row["Footprint"].split(":", 1)
    cols = 12; x = 22 + (passive_index % cols) * 8.2; y = passive_rows[passive_index // cols]
    passive_index += 1
    add_control(row["Reference"], row["Value"], lib, name, x, y)

def control_label(text, x, y, size=1.3):
    t = pcbnew.PCB_TEXT(control); t.SetText(text); t.SetPosition(pos(x,y)); t.SetLayer(pcbnew.F_SilkS)
    t.SetTextSize(pcbnew.VECTOR2I(mm(size),mm(size))); t.SetTextThickness(mm(0.22)); control.Add(t)

control_label("PISCIS-I CONTROL 8 ADC + 8 ENCODER - NOT ROUTED", 33, 77, 1.1)
control_label("POTS", 104, 68); control_label("ENCODERS", 100, 13)
pcbnew.SaveBoard(str(CONTROL_OUT), control)
print(f"Wrote {CONTROL_OUT} with {len(control.GetFootprints())} footprints")
