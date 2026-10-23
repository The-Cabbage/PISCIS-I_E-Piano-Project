EESchema Schematic File Version 4
LIBS:PISCIS-I_HPM6360_RevA-cache
EELAYER 29 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 2 9
Title "MCU, clocks and core power"
Date "2026-09-27"
Rev "A"
Comp "PISCIS-I"
Comment1 "ENGINEERING PROTOTYPE - VERIFY BEFORE FABRICATION"
Comment2 "HPM6360 electronic instrument evaluation board"
Comment3 "Generated from locked Rev.A architecture"
Comment4 ""
$EndDescr
Text Notes 800 550 0    80   ~ 12
HPM6360IPA2 / clocks / internal core DCDC
$Comp
L PISCIS_RevA:HPM6360IPA2 U1
U 1 1 66F00009
P 8000 5800
F 0 "U1" H 8350 5950 50  0000 C CNN
F 1 "HPM6360IPA2" H 8350 5650 50  0000 C CNN
F 2 "PISCIS_Private:HPM6360IPA2_eLQFP144L_20x20mm_P0.5mm_EP5x5mm" H 8000 5800 50  0001 C CNN
F 3 "" H 8000 5800 50  0001 C CNN
F 4 "HPM6360IPA2" H 8000 5800 50  0001 C CNN "LCSC Part"
	1    8000 5800
	1    0    0    -1
$EndComp
NoConn ~ 7500 3100
Text GLabel 7500 3175 2    40   BiDi ~ 0
BOOT_1
Text GLabel 7500 3250 2    40   BiDi ~ 0
BOOT_0
NoConn ~ 7500 3325
Text GLabel 7500 3400 2    40   BiDi ~ 0
VDD_CORE
NoConn ~ 7500 3475
NoConn ~ 7500 3550
NoConn ~ 7500 3625
NoConn ~ 7500 3700
Text GLabel 7500 3775 2    40   BiDi ~ 0
SD_CD_N
Text GLabel 7500 3850 2    40   BiDi ~ 0
SD_DAT1
Text GLabel 7500 3925 2    40   BiDi ~ 0
3V3_SYS
Text GLabel 7500 4000 2    40   BiDi ~ 0
SD_DAT0
Text GLabel 7500 4075 2    40   BiDi ~ 0
SD_CLK
Text GLabel 7500 4150 2    40   BiDi ~ 0
SD_CMD
Text GLabel 7500 4225 2    40   BiDi ~ 0
SD_DAT3
Text GLabel 7500 4300 2    40   BiDi ~ 0
SD_DAT2
Text GLabel 7500 4375 2    40   BiDi ~ 0
VDD_CORE
NoConn ~ 7500 4450
NoConn ~ 7500 4525
Text GLabel 7500 4600 2    40   BiDi ~ 0
QSPI_IO3
Text GLabel 7500 4675 2    40   BiDi ~ 0
QSPI_CLK
Text GLabel 7500 4750 2    40   BiDi ~ 0
3V3_SYS
Text GLabel 7500 4825 2    40   BiDi ~ 0
QSPI_IO0
Text GLabel 7500 4900 2    40   BiDi ~ 0
QSPI_IO2
Text GLabel 7500 4975 2    40   BiDi ~ 0
QSPI_IO1
Text GLabel 7500 5050 2    40   BiDi ~ 0
QSPI_CS_N
Text GLabel 7500 5125 2    40   BiDi ~ 0
VDD_CORE
Text GLabel 7500 5200 2    40   BiDi ~ 0
VDD_CORE
Text GLabel 7500 5275 2    40   BiDi ~ 0
GND
Text GLabel 7500 5350 2    40   BiDi ~ 0
DCDC_LX
Text GLabel 7500 5425 2    40   BiDi ~ 0
3V3_SYS
Text GLabel 7500 5500 2    40   BiDi ~ 0
UART0_RXD
Text GLabel 7500 5575 2    40   BiDi ~ 0
UART0_TXD
NoConn ~ 7500 5650
Text GLabel 7500 5725 2    40   BiDi ~ 0
JTAG_TRST
Text GLabel 7500 5800 2    40   BiDi ~ 0
JTAG_TMS
Text GLabel 7500 5875 2    40   BiDi ~ 0
JTAG_TCK
Text GLabel 7500 5950 2    40   BiDi ~ 0
OTP_CAP
Text GLabel 7500 6025 2    40   BiDi ~ 0
JTAG_TDI
Text GLabel 7500 6100 2    40   BiDi ~ 0
PMC_CAP
Text GLabel 7500 6175 2    40   BiDi ~ 0
JTAG_TDO
Text GLabel 7500 6250 2    40   BiDi ~ 0
3V3_SYS
Text GLabel 7500 6325 2    40   BiDi ~ 0
RTC_XTALI
Text GLabel 7500 6400 2    40   BiDi ~ 0
RTC_XTALO
Text GLabel 7500 6475 2    40   BiDi ~ 0
3V3_SYS
Text GLabel 7500 6550 2    40   BiDi ~ 0
AMP_SHDN
Text GLabel 7500 6625 2    40   BiDi ~ 0
HP_DETECT_N
Text GLabel 7500 6700 2    40   BiDi ~ 0
USBH_OC_N
Text GLabel 7500 6775 2    40   BiDi ~ 0
USBH_EN
Text GLabel 7500 6850 2    40   BiDi ~ 0
TYPEC_INT_N
NoConn ~ 7500 6925
Text GLabel 7500 7000 2    40   BiDi ~ 0
RESET_N
NoConn ~ 7500 7075
Text GLabel 7500 7150 2    40   BiDi ~ 0
VDD_CORE
NoConn ~ 7500 7225
NoConn ~ 7500 7300
NoConn ~ 7500 7375
NoConn ~ 7500 7450
Text GLabel 7500 7525 2    40   BiDi ~ 0
CTRL_IRQ_N
Text GLabel 7500 7600 2    40   BiDi ~ 0
3V3_SYS
NoConn ~ 7500 7675
Text GLabel 7500 7750 2    40   BiDi ~ 0
CTRL_SPI_MOSI
Text GLabel 7500 7825 2    40   BiDi ~ 0
CTRL_SPI_SCK
Text GLabel 7500 7900 2    40   BiDi ~ 0
CTRL_SPI_MISO
Text GLabel 7500 7975 2    40   BiDi ~ 0
ADC_CS_N
Text GLabel 7500 8050 2    40   BiDi ~ 0
ENC_DATA
Text GLabel 7500 8125 2    40   BiDi ~ 0
VDD_CORE
Text GLabel 7500 8200 2    40   BiDi ~ 0
CTRL_PRESENT_N
Text GLabel 7500 8275 2    40   BiDi ~ 0
ENC_LOAD_N
Text GLabel 7500 8350 2    40   BiDi ~ 0
CTRL_I2C_SDA
Text GLabel 7500 8425 2    40   BiDi ~ 0
CTRL_I2C_SCL
NoConn ~ 7500 8500
NoConn ~ 8500 3138
NoConn ~ 8500 3213
NoConn ~ 8500 3288
Text GLabel 8500 3363 0    40   BiDi ~ 0
VDD_CORE
NoConn ~ 8500 3438
NoConn ~ 8500 3513
NoConn ~ 8500 3588
NoConn ~ 8500 3663
Text GLabel 8500 3738 0    40   BiDi ~ 0
I2S0_DIN
Text GLabel 8500 3813 0    40   BiDi ~ 0
I2S0_MCLK
Text GLabel 8500 3888 0    40   BiDi ~ 0
GND
Text GLabel 8500 3963 0    40   BiDi ~ 0
3V3_A
Text GLabel 8500 4038 0    40   BiDi ~ 0
3V3_SYS
Text GLabel 8500 4113 0    40   BiDi ~ 0
3V3_A
Text GLabel 8500 4188 0    40   BiDi ~ 0
XTAL_IN
Text GLabel 8500 4263 0    40   BiDi ~ 0
XTAL_OUT
Text GLabel 8500 4338 0    40   BiDi ~ 0
VDD_CORE
Text GLabel 8500 4413 0    40   BiDi ~ 0
5V_SYS
Text GLabel 8500 4488 0    40   BiDi ~ 0
USBH_DP
Text GLabel 8500 4563 0    40   BiDi ~ 0
USBH_DM
Text GLabel 8500 4638 0    40   BiDi ~ 0
3V3_SYS
Text GLabel 8500 4713 0    40   BiDi ~ 0
I2S0_LRCK
Text GLabel 8500 4788 0    40   BiDi ~ 0
I2S0_BCLK
Text GLabel 8500 4863 0    40   BiDi ~ 0
I2S0_DOUT
Text GLabel 8500 4938 0    40   BiDi ~ 0
SD_A4
Text GLabel 8500 5013 0    40   BiDi ~ 0
SD_A5
Text GLabel 8500 5088 0    40   BiDi ~ 0
SD_A6
Text GLabel 8500 5163 0    40   BiDi ~ 0
SD_A7
Text GLabel 8500 5238 0    40   BiDi ~ 0
SD_A8
Text GLabel 8500 5313 0    40   BiDi ~ 0
SD_A9
Text GLabel 8500 5388 0    40   BiDi ~ 0
VDD_CORE
Text GLabel 8500 5463 0    40   BiDi ~ 0
SD_A11
Text GLabel 8500 5538 0    40   BiDi ~ 0
SD_A12
Text GLabel 8500 5613 0    40   BiDi ~ 0
SD_CKE_SRC
Text GLabel 8500 5688 0    40   BiDi ~ 0
SD_CLK_SRC
Text GLabel 8500 5763 0    40   BiDi ~ 0
SD_A3
Text GLabel 8500 5838 0    40   BiDi ~ 0
SD_A2
Text GLabel 8500 5913 0    40   BiDi ~ 0
SD_A1
Text GLabel 8500 5988 0    40   BiDi ~ 0
SD_A0
Text GLabel 8500 6063 0    40   BiDi ~ 0
VDD_CORE
Text GLabel 8500 6138 0    40   BiDi ~ 0
SD_A10
Text GLabel 8500 6213 0    40   BiDi ~ 0
SD_BA1
Text GLabel 8500 6288 0    40   BiDi ~ 0
SD_BA0
Text GLabel 8500 6363 0    40   BiDi ~ 0
SD_CS_SRC_N
Text GLabel 8500 6438 0    40   BiDi ~ 0
SD_RAS_SRC_N
Text GLabel 8500 6513 0    40   BiDi ~ 0
SD_CAS_SRC_N
Text GLabel 8500 6588 0    40   BiDi ~ 0
3V3_SYS
Text GLabel 8500 6663 0    40   BiDi ~ 0
SD_WE_SRC_N
Text GLabel 8500 6738 0    40   BiDi ~ 0
SD_UDQM
Text GLabel 8500 6813 0    40   BiDi ~ 0
SD_DQ15
Text GLabel 8500 6888 0    40   BiDi ~ 0
SD_DQ14
Text GLabel 8500 6963 0    40   BiDi ~ 0
SD_DQ13
Text GLabel 8500 7038 0    40   BiDi ~ 0
VDD_CORE
Text GLabel 8500 7113 0    40   BiDi ~ 0
SD_DQ12
Text GLabel 8500 7188 0    40   BiDi ~ 0
SD_DQ11
Text GLabel 8500 7263 0    40   BiDi ~ 0
SD_DQ10
Text GLabel 8500 7338 0    40   BiDi ~ 0
SD_DQ9
Text GLabel 8500 7413 0    40   BiDi ~ 0
SD_DQ8
Text GLabel 8500 7488 0    40   BiDi ~ 0
SD_LDQM
Text GLabel 8500 7563 0    40   BiDi ~ 0
3V3_SYS
Text GLabel 8500 7638 0    40   BiDi ~ 0
SD_DQ0
Text GLabel 8500 7713 0    40   BiDi ~ 0
SD_DQ1
Text GLabel 8500 7788 0    40   BiDi ~ 0
SD_DQ2
Text GLabel 8500 7863 0    40   BiDi ~ 0
SD_DQ3
Text GLabel 8500 7938 0    40   BiDi ~ 0
SD_DQ4
Text GLabel 8500 8013 0    40   BiDi ~ 0
SD_DQ5
Text GLabel 8500 8088 0    40   BiDi ~ 0
VDD_CORE
Text GLabel 8500 8163 0    40   BiDi ~ 0
SD_DQ6
Text GLabel 8500 8238 0    40   BiDi ~ 0
SD_DQ7
NoConn ~ 8500 8313
NoConn ~ 8500 8388
Text GLabel 8500 8463 0    40   BiDi ~ 0
GND
$Comp
L PISCIS_RevA:CRYSTAL4 Y1
U 1 1 66F0000A
P 2700 2400
F 0 "Y1" H 3050 2550 50  0000 C CNN
F 1 "X322524MOB4SI 24MHz" H 3050 2250 50  0000 C CNN
F 2 "PISCIS_Private:CRYSTAL-SMD_4P-L3.2-W2.5-BL" H 2700 2400 50  0001 C CNN
F 3 "" H 2700 2400 50  0001 C CNN
F 4 "C70590" H 2700 2400 50  0001 C CNN "LCSC Part"
	1    2700 2400
	1    0    0    -1
$EndComp
Text GLabel 2200 2350 2    40   BiDi ~ 0
XTAL_IN
Text GLabel 2200 2450 2    40   BiDi ~ 0
GND
Text GLabel 3200 2350 0    40   BiDi ~ 0
XTAL_OUT
Text GLabel 3200 2450 0    40   BiDi ~ 0
GND
$Comp
L PISCIS_RevA:CRYSTAL4 Y2
U 1 1 66F0000B
P 2700 4100
F 0 "Y2" H 3050 4250 50  0000 C CNN
F 1 "X252024MOB4SI 32.768kHz" H 3050 3950 50  0000 C CNN
F 2 "PISCIS_Private:CRYSTAL-SMD_4P-L2.5-W2.0-BL" H 2700 4100 50  0001 C CNN
F 3 "" H 2700 4100 50  0001 C CNN
F 4 "C371129" H 2700 4100 50  0001 C CNN "LCSC Part"
	1    2700 4100
	1    0    0    -1
$EndComp
Text GLabel 2200 4050 2    40   BiDi ~ 0
RTC_XTALI
Text GLabel 2200 4150 2    40   BiDi ~ 0
GND
Text GLabel 3200 4050 0    40   BiDi ~ 0
RTC_XTALO
Text GLabel 3200 4150 0    40   BiDi ~ 0
GND
$Comp
L PISCIS_RevA:L L1
U 1 1 66F0000C
P 2800 5800
F 0 "L1" H 3150 5950 50  0000 C CNN
F 1 "2.2uH core DCDC" H 3150 5650 50  0000 C CNN
F 2 "Inductor_SMD:L_1210_3225Metric_Pad1.42x2.65mm_HandSolder" H 2800 5800 50  0001 C CNN
F 3 "" H 2800 5800 50  0001 C CNN
F 4 "" H 2800 5800 50  0001 C CNN "LCSC Part"
	1    2800 5800
	1    0    0    -1
$EndComp
Text GLabel 2300 5800 2    40   BiDi ~ 0
DCDC_LX
Text GLabel 3300 5800 0    40   BiDi ~ 0
VDD_CORE
$Comp
L PISCIS_RevA:C C1
U 1 1 66F0000D
P 2500 7600
F 0 "C1" H 2850 7750 50  0000 C CNN
F 1 "10uF" H 2850 7450 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2500 7600 50  0001 C CNN
F 3 "" H 2500 7600 50  0001 C CNN
F 4 "" H 2500 7600 50  0001 C CNN "LCSC Part"
	1    2500 7600
	1    0    0    -1
$EndComp
Text GLabel 2000 7600 2    40   BiDi ~ 0
VDD_CORE
Text GLabel 3000 7600 0    40   BiDi ~ 0
GND
$Comp
L PISCIS_RevA:C C2
U 1 1 66F0000E
P 3400 7600
F 0 "C2" H 3750 7750 50  0000 C CNN
F 1 "10uF" H 3750 7450 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3400 7600 50  0001 C CNN
F 3 "" H 3400 7600 50  0001 C CNN
F 4 "" H 3400 7600 50  0001 C CNN "LCSC Part"
	1    3400 7600
	1    0    0    -1
$EndComp
Text GLabel 2900 7600 2    40   BiDi ~ 0
VDD_CORE
Text GLabel 3900 7600 0    40   BiDi ~ 0
GND
$Comp
L PISCIS_RevA:C C3
U 1 1 66F0000F
P 4300 7600
F 0 "C3" H 4650 7750 50  0000 C CNN
F 1 "100nF" H 4650 7450 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4300 7600 50  0001 C CNN
F 3 "" H 4300 7600 50  0001 C CNN
F 4 "" H 4300 7600 50  0001 C CNN "LCSC Part"
	1    4300 7600
	1    0    0    -1
$EndComp
Text GLabel 3800 7600 2    40   BiDi ~ 0
3V3_SYS
Text GLabel 4800 7600 0    40   BiDi ~ 0
GND
$Comp
L PISCIS_RevA:C C4
U 1 1 66F00010
P 5200 7600
F 0 "C4" H 5550 7750 50  0000 C CNN
F 1 "100nF" H 5550 7450 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5200 7600 50  0001 C CNN
F 3 "" H 5200 7600 50  0001 C CNN
F 4 "" H 5200 7600 50  0001 C CNN "LCSC Part"
	1    5200 7600
	1    0    0    -1
$EndComp
Text GLabel 4700 7600 2    40   BiDi ~ 0
3V3_SYS
Text GLabel 5700 7600 0    40   BiDi ~ 0
GND
$EndSCHEMATC
