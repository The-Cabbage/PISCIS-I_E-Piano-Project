# Power component CAD library

This project-local KiCad library contains:

| Part | Symbol source | Footprint / 3D source | Pin mapping |
|---|---|---|---|
| SM8082AAAC | EasyEDA/LCSC C479068, checked against Silergy SM8082A datasheet | EasyEDA package `SOT-23-5_L3.0-W1.7-P0.95-LS2.8-BL` and linked STEP/WRL model | 1 EN, 2 GND, 3 LX, 4 IN, 5 FB |
| XC6210B332MR | KiCad official `Regulator_Linear` library, checked against Torex XC6210 datasheet | KiCad official `SOT-23-5` footprint and STEP model | 1 VIN, 2 VSS, 3 CE, 4 NC, 5 VOUT |
| SPX3819M5-L-1-8 | KiCad official `Regulator_Linear` library, checked against MaxLinear SPX3819 datasheet | KiCad official `SOT-23-5` footprint and STEP model | 1 IN, 2 GND, 3 EN, 4 BP, 5 OUT |

The EasyEDA assets were retrieved through the public component record for LCSC part `C479068`. The KiCad assets retain the upstream KiCad library licensing terms. Always confirm the selected package against the purchasing MPN before production release.
