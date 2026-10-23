# Rev.A validation status

## Automated checks passed

- Eight main-board hierarchy sheets and one control-board schematic are syntactically complete legacy KiCad files.
- Main-board references are unique; required nets, selected parts and LCSC sourcing fields are present.
- `pinmux.csv` contains the official HPM6300EVK FEMC/SDXC0 assignments and HPM6360 IOMUX-derived XPI/I2S/SPI assignments.
- The main PCB has six copper layers and contains all 102 main-board BOM footprints.
- The control PCB contains all 57 control-board BOM footprints.
- Both PCB files load and render with KiCad 10.0.6.
- `BOM_RevA.csv` contains 159 rows: 102 main-board and 57 control-board items.

Run the reproducible checks with:

```sh
node tools/generate_reva_hardware.mjs
node tools/export_reva_bom.mjs
/Applications/KiCad/KiCad.app/Contents/Frameworks/Python.framework/Versions/Current/bin/python3 tools/generate_reva_pcb.py
node tools/check_reva_hardware.mjs
```

## Open fabrication gates

- The PCB files are placement baselines only. They contain no synchronized netlist, tracks, planes, length matching or impedance rules and must not be sent for fabrication.
- KiCad 10.0.6 on this workstation remains stuck in its legacy schematic import progress dialog, including for a minimal legacy test file. Therefore no modern `.kicad_sch` conversion or KiCad ERC report is claimed.
- PCB DRC is not meaningful until the schematics are converted, nets are imported, power/GND planes are created and routing is complete.
- Confirm the PAM8302A ordering-code pinout and input network, SM8082 feedback ratio/current capability, HPM6360 exposed-pad dimensions, microSD switch polarity, connector mechanical keepouts and every imported EasyEDA footprint against the actual purchased parts.
- Confirm all non-EVK pin assignments in the official HPM pinmux/configuration tool before routing.
- Complete the planned bring-up tests for SDRAM, QSPI, SDIO streaming, USB MIDI/over-current, audio noise/mute/thermal performance and the 8+8 control board on assembled hardware.

