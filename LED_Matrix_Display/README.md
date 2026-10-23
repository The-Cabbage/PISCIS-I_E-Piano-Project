# LED Matrix Display

Independent KiCad 10 project for evaluating the small hidden LED matrix display.

## Project-private libraries

- `LED_Matrix_Private:LED_0201_0603Metric`
- `LED_Matrix_Private:LED_0402_1005Metric`

Each private symbol has its matching project-local footprint preassigned. Pin mapping follows the KiCad LED convention: pad 1 is cathode (`K`) and pad 2 is anode (`A`).

The symbol graphics and the two footprint snapshots originate from the KiCad 10 standard `Device` and `LED_SMD` libraries installed with KiCad 10.0.6. The footprints retain their standard KiCad 3D-model references through `${KICAD10_3DMODEL_DIR}`. Before production, the selected manufacturer's LED datasheet must be checked against pad geometry, polarity marking and reflow requirements.

No schematic circuit or PCB layout has been created yet.
