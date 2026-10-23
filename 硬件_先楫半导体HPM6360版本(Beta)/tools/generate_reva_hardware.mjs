import fs from 'node:fs';
import path from 'node:path';

const root = process.cwd();
const out = path.join(root, 'hardware', 'rev_a');
fs.mkdirSync(out, { recursive: true });

const project = 'PISCIS-I_HPM6360_RevA';
let stamp = 0x66f00000;
const nextStamp = () => (++stamp).toString(16).toUpperCase();

const hpmPins = [
  'PA22','PA21','PA20','PA19','VDD_SOC','PA18','PA17','PA16','PA15','PA14','PA13','VIO_01','PA12','PA11','PA10','PA09','PA08','VDD_SOC','PA07','PA06','PA05','PA04','VIO_00','PA03','PA02','PA01','PA00','VDD_SOC','DCDC_SNS','DCDC_GND','DCDC_LP','DCDC_IN','PY07','PY06','PY05','PY04',
  'PY03','PY02','VDD_OTPCAP','PY01','VDD_PMCCAP','PY00','VPMC','RTC_XTALI','RTC_XTALO','VBAT','PZ07','PZ06','PZ05','PZ04','PZ03','PZ02','PZ01','PZ00','VDD_SOC','PC27','PC26','PC25','PC24','PC23','VIO_02','PC22','PC21','PC20','PC19','PC18','PC17','VDD_SOC','PC16','PC15','PC14','PC13',
  'PC12','PC11','PC10','PC09','VDD_SOC','PC08','PC07','PC06','PC05','PC04','PC03','VREFL','VREFH','VIO_02','VANA','XTALI','XTALO','VDD_SOC','USBVBUS','USB_DP','USB_DM','VIO_01','PC02','PC01','PC00','PB31','PB30','PB29','PB28','PB27','PB26','VDD_SOC','PB25','PB24','PB23','PB22',
  'PB21','PB20','PB19','PB18','VDD_SOC','PB17','PB16','PB15','PB14','PB13','PB12','VIO_01','PB11','PB10','PB09','PB08','PB07','VDD_SOC','PB06','PB05','PB04','PB03','PB02','PB01','VIO_01','PB00','PA31','PA30','PA29','PA28','PA27','VDD_SOC','PA26','PA25','PA24','PA23','VSS'
];

const sdramPins = [
  'VDD','DQ0','VDDQ','DQ1','DQ2','VSSQ','DQ3','DQ4','VDDQ','DQ5','DQ6','VSSQ','DQ7','VDD','LDQM','WE_N','CAS_N','RAS_N','CS_N','BA0','BA1','A10','A0','A1','A2','A3','VDD','VSS','A4','A5','A6','A7','A8','A9','A11','A12','CKE','CLK','UDQM','NC','VSS','DQ8','VDDQ','DQ9','DQ10','VSSQ','DQ11','DQ12','VDDQ','DQ13','DQ14','VSSQ','DQ15','VSS'
];

const defs = {
  HPM6360IPA2: hpmPins,
  W9825G6KH_6I: sdramPins,
  GD25Q128ESIGR: ['CS_N','IO1','IO2','GND','IO0','CLK','IO3','VCC'],
  ES8388: ['MCLK','DVDD','PVDD','DGND','SCLK','DSDIN','LRCK','ASDOUT','NC','VREF','ROUT1','LOUT1','HPGND','ROUT2','LOUT2','HPVDD','AVDD','AGND','ADCVREF','VMID','MICBIAS','MICIN','RINPUT2','LINPUT2','RINPUT1','LINPUT1','CCLK','CDATA'],
  FUSB303BTMX: ['CC1','CC2','PORT_DEBUG_N','VBUS_DET','ADDR_ORIENT','INT_N','SDA','SCL','ID','GND','EN_N','VDD'],
  MT9700_N: ['VOUT','GND','SET','EN','VIN'],
  SM8082AAAC: ['EN','GND','LX','IN','FB'],
  PAM8302A: ['SHDN','INP','INN','GND','OUTN','VDD','OUTP','NC'],
  ESD5304D_10_TR: ['IO1','IO2','GND1','IO3','IO4','NC1','NC2','GND2','NC3','NC4'],
  ESD5311N_2_TR: ['IO','GND'],
  MICRO_SD: ['DAT2','DAT3','CMD','VDD','CLK','VSS','DAT0','DAT1','SW1','SW2'],
  USB_A: ['VBUS','DM','DP','GND'],
  USB_C_PWR: ['VBUS','VBUS','CC1','CC2','GND','GND'],
  PJ320D: ['TIP_L','RING_R','SLEEVE_GND','DETECT'],
  CONN_2X08: Array.from({length: 16}, (_, i) => `PIN${i + 1}`),
  TAGCONNECT_10: ['VTREF','TMS','GND','TCK','GND','TDO','KEY','TDI','GND','TRST'],
  MCP3208: ['CH0','CH1','CH2','CH3','CH4','CH5','CH6','CH7','DGND','CS_N','DIN','DOUT','CLK','AGND','VREF','VDD'],
  HC165: ['PL_N','CP','D4','D5','D6','D7','Q7','GND','Q7_N','DS','D0','D1','D2','D3','CE_N','VCC'],
  CONN3: ['VCC','SIG1','SIG2'],
  CRYSTAL4: ['X1','GND','X2','GND'],
  R: ['1','2'], C: ['1','2'], L: ['1','2'], FUSE: ['1','2'], DIODE: ['K','A'], SW: ['1','2'], TP: ['1'],
};

const fp = {
  HPM6360IPA2: 'PISCIS_Private:HPM6360IPA2_eLQFP144L_20x20mm_P0.5mm_EP5x5mm',
  W9825G6KH_6I: 'PISCIS_Private:TSOP-54_L22.2-W10.2-P0.80-LS11.8-BL',
  GD25Q128ESIGR: 'PISCIS_Private:SOP-8_L5.3-W5.3-P1.27-LS8.0-BL',
  ES8388: 'PISCIS_Private:QFN-28_L4.0-W4.0-P0.45-BL-EP',
  FUSB303BTMX: 'PISCIS_Private:X2QFN-12_L1.6-W1.6-P0.40-BL',
  MT9700_N: 'PISCIS_Private:SOT-23-5_L2.9-W1.6-P0.95-LS2.9-BL',
  SM8082AAAC: 'PISCIS_Private:SOT-23-5_L3.0-W1.7-P0.95-LS2.8-BL',
  PAM8302A: 'Package_SO:MSOP-8_3x3mm_P0.65mm',
  ESD5304D_10_TR: 'PISCIS_Private:UDFN-10_L2.5-W1.0-P0.50-BL',
  ESD5311N_2_TR: 'PISCIS_Private:X1-DFN1006-2_L1.0-W0.6_ESD5311N-2-TR',
  MICRO_SD: 'Connector_Card:microSD_HC_Hirose_DM3AT-SF-PEJM5',
  USB_A: 'Connector_USB:USB_A_Stewart_SS-52100-001_Horizontal',
  USB_C_PWR: 'PISCIS_Private:USB-C_SMD-TYPE-C-31-M-12_1',
  PJ320D: 'PISCIS_Private:AUDIO-SMD_XKB_PJ-320D-B',
  CONN_2X08: 'Connector_IDC:IDC-Header_2x08_P2.54mm_Vertical',
  TAGCONNECT_10: 'Connector_Tag-Connect:Tag-Connect_TC2050-IDC-NL_2x05_P1.27mm_Vertical',
  MCP3208: 'Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm', HC165: 'Package_SO:SOIC-16_3.9x9.9mm_P1.27mm',
  CONN3: 'Connector_JST:JST_PH_S3B-PH-K_1x03_P2.00mm_Horizontal',
  CRYSTAL4: 'PISCIS_Private:CRYSTAL-SMD_4P-L3.2-W2.5-BL',
  R: 'Resistor_SMD:R_0805_2012Metric', C: 'Capacitor_SMD:C_0805_2012Metric',
  L: 'Inductor_SMD:L_1210_3225Metric_Pad1.42x2.65mm_HandSolder', FUSE: 'Fuse:Fuse_1206_3216Metric',
  DIODE: 'Diode_SMD:D_SMA_Handsoldering', SW: 'PISCIS_Private:SW_SPST_TS-1088-xR020', TP: 'TestPoint:TestPoint_Plated_Hole_D2.0mm',
};

const lcsc = {
  W9825G6KH_6I: 'C97572', GD25Q128ESIGR: 'C2928891', ES8388: 'C365736',
  FUSB303BTMX: 'C895444', MT9700_N: 'C42441843', PJ320D: 'C2884940',
  ESD5304D_10_TR: 'C153722', ESD5311N_2_TR: 'C153721',
  CRYSTAL4: 'C70590', USB_C_PWR: 'C165948',
};

function legacySymbol(name, pins) {
  const split = Math.ceil(pins.length / 2);
  const pitch = pins.length > 80 ? 75 : 100;
  const maxSide = Math.max(split, pins.length - split);
  const top = Math.floor((maxSide - 1) * pitch / 2);
  const halfH = top + 100;
  let draw = `S -300 ${halfH} 300 -${halfH} 0 1 10 f\n`;
  pins.forEach((p, i) => {
    const left = i < split;
    const row = left ? i : i - split;
    const count = left ? split : pins.length - split;
    const y = Math.floor((count - 1) * pitch / 2) - row * pitch;
    const x = left ? -500 : 500;
    const orient = left ? 'R' : 'L';
    draw += `X ${p} ${i + 1} ${x} ${y} 200 ${orient} 40 40 1 1 P\n`;
  });
  return `#\n# ${name}\n#\nDEF ${name} U 0 30 Y Y 1 F N\nF0 "U" 0 ${halfH + 120} 50 H V C CNN\nF1 "${name}" 0 -${halfH + 120} 50 H V C CNN\nDRAW\n${draw}ENDDRAW\nENDDEF\n`;
}

let cache = 'EESchema-LIBRARY Version 2.4\n#encoding utf-8\n';
for (const [name, pins] of Object.entries(defs)) cache += legacySymbol(name, pins);
cache += '#\n#End Library\n';
fs.writeFileSync(path.join(out, `${project}-cache.lib`), cache);
// Keep the nickname identical to the prefix emitted in every legacy `L` line.
// A mismatch here leaves KiCad waiting in the legacy symbol rescue/load stage.
fs.writeFileSync(path.join(out, 'sym-lib-table'), `(sym_lib_table
  (version 7)
  (lib (name "PISCIS_Private") (type "KiCad") (uri "\${KIPRJMOD}/../libraries/private/PISCIS_Private.kicad_sym") (options "") (descr "PISCIS-I project-private symbols, including PJ-320D-B-SMT"))
  (lib (name "PISCIS_RevA") (type "Legacy") (uri "\${KIPRJMOD}/${project}-cache.lib") (options "") (descr "Generated Rev.A project cache library"))
)
`);
fs.writeFileSync(path.join(out, 'fp-lib-table'), `(fp_lib_table
  (version 7)
  (lib (name "PISCIS_Private") (type "KiCad") (uri "\${KIPRJMOD}/../libraries/private/PISCIS_Private.pretty") (options "") (descr "PISCIS-I project-private footprints, including AUDIO-SMD_XKB_PJ-320D-B"))
)
`);

const sheetHeader = (number, count, title) => `EESchema Schematic File Version 4\nLIBS:${project}-cache\nEELAYER 29 0\nEELAYER END\n$Descr A3 16535 11693\nencoding utf-8\nSheet ${number} ${count}\nTitle "${title}"\nDate "2026-09-27"\nRev "A"\nComp "PISCIS-I"\nComment1 "ENGINEERING PROTOTYPE - VERIFY BEFORE FABRICATION"\nComment2 "HPM6360 electronic instrument evaluation board"\nComment3 "Generated from locked Rev.A architecture"\nComment4 ""\n$EndDescr\n`;

class Sheet {
  constructor(number, count, title) { this.text = sheetHeader(number, count, title); this.used = new Set(); }
  note(x, y, text, size = 80) { this.text += `Text Notes ${x} ${y} 0    ${size}   ~ 12\n${text}\n`; }
  component(symbol, ref, value, x, y, footprint = fp[symbol] ?? '', fields = {}) {
    const ts = nextStamp();
    this.text += `$Comp\nL PISCIS_RevA:${symbol} ${ref}\nU 1 1 ${ts}\nP ${x} ${y}\nF 0 "${ref}" H ${x + 350} ${y + 150} 50  0000 C CNN\nF 1 "${value}" H ${x + 350} ${y - 150} 50  0000 C CNN\nF 2 "${footprint}" H ${x} ${y} 50  0001 C CNN\nF 3 "" H ${x} ${y} 50  0001 C CNN\nF 4 "${fields.LCSC ?? lcsc[symbol] ?? ''}" H ${x} ${y} 50  0001 C CNN "LCSC Part"\n\t1    ${x} ${y}\n\t1    0    0    -1\n$EndComp\n`;
    return {symbol, ref, x, y};
  }
  pinXY(comp, pinNumber) {
    const pins = defs[comp.symbol];
    const split = Math.ceil(pins.length / 2);
    const pitch = pins.length > 80 ? 75 : 100;
    const index = pinNumber - 1;
    const left = index < split;
    const row = left ? index : index - split;
    const count = left ? split : pins.length - split;
    const py = Math.floor((count - 1) * pitch / 2) - row * pitch;
    return [comp.x + (left ? -500 : 500), comp.y - py];
  }
  connect(comp, pin, net, dir = 'BiDi') {
    const [x, y] = this.pinXY(comp, pin);
    this.used.add(`${comp.ref}:${pin}`);
    this.text += `Text GLabel ${x} ${y} ${x < comp.x ? 2 : 0}    40   ${dir} ~ 0\n${net}\n`;
  }
  nc(comp, pin) { const [x,y] = this.pinXY(comp,pin); this.used.add(`${comp.ref}:${pin}`); this.text += `NoConn ~ ${x} ${y}\n`; }
  finish() { this.text += '$EndSCHEMATC\n'; return this.text; }
}

const sheets = [
  ['01_MCU', 'MCU, clocks and core power'], ['02_Power', 'USB-C 5V input and system power'],
  ['03_Memory', '32 MiB SDRAM and 16 MiB QSPI NOR'], ['04_Storage', '4-bit SDIO microSD'],
  ['05_USB_Host', 'USB 2.0 Type-A host and VBUS switch'], ['06_Audio', 'ES8388 headphone and mono speaker audio'],
  ['07_Control', 'Control-board expansion interface'], ['08_Debug', 'JTAG, UART, reset and boot']
];

let rootSch = sheetHeader(1, sheets.length + 1, 'PISCIS-I HPM6360 Electronic Instrument Evaluation Board');
rootSch += 'Text Notes 900 700 0    140  ~ 28\nREV.A FUNCTIONAL ARCHITECTURE\n';
rootSch += 'Text Notes 900 1050 0    70   ~ 12\nUSB-C 5V power-only | USB-A Host/MIDI | 32 MiB SDRAM | microSD samples | ES8388 audio | 8+8 control expansion\n';
sheets.forEach(([file, title], i) => {
  const col = i % 2, row = Math.floor(i / 2);
  const x = 1100 + col * 6500, y = 1700 + row * 2100;
  rootSch += `$Sheet\nS ${x} ${y} 5000 1400\nU ${nextStamp()}\nF0 "${title}" 60\nF1 "${file}.sch" 60\n$EndSheet\n`;
});
rootSch += '$EndSCHEMATC\n';
fs.writeFileSync(path.join(out, `${project}.sch`), rootSch);

const pinByName = new Map(hpmPins.map((name, i) => [name, i + 1]));
const mcuNet = {
  PA20:'BOOT_0', PA21:'BOOT_1',
  PA00:'QSPI_CS_N', PA01:'QSPI_IO1', PA02:'QSPI_IO2', PA03:'QSPI_IO0', PA04:'QSPI_CLK', PA05:'QSPI_IO3',
  PA08:'SD_DAT2', PA09:'SD_DAT3', PA10:'SD_CMD', PA11:'SD_CLK', PA12:'SD_DAT0', PA13:'SD_DAT1', PA14:'SD_CD_N',
  PA25:'SD_DQ7', PA26:'SD_DQ6', PA27:'SD_DQ5', PA28:'SD_DQ4', PA29:'SD_DQ3', PA30:'SD_DQ2', PA31:'SD_DQ1', PB00:'SD_DQ0',
  PB01:'SD_LDQM', PB02:'SD_DQ8', PB03:'SD_DQ9', PB04:'SD_DQ10', PB05:'SD_DQ11', PB06:'SD_DQ12', PB07:'SD_DQ13', PB08:'SD_DQ14', PB09:'SD_DQ15', PB10:'SD_UDQM',
  PB11:'SD_WE_SRC_N', PB12:'SD_CAS_SRC_N', PB13:'SD_RAS_SRC_N', PB14:'SD_CS_SRC_N', PB15:'SD_BA0', PB16:'SD_BA1', PB17:'SD_A10', PB18:'SD_A0', PB19:'SD_A1', PB20:'SD_A2', PB21:'SD_A3', PB22:'SD_CLK_SRC', PB23:'SD_CKE_SRC', PB24:'SD_A12', PB25:'SD_A11', PB26:'SD_A9', PB27:'SD_A8', PB28:'SD_A7', PB29:'SD_A6', PB30:'SD_A5', PB31:'SD_A4',
  PC00:'I2S0_DOUT', PC01:'I2S0_BCLK', PC02:'I2S0_LRCK', PC03:'I2S0_MCLK', PC04:'I2S0_DIN', PC13:'CTRL_I2C_SCL', PC14:'CTRL_I2C_SDA',
  PC18:'ADC_CS_N', PC19:'CTRL_SPI_MISO', PC20:'CTRL_SPI_SCK', PC21:'CTRL_SPI_MOSI', PC15:'ENC_LOAD_N', PC17:'ENC_DATA', PC16:'CTRL_PRESENT_N', PC23:'CTRL_IRQ_N',
  PZ03:'TYPEC_INT_N', PZ04:'USBH_EN', PZ05:'USBH_OC_N', PZ06:'HP_DETECT_N', PZ07:'AMP_SHDN', PZ01:'RESET_N',
  PY00:'JTAG_TDO', PY01:'JTAG_TDI', PY02:'JTAG_TCK', PY03:'JTAG_TMS', PY04:'JTAG_TRST', PY06:'UART0_TXD', PY07:'UART0_RXD',
  USB_DP:'USBH_DP', USB_DM:'USBH_DM', USBVBUS:'5V_SYS', XTALI:'XTAL_IN', XTALO:'XTAL_OUT', RTC_XTALI:'RTC_XTALI', RTC_XTALO:'RTC_XTALO',
  DCDC_IN:'3V3_SYS', DCDC_GND:'GND', DCDC_LP:'DCDC_LX', DCDC_SNS:'VDD_CORE', VANA:'3V3_A', VREFH:'3V3_A', VREFL:'GND', VPMC:'3V3_SYS', VBAT:'3V3_SYS', VDD_OTPCAP:'OTP_CAP', VDD_PMCCAP:'PMC_CAP',
};

{
  const s = new Sheet(2, 9, sheets[0][1]); s.note(800,550,'HPM6360IPA2 / clocks / internal core DCDC');
  const u = s.component('HPM6360IPA2','U1','HPM6360IPA2',8000,5800,fp.HPM6360IPA2,{LCSC:'HPM6360IPA2'});
  for (let p = 1; p <= hpmPins.length; p++) {
    const name = hpmPins[p-1];
    let net = mcuNet[name];
    if (name === 'VSS' || name === 'DCDC_GND') net = 'GND';
    if (name.startsWith('VIO_')) net = '3V3_SYS';
    if (name === 'VDD_SOC') net = 'VDD_CORE';
    if (net) s.connect(u,p,net); else s.nc(u,p);
  }
  const y1=s.component('CRYSTAL4','Y1','X322524MOB4SI 24MHz',2700,2400,'PISCIS_Private:CRYSTAL-SMD_4P-L3.2-W2.5-BL',{LCSC:'C70590'}); s.connect(y1,1,'XTAL_IN'); s.connect(y1,2,'GND'); s.connect(y1,3,'XTAL_OUT'); s.connect(y1,4,'GND');
  const y2=s.component('CRYSTAL4','Y2','X252024MOB4SI 32.768kHz',2700,4100,'PISCIS_Private:CRYSTAL-SMD_4P-L2.5-W2.0-BL',{LCSC:'C371129'}); s.connect(y2,1,'RTC_XTALI'); s.connect(y2,2,'GND'); s.connect(y2,3,'RTC_XTALO'); s.connect(y2,4,'GND');
  const l=s.component('L','L1','2.2uH core DCDC',2800,5800); s.connect(l,1,'DCDC_LX'); s.connect(l,2,'VDD_CORE');
  ['10uF','10uF','100nF','100nF'].forEach((v,i)=>{const c=s.component('C',`C${i+1}`,v,2500+i*900,7600);s.connect(c,1,i<2?'VDD_CORE':'3V3_SYS');s.connect(c,2,'GND');});
  fs.writeFileSync(path.join(out,'01_MCU.sch'),s.finish());
}

{
  const s=new Sheet(3,9,sheets[1][1]); s.note(800,550,'5V/3A Type-C sink, protected 5V system rail and 3.3V buck');
  const j=s.component('USB_C_PWR','J1','USB-C POWER ONLY',2200,2400,fp.USB_C_PWR,{LCSC:'C165948'}); [1,2].forEach(p=>s.connect(j,p,'VBUS_RAW')); s.connect(j,3,'CC1'); s.connect(j,4,'CC2'); [5,6].forEach(p=>s.connect(j,p,'GND'));
  const fusb=s.component('FUSB303BTMX','U2','FUSB303BTMX',5200,2300); s.connect(fusb,1,'CC1');s.connect(fusb,2,'CC2');s.connect(fusb,3,'GND');s.connect(fusb,4,'VBUS_RAW');s.connect(fusb,5,'GND');s.connect(fusb,6,'TYPEC_INT_N');s.connect(fusb,7,'CTRL_I2C_SDA');s.connect(fusb,8,'CTRL_I2C_SCL');s.connect(fusb,9,'GND');s.connect(fusb,10,'GND');s.connect(fusb,11,'GND');s.connect(fusb,12,'3V3_SYS');
  const f=s.component('FUSE','F1','3A resettable',2900,4300);s.connect(f,1,'VBUS_RAW');s.connect(f,2,'5V_SYS');
  const tvs=s.component('DIODE','D1','SMBJ5.0A',4700,4300,'PISCIS_Private:SMB_L4.6-W3.6-LS5.3-RD',{LCSC:'C340697'});s.connect(tvs,1,'5V_SYS');s.connect(tvs,2,'GND');
  const cc1=s.component('ESD5311N_2_TR','D3','ESD5311N-2/TR CC1',2800,6500);s.connect(cc1,1,'CC1');s.connect(cc1,2,'GND');
  const cc2=s.component('ESD5311N_2_TR','D4','ESD5311N-2/TR CC2',4300,6500);s.connect(cc2,1,'CC2');s.connect(cc2,2,'GND');
  const buck=s.component('SM8082AAAC','U3','SM8082AAAC 3V3 BUCK',7600,4200);s.connect(buck,1,'5V_SYS');s.connect(buck,2,'GND');s.connect(buck,3,'BUCK_SW');s.connect(buck,4,'5V_SYS');s.connect(buck,5,'BUCK_FB');
  const l=s.component('L','L2','4.7uH',9700,3900);s.connect(l,1,'BUCK_SW');s.connect(l,2,'3V3_SYS');
  const r1=s.component('R','R1','330k',9700,4700);s.connect(r1,1,'3V3_SYS');s.connect(r1,2,'BUCK_FB'); const r2=s.component('R','R2','100k',11100,4700);s.connect(r2,1,'BUCK_FB');s.connect(r2,2,'GND');
  const fb=s.component('L','FB1','Ferrite 600R@100MHz',8200,6500);s.connect(fb,1,'3V3_SYS');s.connect(fb,2,'3V3_A');
  ['22uF','22uF','100nF','10uF','100nF'].forEach((v,i)=>{const c=s.component('C',`C${10+i}`,v,2600+i*1500,8300);s.connect(c,1,i<2?'5V_SYS':i<4?'3V3_SYS':'3V3_A');s.connect(c,2,'GND');});
  for (const [i,n] of ['5V_SYS','3V3_SYS','3V3_A','VDD_CORE'].entries()){const t=s.component('TP',`TP${i+1}`,n,11500,6200+i*800);s.connect(t,1,n);}
  fs.writeFileSync(path.join(out,'02_Power.sch'),s.finish());
}

{
  const s=new Sheet(4,9,sheets[2][1]); s.note(800,550,'FEMC SDRAM follows official hpm6300evk pinmux; QSPI uses XPI0 Port A');
  const ram=s.component('W9825G6KH_6I','U4','W9825G6KH-6I',10200,4800,fp.W9825G6KH_6I,{LCSC:'C97572'});
  const ramNets={}; for(let i=0;i<16;i++)ramNets[`DQ${i}`]=`SD_DQ${i}`; for(let i=0;i<=12;i++)ramNets[`A${i}`]=`SD_A${i}`; Object.assign(ramNets,{VDD:'3V3_SYS',VDDQ:'3V3_SYS',VSS:'GND',VSSQ:'GND',LDQM:'SD_LDQM',UDQM:'SD_UDQM',WE_N:'SD_WE_N',CAS_N:'SD_CAS_N',RAS_N:'SD_RAS_N',CS_N:'SD_CS_N',BA0:'SD_BA0',BA1:'SD_BA1',CKE:'SD_CKE',CLK:'SD_CLK'});
  sdramPins.forEach((n,i)=>{if(n==='NC')s.nc(ram,i+1);else s.connect(ram,i+1,ramNets[n]);});
  const series=[['R10','22R','SD_CLK_SRC','SD_CLK'],['R11','22R','SD_CKE_SRC','SD_CKE'],['R12','22R','SD_CS_SRC_N','SD_CS_N'],['R13','22R','SD_RAS_SRC_N','SD_RAS_N'],['R14','22R','SD_CAS_SRC_N','SD_CAS_N'],['R15','22R','SD_WE_SRC_N','SD_WE_N']];
  series.forEach(([ref,val,a,b],i)=>{const r=s.component('R',ref,val,2500,1700+i*900);s.connect(r,1,a);s.connect(r,2,b);});
  const flash=s.component('GD25Q128ESIGR','U5','GD25Q128ESIGR',4400,7600,fp.GD25Q128ESIGR,{LCSC:'C2928891'}); ['QSPI_CS_N','QSPI_IO1','QSPI_IO2','GND','QSPI_IO0','QSPI_CLK','QSPI_IO3','3V3_SYS'].forEach((n,i)=>s.connect(flash,i+1,n));
  const memtp=s.component('TP','TP10','SD_CLK',6200,7600,'TestPoint:TestPoint_Pad_D1.0mm');s.connect(memtp,1,'SD_CLK');
  for(let i=0;i<8;i++){const c=s.component('C',`C${20+i}`,'100nF',7600+(i%4)*1200,8500+Math.floor(i/4)*900);s.connect(c,1,'3V3_SYS');s.connect(c,2,'GND');}
  fs.writeFileSync(path.join(out,'03_Memory.sch'),s.finish());
}

{
  const s=new Sheet(5,9,sheets[3][1]); s.note(800,550,'4-bit SDIO microSD, removable sample storage');
  const j=s.component('MICRO_SD','J2','microSD 4-bit SDIO',5200,4300); ['SD_DAT2','SD_DAT3','SD_CMD','3V3_SYS','SD_CLK_CARD','GND','SD_DAT0','SD_DAT1','SD_CD_N','GND'].forEach((n,i)=>s.connect(j,i+1,n));
  ['SD_DAT0','SD_DAT1','SD_DAT2','SD_DAT3','SD_CMD'].forEach((n,i)=>{const r=s.component('R',`R${30+i}`,'47k pull-up',9000,2200+i*850);s.connect(r,1,'3V3_SYS');s.connect(r,2,n);});
  const rs=s.component('R','R35','22R CLK source',9000,7000);s.connect(rs,1,'SD_CLK');s.connect(rs,2,'SD_CLK_CARD');
  const c=s.component('C','C30','10uF',3400,7200);s.connect(c,1,'3V3_SYS');s.connect(c,2,'GND'); const c2=s.component('C','C31','100nF',4700,7200);s.connect(c2,1,'3V3_SYS');s.connect(c2,2,'GND');
  const sdesd=s.component('ESD5304D_10_TR','D8','ESD5304D-10/TR SD DAT',12000,4100);['SD_DAT0','SD_DAT1','GND','SD_DAT2','SD_DAT3'].forEach((n,i)=>s.connect(sdesd,i+1,n));s.nc(sdesd,6);s.nc(sdesd,7);s.connect(sdesd,8,'GND');s.nc(sdesd,9);s.nc(sdesd,10);
  const cmdesd=s.component('ESD5311N_2_TR','D9','ESD5311N-2/TR SD CMD',11200,6100);s.connect(cmdesd,1,'SD_CMD');s.connect(cmdesd,2,'GND');
  const clkesd=s.component('ESD5311N_2_TR','D10','ESD5311N-2/TR SD CLK',12800,6100);s.connect(clkesd,1,'SD_CLK_CARD');s.connect(clkesd,2,'GND');
  [['TP11','SD_CLK_CARD'],['TP12','SD_CMD']].forEach(([ref,n],i)=>{const t=s.component('TP',ref,n,6500+i*1400,7600,'TestPoint:TestPoint_Pad_D1.0mm');s.connect(t,1,n);});
  s.note(8200,8400,'Place low-capacitance ESD devices at card connector; SD_CLK series resistor at MCU.');
  fs.writeFileSync(path.join(out,'04_Storage.sch'),s.finish());
}

{
  const s=new Sheet(6,9,sheets[4][1]); s.note(800,550,'USB0 fixed Host; Type-A VBUS is current-limited by MT9700-N');
  const sw=s.component('MT9700_N','U6','MT9700-N',4600,3800);s.connect(sw,1,'USBH_5V');s.connect(sw,2,'GND');s.connect(sw,3,'USBH_OC_N');s.connect(sw,4,'USBH_EN');s.connect(sw,5,'5V_SYS');
  const j=s.component('USB_A','J3','USB-A HOST / MIDI',8500,3900);s.connect(j,1,'USBH_5V');s.connect(j,2,'USBH_DM');s.connect(j,3,'USBH_DP');s.connect(j,4,'GND');
  const tvs=s.component('DIODE','D2','SMBJ5.0A',6500,6500,'PISCIS_Private:SMB_L4.6-W3.6-LS5.3-RD',{LCSC:'C340697'});s.connect(tvs,1,'USBH_5V');s.connect(tvs,2,'GND');
  const esd=s.component('ESD5304D_10_TR','D5','ESD5304D-10/TR',10800,3900);s.connect(esd,1,'USBH_DM');s.connect(esd,2,'USBH_DP');s.connect(esd,3,'GND');s.nc(esd,4);s.nc(esd,5);s.nc(esd,6);s.nc(esd,7);s.connect(esd,8,'GND');s.nc(esd,9);s.nc(esd,10);
  [['TP13','USBH_DP'],['TP14','USBH_DM'],['TP15','USBH_OC_N']].forEach(([ref,n],i)=>{const t=s.component('TP',ref,n,8500+i*1300,8000,'TestPoint:TestPoint_Pad_D1.0mm');s.connect(t,1,n);});
  s.note(7500,6500,'Place ESD5304D within a few mm of J3; route USB D+/D- as 90-ohm differential pair.');
  fs.writeFileSync(path.join(out,'05_USB_Host.sch'),s.finish());
}

{
  const s=new Sheet(7,9,sheets[5][1]); s.note(800,550,'ES8388 stereo headphone + resistor-summed mono Class-D speaker');
  const codec=s.component('ES8388','U7','ES8388',5000,4300,fp.ES8388,{LCSC:'C365736'});
  const nets=['I2S0_MCLK','3V3_SYS','3V3_SYS','GND','I2S0_BCLK','I2S0_DOUT','I2S0_LRCK','I2S0_DIN','NC_ES8388','CODEC_VREF','HP_R_CODEC','HP_L_CODEC','GND','LINE_R','LINE_L','3V3_A','3V3_A','GND','CODEC_ADCVREF','CODEC_VMID','MICBIAS','NC_MIC','GND','GND','GND','GND','CTRL_I2C_SCL','CTRL_I2C_SDA'];
  nets.forEach((n,i)=>n.startsWith('NC_')?s.nc(codec,i+1):s.connect(codec,i+1,n));
  const hp=s.component('PJ320D','J4','XKB PJ-320D-B-SMT',9900,3000,fp.PJ320D,{LCSC:'C2884940'});s.connect(hp,1,'HP_L');s.connect(hp,2,'HP_R');s.connect(hp,3,'GND');s.connect(hp,4,'HP_DETECT_N');
  const hplc=s.component('C','C44','220uF HP coupling L',7600,2400);s.connect(hplc,1,'HP_L_CODEC');s.connect(hplc,2,'HP_L_AC'); const hplr=s.component('R','R42','33R',8700,2400);s.connect(hplr,1,'HP_L_AC');s.connect(hplr,2,'HP_L');
  const hprc=s.component('C','C45','220uF HP coupling R',7600,3300);s.connect(hprc,1,'HP_R_CODEC');s.connect(hprc,2,'HP_R_AC'); const hprr=s.component('R','R43','33R',8700,3300);s.connect(hprr,1,'HP_R_AC');s.connect(hprr,2,'HP_R');
  const hplesd=s.component('ESD5311N_2_TR','D6','ESD5311N-2/TR HP_L',11200,2600);s.connect(hplesd,1,'HP_L');s.connect(hplesd,2,'GND');
  const hpresd=s.component('ESD5311N_2_TR','D7','ESD5311N-2/TR HP_R',12600,2600);s.connect(hpresd,1,'HP_R');s.connect(hpresd,2,'GND');
  const rl=s.component('R','R40','10k mono sum L',7900,5200);s.connect(rl,1,'LINE_L');s.connect(rl,2,'AMP_IN'); const rr=s.component('R','R41','10k mono sum R',7900,6100);s.connect(rr,1,'LINE_R');s.connect(rr,2,'AMP_IN');
  const amp=s.component('PAM8302A','U8','PAM8302AASCR',10500,6200);s.connect(amp,1,'AMP_SHDN');s.connect(amp,2,'AMP_IN');s.connect(amp,3,'CODEC_VMID');s.connect(amp,4,'GND');s.connect(amp,5,'SPK_N');s.connect(amp,6,'5V_SYS');s.connect(amp,7,'SPK_P');s.nc(amp,8);
  const spk=s.component('R','J5','JST-PH 2P / 4R 3W SPEAKER',13200,6200,'PISCIS_Private:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal');s.connect(spk,1,'SPK_P');s.connect(spk,2,'SPK_N');
  [['C40','1uF','CODEC_VREF'],['C41','4.7uF','CODEC_VMID'],['C42','10uF','3V3_A'],['C43','470uF','5V_SYS']].forEach(([r,v,n],i)=>{const c=s.component('C',r,v,2400+i*1300,8200);s.connect(c,1,n);s.connect(c,2,'GND');});
  [['R44','4.7k','CTRL_I2C_SCL'],['R45','4.7k','CTRL_I2C_SDA']].forEach(([r,v,n],i)=>{const x=s.component('R',r,v,10800+i*1500,8200);s.connect(x,1,'3V3_SYS');s.connect(x,2,n);});
  [['TP16','I2S0_BCLK'],['TP17','I2S0_LRCK'],['TP18','AMP_SHDN']].forEach(([ref,n],i)=>{const t=s.component('TP',ref,n,3000+i*1500,9600,'TestPoint:TestPoint_Pad_D1.0mm');s.connect(t,1,n);});
  s.note(8500,9000,'Verify PAM8302A ordering-code pinout before procurement. SPK_P/SPK_N are BTL and must never connect to GND.');
  fs.writeFileSync(path.join(out,'06_Audio.sch'),s.finish());
}

{
  const s=new Sheet(8,9,sheets[6][1]); s.note(800,550,'2x8 keyed IDC control board interface, cable <=20 cm');
  const j=s.component('CONN_2X08','J6','CTRL_EXP_2X08',5500,4300); const pinNets=['3V3_CTRL','GND','5V_CTRL_SW','GND','CTRL_SPI_SCK','GND','CTRL_SPI_MOSI','CTRL_SPI_MISO','ADC_CS_N','ENC_LOAD_N','ENC_DATA','CTRL_PRESENT_N','CTRL_I2C_SCL','CTRL_I2C_SDA','CTRL_IRQ_N','GND'];pinNets.forEach((n,i)=>s.connect(j,i+1,n));
  const sw=s.component('MT9700_N','U9','MT9700-N CTRL 5V',9500,2800);s.connect(sw,1,'5V_CTRL_SW');s.connect(sw,2,'GND');s.connect(sw,3,'CTRL_5V_OC_N');s.connect(sw,4,'CTRL_5V_EN');s.connect(sw,5,'5V_SYS');
  const fb=s.component('L','FB2','Ferrite 600R@100MHz',9300,5000);s.connect(fb,1,'3V3_SYS');s.connect(fb,2,'3V3_CTRL');
  [['R50','4.7k','CTRL_I2C_SCL'],['R51','4.7k','CTRL_I2C_SDA'],['R52','10k','CTRL_PRESENT_N'],['R53','10k','CTRL_IRQ_N']].forEach(([r,v,n],i)=>{const x=s.component('R',r,v,9500+i*1200,6900);s.connect(x,1,'3V3_CTRL');s.connect(x,2,n);});
  [['TP19','CTRL_SPI_SCK'],['TP20','CTRL_IRQ_N']].forEach(([ref,n],i)=>{const t=s.component('TP',ref,n,9500+i*1500,8300,'TestPoint:TestPoint_Pad_D1.0mm');s.connect(t,1,n);});
  s.note(800,8600,'Daughter board: MCP3208 (8x 10k pots, each 1k/100nF) + 2x 74HC165 (16 encoder A/B inputs).');
  fs.writeFileSync(path.join(out,'07_Control.sch'),s.finish());
}

{
  const s=new Sheet(9,9,sheets[7][1]); s.note(800,550,'JTAG / UART / reset / boot service interface');
  const j=s.component('TAGCONNECT_10','J7','TAG-CONNECT JTAG',4800,3600); ['3V3_SYS','JTAG_TMS','GND','JTAG_TCK','GND','JTAG_TDO','NC_KEY','JTAG_TDI','GND','JTAG_TRST'].forEach((n,i)=>n==='NC_KEY'?s.nc(j,i+1):s.connect(j,i+1,n));
  const reset=s.component('SW','SW1','RESET',8700,2500,fp.SW,{LCSC:'C720477'});s.connect(reset,1,'RESET_N');s.connect(reset,2,'GND'); const rr=s.component('R','R60','10k',10500,2500);s.connect(rr,1,'3V3_SYS');s.connect(rr,2,'RESET_N');
  const b0=s.component('SW','SW2','BOOT0',8700,4200,fp.SW,{LCSC:'C720477'});s.connect(b0,1,'BOOT_0');s.connect(b0,2,'GND'); const b1=s.component('SW','SW3','BOOT1',8700,5700,fp.SW,{LCSC:'C720477'});s.connect(b1,1,'BOOT_1');s.connect(b1,2,'GND');
  const uart=s.component('CONN_2X08','J8','UART/SERVICE HEADER',4300,7600,'Connector_PinHeader_2.54mm:PinHeader_2x08_P2.54mm_Vertical'); ['3V3_SYS','GND','UART0_TXD','UART0_RXD','RESET_N','BOOT_0','BOOT_1','GND','CTRL_I2C_SCL','CTRL_I2C_SDA','USBH_EN','USBH_OC_N','AMP_SHDN','HP_DETECT_N','5V_SYS','GND'].forEach((n,i)=>s.connect(uart,i+1,n));
  fs.writeFileSync(path.join(out,'08_Debug.sch'),s.finish());
}

const pinmuxRows = [['Peripheral','Signal','MCU pad','Package pin','Net','Source']];
for (const [pad, net] of Object.entries(mcuNet)) {
  const pin = pinByName.get(pad) ?? pinByName.get(net);
  let peripheral='GPIO';
  if(['PA08','PA09','PA10','PA11','PA12','PA13','PA14'].includes(pad)) peripheral='SDXC0';
  else if(net.startsWith('SD_')) peripheral='FEMC SDRAM';
  if(net.startsWith('QSPI'))peripheral='XPI0 QSPI'; if(net.startsWith('I2S'))peripheral='I2S0'; if(net.includes('I2C_'))peripheral='I2C0'; if(net.startsWith('CTRL_SPI')||net==='ADC_CS_N')peripheral='SPI3'; if(net.startsWith('JTAG'))peripheral='JTAG'; if(net.startsWith('UART'))peripheral='UART0'; if(net.startsWith('USBH_D'))peripheral='USB0'; if(net.startsWith('BOOT_'))peripheral='BOOT_MODE';
  const source = peripheral==='BOOT_MODE' ? 'HPM6360EVK User Guide v1.0 section 2.1.12' : peripheral==='FEMC SDRAM'||peripheral==='SDXC0'||peripheral==='XPI0 QSPI' ? 'HPM SDK hpm6300evk / hpm_iomux.h' : 'Rev.A allocation; verify in HPM Pinmux Tool';
  pinmuxRows.push([peripheral,net,pad,pin ?? '',net,source]);
}
fs.writeFileSync(path.join(out,'pinmux.csv'),pinmuxRows.map(r=>r.map(v=>`"${String(v).replaceAll('"','""')}"`).join(',')).join('\n')+'\n');

const iface = `# Rev.A connector contract\n\n## J1 — USB-C power input\n\nPower-only fixed Sink. Use a 5 V / 3 A Type-C source. D+/D- are intentionally not connected. FUSB303 reports advertised current to firmware.\n\n## J3 — USB-A Host\n\nUSB0 D+/D-, with VBUS supplied through MT9700-N. Intended for USB MIDI devices.\n\n## J4 — PJ-320D-B headphone\n\nStereo TRS output plus detect contact. Firmware shall deassert AMP_SHDN after headphone insertion.\n\n## J5 — speaker\n\nJST-PH 2.0 mm, pin 1 SPK_P, pin 2 SPK_N. Both pins are bridge outputs; neither may be grounded. Load: 4 ohm, up to 3 W.\n\n## J6 — control expansion\n\n| Pin | Signal | Pin | Signal |\n|---:|---|---:|---|\n|1|3V3_CTRL|2|GND|\n|3|5V_CTRL_SW|4|GND|\n|5|CTRL_SPI_SCK|6|GND|\n|7|CTRL_SPI_MOSI|8|CTRL_SPI_MISO|\n|9|ADC_CS_N|10|ENC_LOAD_N|\n|11|ENC_DATA|12|CTRL_PRESENT_N|\n|13|CTRL_I2C_SCL|14|CTRL_I2C_SDA|\n|15|CTRL_IRQ_N|16|GND|\n\nCable length is limited to 20 cm.\n`;
fs.writeFileSync(path.join(out,'INTERFACES.md'),iface);

{
  const s=new Sheet(1,1,'PISCIS-I 8 analog + 8 encoder control daughterboard');
  s.note(700,500,'CONTROL DAUGHTERBOARD: MCP3208 + 2x 74HC165 / 3.3V ratiometric controls');
  const j=s.component('CONN_2X08','J1','CTRL_EXP_2X08',2100,2600); const pinNets=['3V3_CTRL','GND','5V_CTRL_SW','GND','CTRL_SPI_SCK','GND','CTRL_SPI_MOSI','CTRL_SPI_MISO','ADC_CS_N','ENC_LOAD_N','ENC_DATA','CTRL_PRESENT_N','CTRL_I2C_SCL','CTRL_I2C_SDA','CTRL_IRQ_N','GND']; pinNets.forEach((n,i)=>s.connect(j,i+1,n));
  const adc=s.component('MCP3208','U1','MCP3208-CI/SO',5600,2800,fp.MCP3208); ['ADC_CH0','ADC_CH1','ADC_CH2','ADC_CH3','ADC_CH4','ADC_CH5','ADC_CH6','ADC_CH7','GND','ADC_CS_N','CTRL_SPI_MOSI','CTRL_SPI_MISO','CTRL_SPI_SCK','GND','3V3_CTRL','3V3_CTRL'].forEach((n,i)=>s.connect(adc,i+1,n));
  const sr1=s.component('HC165','U2','74HC165D',9300,2500,fp.HC165); const sr2=s.component('HC165','U3','74HC165D',12000,2500,fp.HC165);
  const map165=(u,index,serialIn,serialOut)=>{s.connect(u,1,'ENC_LOAD_N');s.connect(u,2,'CTRL_SPI_SCK');s.connect(u,3,`ENC${index+2}_A`);s.connect(u,4,`ENC${index+2}_B`);s.connect(u,5,`ENC${index+3}_A`);s.connect(u,6,`ENC${index+3}_B`);s.connect(u,7,serialOut);s.connect(u,8,'GND');s.nc(u,9);s.connect(u,10,serialIn);s.connect(u,11,`ENC${index}_A`);s.connect(u,12,`ENC${index}_B`);s.connect(u,13,`ENC${index+1}_A`);s.connect(u,14,`ENC${index+1}_B`);s.connect(u,15,'GND');s.connect(u,16,'3V3_CTRL');};
  map165(sr1,0,'GND','ENC_CHAIN'); map165(sr2,4,'ENC_CHAIN','ENC_DATA');
  for(let i=0;i<8;i++){
    const y=5600+(i%4)*1250, x=i<4?1800:5300;
    const p=s.component('CONN3',`JP${i+1}`,`POT${i} 10k`,x,y,fp.CONN3);s.connect(p,1,'3V3_CTRL');s.connect(p,2,`POT${i}_WIPER`);s.connect(p,3,'GND');
    const r=s.component('R',`R${100+i}`,'1k',x+1200,y);s.connect(r,1,`POT${i}_WIPER`);s.connect(r,2,`ADC_CH${i}`);
    const c=s.component('C',`C${100+i}`,'100nF',x+2300,y);s.connect(c,1,`ADC_CH${i}`);s.connect(c,2,'GND');
  }
  for(let i=0;i<8;i++){
    const y=5600+(i%4)*1250, x=i<4?8700:12200;
    const e=s.component('CONN3',`JE${i+1}`,`ENC${i} A/B`,x,y,fp.CONN3);s.connect(e,1,'GND');s.connect(e,2,`ENC${i}_A`);s.connect(e,3,`ENC${i}_B`);
    const ra=s.component('R',`R${120+i*2}`,'10k pull-up',x+1100,y-250);s.connect(ra,1,'3V3_CTRL');s.connect(ra,2,`ENC${i}_A`); const rb=s.component('R',`R${121+i*2}`,'10k pull-up',x+1100,y+250);s.connect(rb,1,'3V3_CTRL');s.connect(rb,2,`ENC${i}_B`);
  }
  const id=s.component('R','R150','0R board present',3500,4300);s.connect(id,1,'CTRL_PRESENT_N');s.connect(id,2,'GND');
  ['10uF','100nF','100nF','100nF'].forEach((v,i)=>{const c=s.component('C',`C${120+i}`,v,8500+i*1300,10500);s.connect(c,1,'3V3_CTRL');s.connect(c,2,'GND');});
  fs.writeFileSync(path.join(out,'PISCIS-I_Control_8x8.sch'),s.finish());
  fs.copyFileSync(path.join(root,'PISCIS-I_HPM6360_Module.kicad_pro'),path.join(out,'PISCIS-I_Control_8x8.kicad_pro'));
}

const revaReadme = `# PISCIS-I HPM6360 Rev.A engineering project\n\nThis directory is the reconstructed Rev.A design generated by \`tools/generate_reva_hardware.mjs\`. The hierarchical schematics intentionally remain in KiCad legacy \`.sch\` syntax so they can be generated and inspected as text; see \`VALIDATION.md\` for the KiCad 10 conversion status.\n\nDesign basis: official HPM SDK \`boards/hpm6300evk/pinmux.c\` for FEMC and SDXC0, and \`soc/HPM6300/HPM6360/hpm_iomux.h\` for XPI0/I2S/SPI assignments.\n\nThe folder also contains \`PISCIS-I_Control_8x8.sch\`, the matching daughterboard with MCP3208, two 74HC165 devices, eight filtered potentiometer inputs and eight A/B encoder headers. Both PCB files contain every BOM footprint and an unrouted functional placement baseline.\n\nBefore fabrication: verify the PAM8302A order-code pinout, the SM8082 feedback ratio, HPM6360 exposed-pad dimensions, microSD switch polarity, and every imported EasyEDA footprint against the purchased lot.\n`;
fs.writeFileSync(path.join(out,'README.md'),revaReadme);

fs.copyFileSync(path.join(root,'PISCIS-I_HPM6360_Module.kicad_pro'),path.join(out,`${project}.kicad_pro`));
console.log(`Generated legacy Rev.A hierarchy in ${out}`);
