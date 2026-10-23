# Root schematic passive selection

Scope: `PISCIS-I_HPM6360_Module.kicad_sch` only. The `hardware/rev_a` hierarchy is not modified.

The schematic stores the circuit value in `Value` and the purchasable identity in `Manufacturer`, `MPN`, and `Rating`. All resistors, capacitors, and inductors now have a real footprint.

| Circuit use | References | Selected implementation |
|---|---|---|
| HPM6360 internal DCDC output | C1, C2, L1 | 2 x 22 uF / 16 V X5R in 1206; 4.7 uH shielded Coilcraft XAL4030-472MEC |
| HPM6360 rail decoupling | C3-C14, C17, C32, C38, C39 | 100 nF / 50 V X7R in 0603 |
| OTP and PMC regulator reservoirs | C15, C16 | 4.7 uF / 10 V X5R in 0603, each paired with a 100 nF capacitor |
| XC6210 input/output and enable filtering | C18-C21, C33, R1 | 100 nF plus 1 uF ceramic input/output filtering; 100 kOhm CE bias and 1 uF delay/filter capacitor |
| SM8082 3.3 V buck feedback | R2, R3, C22 | 100 kOhm / 22.1 kOhm divider and 22 pF C0G feed-forward capacitor, matching the 3.3 V table in the SM8082A datasheet |
| SM8082 3.3 V buck power stage | C23, C24, C25, C26, L2 | 2 x 22 uF / 16 V X5R plus 2 x 100 nF; 2.2 uH shielded Coilcraft XAL4030-222MEC |
| 5 V input decoupling | C27, C28, C29 | 2 x 10 uF / 10 V X5R plus 100 nF |
| Analog LDO rail | C30, C31 | 100 nF plus 4.7 uF ceramic |
| 24 MHz crystal network | C34, C35, R5 | 12 pF / 50 V C0G load capacitors and 2 MOhm feedback resistor |
| Reset filter | C36, R6, R7 | 100 nF, 33 Ohm series damping, 10 kOhm pull-up |
| 5 V system bulk | C37 | 22 uF / 16 V X5R in 1206 |
| 3.3 V rail bulk | C40 | 4.7 uF / 10 V X5R in 0603 |
| Boot, JTAG and flash straps | R8-R13, R18-R20 | 10 kOhm, 1%, 0603 |
| UART damping | R14, R15 | 33 Ohm, 1%, 0603 |
| USB-C sink CC terminations | R16, R17, R21, R22 | 5.1 kOhm, 1%, 0603 |

Primary design references:

- HPMicro HPM6300 user manual: DCDC_LP accepts 2.2-10 uH; VDD_PMCCAP and VDD_OTPCAP each require 4.7 uF.
- Silergy SM8082A application note: 3.3 V uses 100 kOhm / 22.1 kOhm / 22 pF feedback parts, input capacitance of at least 10 uF, output capacitance greater than 10 uF, and an inductor whose peak-current rating exceeds the full-load peak current.
- Torex XC6210 datasheet: 3.3 V versions support ceramic input/output capacitors, with at least 1 uF required on the output.

The update is reproducible with:

```sh
node tools/update_root_passives.mjs
```
