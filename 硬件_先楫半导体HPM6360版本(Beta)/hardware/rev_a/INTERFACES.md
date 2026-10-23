# Rev.A connector contract

## J1 — USB-C power input

Power-only fixed Sink. Use a 5 V / 3 A Type-C source. D+/D- are intentionally not connected. FUSB303 reports advertised current to firmware.

## J3 — USB-A Host

USB0 D+/D-, with VBUS supplied through MT9700-N. Intended for USB MIDI devices.

## J4 — PJ-320D-B headphone

Stereo TRS output plus detect contact. Firmware shall deassert AMP_SHDN after headphone insertion.

## J5 — speaker

JST-PH 2.0 mm, pin 1 SPK_P, pin 2 SPK_N. Both pins are bridge outputs; neither may be grounded. Load: 4 ohm, up to 3 W.

## J6 — control expansion

| Pin | Signal | Pin | Signal |
|---:|---|---:|---|
|1|3V3_CTRL|2|GND|
|3|5V_CTRL_SW|4|GND|
|5|CTRL_SPI_SCK|6|GND|
|7|CTRL_SPI_MOSI|8|CTRL_SPI_MISO|
|9|ADC_CS_N|10|ENC_LOAD_N|
|11|ENC_DATA|12|CTRL_PRESENT_N|
|13|CTRL_I2C_SCL|14|CTRL_I2C_SDA|
|15|CTRL_IRQ_N|16|GND|

Cable length is limited to 20 cm.
