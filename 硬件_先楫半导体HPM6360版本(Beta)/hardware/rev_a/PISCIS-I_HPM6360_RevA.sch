EESchema Schematic File Version 4
LIBS:PISCIS-I_HPM6360_RevA-cache
EELAYER 29 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 9
Title "PISCIS-I HPM6360 Electronic Instrument Evaluation Board"
Date "2026-09-27"
Rev "A"
Comp "PISCIS-I"
Comment1 "ENGINEERING PROTOTYPE - VERIFY BEFORE FABRICATION"
Comment2 "HPM6360 electronic instrument evaluation board"
Comment3 "Generated from locked Rev.A architecture"
Comment4 ""
$EndDescr
Text Notes 900 700 0    140  ~ 28
REV.A FUNCTIONAL ARCHITECTURE
Text Notes 900 1050 0    70   ~ 12
USB-C 5V power-only | USB-A Host/MIDI | 32 MiB SDRAM | microSD samples | ES8388 audio | 8+8 control expansion
$Sheet
S 1100 1700 5000 1400
U 66F00001
F0 "MCU, clocks and core power" 60
F1 "01_MCU.sch" 60
$EndSheet
$Sheet
S 7600 1700 5000 1400
U 66F00002
F0 "USB-C 5V input and system power" 60
F1 "02_Power.sch" 60
$EndSheet
$Sheet
S 1100 3800 5000 1400
U 66F00003
F0 "32 MiB SDRAM and 16 MiB QSPI NOR" 60
F1 "03_Memory.sch" 60
$EndSheet
$Sheet
S 7600 3800 5000 1400
U 66F00004
F0 "4-bit SDIO microSD" 60
F1 "04_Storage.sch" 60
$EndSheet
$Sheet
S 1100 5900 5000 1400
U 66F00005
F0 "USB 2.0 Type-A host and VBUS switch" 60
F1 "05_USB_Host.sch" 60
$EndSheet
$Sheet
S 7600 5900 5000 1400
U 66F00006
F0 "ES8388 headphone and mono speaker audio" 60
F1 "06_Audio.sch" 60
$EndSheet
$Sheet
S 1100 8000 5000 1400
U 66F00007
F0 "Control-board expansion interface" 60
F1 "07_Control.sch" 60
$EndSheet
$Sheet
S 7600 8000 5000 1400
U 66F00008
F0 "JTAG, UART, reset and boot" 60
F1 "08_Debug.sch" 60
$EndSheet
$EndSCHEMATC
