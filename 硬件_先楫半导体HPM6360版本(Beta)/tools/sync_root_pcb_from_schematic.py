#!/Applications/KiCad/KiCad.app/Contents/Frameworks/Python.framework/Versions/3.9/bin/python3
"""Sync pad nets for existing root-PCB footprints and add the available U3."""

from pathlib import Path
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET

import pcbnew


board_path = Path(sys.argv[1] if len(sys.argv) > 1 else "PISCIS-I_HPM6360_Module.kicad_pcb")
project_root = Path(__file__).resolve().parent.parent
schematic_path = project_root / "PISCIS-I_HPM6360_Module.kicad_sch"
library_path = project_root / "hardware/libraries/private/PISCIS_Private.pretty"

with tempfile.NamedTemporaryFile(suffix=".xml") as netlist_file:
    subprocess.run(
        [
            "kicad-cli",
            "sch",
            "export",
            "netlist",
            "--format",
            "kicadxml",
            "--output",
            netlist_file.name,
            str(schematic_path),
        ],
        check=True,
        stdout=subprocess.DEVNULL,
    )
    root = ET.parse(netlist_file.name).getroot()

pad_net_names = {}
for net in root.findall("./nets/net"):
    net_name = net.attrib["name"]
    for node in net.findall("node"):
        pad_net_names[(node.attrib["ref"], node.attrib["pin"])] = net_name

board = pcbnew.LoadBoard(str(board_path))
references = {footprint.GetReference() for footprint in board.GetFootprints()}

additions = {
    "U3": {
        "footprint": "SOT-23-5_L3.0-W1.7-P0.95-LS2.8-BL",
        "value": "SM8082AAAC",
        "position": (143.0, 77.5),
        "rotation": 90,
    },
    "J1": {
        "footprint": "USB_C_Receptacle_GCT_USB4105-xx-A_14P_Compat_TopMnt_Horizontal",
        "value": "USB_C_Receptacle_USB2.0_14P",
        "position": (129.5, 23.675),
        "rotation": 180,
    },
    "USBC1": {
        "footprint": "USB-C_SMD-TYPE-C-31-M-12_1",
        "value": "TYPE-C-31-M-12",
        "position": (159.41, 61.0),
        "rotation": 270,
    },
    "X1": {
        "footprint": "CRYSTAL-SMD_4P-L2.5-W2.0-BL",
        "value": "X252024MOB4SI",
        "position": (144.0, 49.5),
        "rotation": 90,
    },
}

for reference, item in additions.items():
    if reference in references:
        continue
    footprint = pcbnew.FootprintLoad(
        str(library_path),
        item["footprint"],
    )
    if footprint is None:
        raise RuntimeError(f"Could not load the assigned {reference} footprint")
    footprint.SetReference(reference)
    footprint.SetValue(item["value"])
    pos_x, pos_y = item["position"]
    footprint.SetPosition(
        pcbnew.VECTOR2I(pcbnew.FromMM(pos_x), pcbnew.FromMM(pos_y))
    )
    footprint.SetOrientationDegrees(item["rotation"])
    board.Add(footprint)
    print(f"Added {reference} from the assigned project-local footprint.")

nets_by_name = {str(name): net for name, net in board.GetNetsByName().items()}
updated_pads = 0
unmapped_pads = []
for footprint in board.GetFootprints():
    reference = footprint.GetReference()
    for pad in footprint.Pads():
        pad_number = pad.GetNumber()
        if not pad_number:  # Unnumbered paste-only thermal-window subdivisions.
            continue
        net_name = pad_net_names.get((reference, pad_number))
        if net_name is None:
            unmapped_pads.append(f"{reference}.{pad_number}")
            continue
        net = nets_by_name.get(net_name)
        if net is None:
            net = pcbnew.NETINFO_ITEM(board, net_name)
            board.Add(net)
            nets_by_name[net_name] = net
        if pad.GetNetname() != net_name:
            pad.SetNet(net)
            updated_pads += 1

if unmapped_pads:
    raise RuntimeError("Pads missing from schematic netlist: " + ", ".join(unmapped_pads))

pcbnew.SaveBoard(str(board_path), board)
print(f"Synchronized pad nets; changed {updated_pads} pad assignments.")
