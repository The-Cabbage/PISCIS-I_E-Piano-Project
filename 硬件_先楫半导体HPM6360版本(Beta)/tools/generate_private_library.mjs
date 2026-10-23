import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const libraryRoot = path.join(projectRoot, 'hardware', 'libraries', 'private');
const privateFootprints = path.join(libraryRoot, 'PISCIS_Private.pretty');
const privateModels = path.join(libraryRoot, 'PISCIS_Private.3dshapes');
const privateSymbols = path.join(libraryRoot, 'PISCIS_Private.kicad_sym');
const privateVendorSymbols = path.join(libraryRoot, 'vendor-kicad10');
const sourceFootprints = path.join(projectRoot, 'hardware', 'libraries', 'footprints', 'PISCIS_HPM6360.pretty');
const sourceModels = path.join(projectRoot, 'hardware', 'libraries', '3dmodels', 'PISCIS_HPM6360.3dshapes');
const sourceControlModels = path.join(projectRoot, 'hardware', 'libraries', '3dmodels', 'PISCIS_Control.3dshapes');
const sourceSymbols = path.join(projectRoot, 'hardware', 'libraries', 'symbols', 'vendor-kicad10');
const sourcePowerRoot = path.join(projectRoot, 'hardware', 'libraries', 'power_components');
const sourcePowerFootprints = path.join(sourcePowerRoot, 'Power_Components.pretty');
const sourcePowerModels = path.join(sourcePowerRoot, 'Power_Components.3dshapes');
const sourcePowerSymbols = path.join(sourcePowerRoot, 'Power_Components.kicad_sym');
const stockFootprint = '/Applications/KiCad/KiCad.app/Contents/SharedSupport/footprints/Package_QFP.pretty/LQFP-144_20x20mm_P0.5mm.kicad_mod';

fs.mkdirSync(libraryRoot, { recursive: true });
fs.rmSync(privateFootprints, { recursive: true, force: true });
fs.rmSync(privateModels, { recursive: true, force: true });
fs.rmSync(privateVendorSymbols, { recursive: true, force: true });
fs.cpSync(sourceFootprints, privateFootprints, { recursive: true });
fs.cpSync(sourceModels, privateModels, { recursive: true });
fs.cpSync(sourceControlModels, privateModels, { recursive: true });
fs.cpSync(sourceSymbols, privateVendorSymbols, { recursive: true });
fs.cpSync(sourcePowerFootprints, privateFootprints, { recursive: true });
fs.cpSync(sourcePowerModels, privateModels, { recursive: true });

for (const filename of fs.readdirSync(privateFootprints).filter((name) => name.endsWith('.kicad_mod'))) {
  const filenamePath = path.join(privateFootprints, filename);
  const text = fs.readFileSync(filenamePath, 'utf8')
    .replaceAll('hardware/libraries/3dmodels/PISCIS_HPM6360.3dshapes', 'hardware/libraries/private/PISCIS_Private.3dshapes')
    .replaceAll('hardware/libraries/power_components/Power_Components.3dshapes', 'hardware/libraries/private/PISCIS_Private.3dshapes');
  fs.writeFileSync(filenamePath, text);
}

const pinNames = [
  'PA22','PA21','PA20','PA19','VDD_SOC','PA18','PA17','PA16','PA15','PA14','PA13','VIO_01','PA12','PA11','PA10','PA09','PA08','VDD_SOC','PA07','PA06','PA05','PA04','VIO_00','PA03','PA02','PA01','PA00','VDD_SOC','DCDC_SNS','DCDC_GND','DCDC_LP','DCDC_IN','PY07','PY06','PY05','PY04',
  'PY03','PY02','VDD_OTPCAP','PY01','VDD_PMCCAP','PY00','VPMC','RTC_XTALI','RTC_XTALO','VBAT','PZ07','PZ06','PZ05','PZ04','PZ03','PZ02','PZ01','PZ00','VDD_SOC','PC27','PC26','PC25','PC24','PC23','VIO_02','PC22','PC21','PC20','PC19','PC18','PC17','VDD_SOC','PC16','PC15','PC14','PC13',
  'PC12','PC11','PC10','PC09','VDD_SOC','PC08','PC07','PC06','PC05','PC04','PC03','VREFL','VREFH','VIO_02','VANA','XTALI','XTALO','VDD_SOC','USBVBUS','USB_DP','USB_DM','VIO_01','PC02','PC01','PC00','PB31','PB30','PB29','PB28','PB27','PB26','VDD_SOC','PB25','PB24','PB23','PB22',
  'PB21','PB20','PB19','PB18','VDD_SOC','PB17','PB16','PB15','PB14','PB13','PB12','VIO_01','PB11','PB10','PB09','PB08','PB07','VDD_SOC','PB06','PB05','PB04','PB03','PB02','PB01','VIO_01','PB00','PA31','PA30','PA29','PA28','PA27','VDD_SOC','PA26','PA25','PA24','PA23',
  'VSS'
];

if (pinNames.length !== 145) throw new Error(`Expected 145 pins including exposed pad, got ${pinNames.length}`);

const powerPins = new Set(['VDD_SOC','VIO_00','VIO_01','VIO_02','VDD_OTPCAP','VDD_PMCCAP','VPMC','VBAT','VREFH','VREFL','VANA','USBVBUS','VSS','DCDC_IN']);
const inputPins = new Set(['XTALI','RTC_XTALI','DCDC_SNS']);
const outputPins = new Set(['XTALO','RTC_XTALO','DCDC_LP']);
const electricalType = (name) => powerPins.has(name) ? 'power_in' : inputPins.has(name) ? 'input' : outputPins.has(name) ? 'output' : 'bidirectional';
const pin = (number, name, x, y, rotation) => `\t\t\t(pin ${electricalType(name)} line\n\t\t\t\t(at ${x.toFixed(2)} ${y.toFixed(2)} ${rotation})\n\t\t\t\t(length 8)\n\t\t\t\t(name "${name}"\n\t\t\t\t\t(effects (font (size 1.00 1.00)))\n\t\t\t\t)\n\t\t\t\t(number "${number}"\n\t\t\t\t\t(effects (font (size 1.00 1.00)))\n\t\t\t\t)\n\t\t\t)\n`;

let pinText = '';
for (let i = 0; i < 36; i += 1) pinText += pin(i + 1, pinNames[i], -60, 44.45 - i * 2.54, 0);
for (let i = 36; i < 72; i += 1) pinText += pin(i + 1, pinNames[i], -44.45 + (i - 36) * 2.54, -60, 90);
for (let i = 72; i < 108; i += 1) pinText += pin(i + 1, pinNames[i], 60, -44.45 + (i - 72) * 2.54, 180);
for (let i = 108; i < 144; i += 1) pinText += pin(i + 1, pinNames[i], 44.45 - (i - 108) * 2.54, 60, 270);
pinText += pin(145, pinNames[144], 0, -60, 90);

const hiddenProperty = (name, value) => `\t\t(property "${name}" "${value}" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))`;
const symbol = `(kicad_symbol_lib\n\t(version 20251024)\n\t(generator "PISCIS-I official-data-derived library generator")\n\t(symbol "HPM6360IPA2"\n\t\t(exclude_from_sim no)\n\t\t(in_bom yes)\n\t\t(on_board yes)\n\t\t(property "Reference" "U" (at -50 64 0) (effects (font (size 1.27 1.27)) (justify right)))\n\t\t(property "Value" "HPM6360IPA2" (at -50 61.5 0) (effects (font (size 1.27 1.27)) (justify right)))\n${hiddenProperty('Footprint', '')}\n${hiddenProperty('Datasheet', 'https://doc.hpmicro.com/Public/Uploads/uploadfile/files/20250205/HPM6300DSV27.pdf')}\n${hiddenProperty('Description', 'HPMicro HPM6360IPA2, 20x20 mm eLQFP144L P0.5; pinout derived from HPM6300 Data Manual Rev2.7 Figure 2. Select the EP footprint only after die-size/EP confirmation.')}\n${hiddenProperty('Manufacturer', 'HPMicro')}\n${hiddenProperty('MPN', 'HPM6360IPA2')}\n${hiddenProperty('Source_Status', 'Official-data-derived; HPMicro public CAD/CAE portal reported no downloadable CAD/CAE resource on 2026-09-17.')}\n\t\t(symbol "HPM6360IPA2_0_1"\n\t\t\t(rectangle (start -52.00 52.00) (end 52.00 -52.00) (stroke (width 0.50) (type default)) (fill (type background)))\n\t\t)\n\t\t(symbol "HPM6360IPA2_1_1"\n${pinText}\t\t)\n\t)\n)\n`;
const powerSymbolLibrary = fs.readFileSync(sourcePowerSymbols, 'utf8');
const powerSymbols = powerSymbolLibrary
  .slice(powerSymbolLibrary.indexOf('\t(symbol "SM8082AAAC"'), powerSymbolLibrary.lastIndexOf('\n)'))
  .replaceAll('PISCIS_Power_Components:', 'PISCIS_Private:');
const w25q128jvpSymbol = `\t(symbol "W25Q128JVP"
\t\t(exclude_from_sim no)
\t\t(in_bom yes)
\t\t(on_board yes)
\t\t(in_pos_files yes)
\t\t(duplicate_pin_numbers_are_jumpers no)
\t\t(property "Reference" "U" (at -6.35 11.43 0) (effects (font (size 1.27 1.27))))
\t\t(property "Value" "W25Q128JVP" (at 7.62 11.43 0) (effects (font (size 1.27 1.27))))
\t\t(property "Footprint" "PISCIS_Private:WSON-8-1EP_6x5mm_P1.27mm_EP3.4x4.3mm" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Datasheet" "https://www.winbond.com/resource-files/w25q128jv_dtr%20revc%2003272018%20plus.pdf" (at 0 -2.54 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Description" "Winbond W25Q128JV, 128-Mbit serial NOR flash, SPI/Dual/Quad I/O, 2.7-3.6 V, WSON-8 6x5 mm (P package)" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Manufacturer" "Winbond Electronics" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "MPN" "W25Q128JVP" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_keywords" "flash memory SPI Dual Quad QSPI NOR Winbond" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_fp_filters" "WSON*1EP*6x5mm*P1.27mm*EP3.4x4.3mm*" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(symbol "W25Q128JVP_0_1"
\t\t\t(rectangle (start -7.62 10.16) (end 10.16 -10.16) (stroke (width 0.254) (type default)) (fill (type background)))
\t\t)
\t\t(symbol "W25Q128JVP_1_1"
\t\t\t(pin input line (at -10.16 7.62 0) (length 2.54) (name "~{CS}" (effects (font (size 1.27 1.27)))) (number "1" (effects (font (size 1.27 1.27)))))
\t\t\t(pin bidirectional line (at -10.16 0 0) (length 2.54) (name "DO/IO_{1}" (effects (font (size 1.27 1.27)))) (number "2" (effects (font (size 1.27 1.27)))))
\t\t\t(pin bidirectional line (at -10.16 -2.54 0) (length 2.54) (name "~{WP}/IO_{2}" (effects (font (size 1.27 1.27)))) (number "3" (effects (font (size 1.27 1.27)))))
\t\t\t(pin power_in line (at 0 -12.7 90) (length 2.54) (name "GND" (effects (font (size 1.27 1.27)))) (number "4" (effects (font (size 1.27 1.27)))))
\t\t\t(pin bidirectional line (at -10.16 2.54 0) (length 2.54) (name "DI/IO_{0}" (effects (font (size 1.27 1.27)))) (number "5" (effects (font (size 1.27 1.27)))))
\t\t\t(pin input line (at -10.16 5.08 0) (length 2.54) (name "CLK" (effects (font (size 1.27 1.27)))) (number "6" (effects (font (size 1.27 1.27)))))
\t\t\t(pin bidirectional line (at -10.16 -5.08 0) (length 2.54) (name "~{HOLD}/~{RESET}/IO_{3}" (effects (font (size 1.27 1.27)))) (number "7" (effects (font (size 1.27 1.27)))))
\t\t\t(pin power_in line (at 0 12.7 270) (length 2.54) (name "VCC" (effects (font (size 1.27 1.27)))) (number "8" (effects (font (size 1.27 1.27)))))
\t\t\t(pin power_in line (at 2.54 -12.7 90) (length 2.54) (name "EP/GND" (effects (font (size 1.27 1.27)))) (number "9" (effects (font (size 1.27 1.27)))))
\t\t)
\t)
`;
const syt13s03smdSymbol = `\t(symbol "SYT13S03SMD"
\t\t(exclude_from_sim no)
\t\t(in_bom yes)
\t\t(on_board yes)
\t\t(in_pos_files yes)
\t\t(duplicate_pin_numbers_are_jumpers no)
\t\t(property "Reference" "D" (at -7.62 6.35 0) (effects (font (size 1.27 1.27))))
\t\t(property "Value" "SYT13S03SMD" (at 2.54 6.35 0) (effects (font (size 1.27 1.27))))
\t\t(property "Footprint" "PISCIS_Private:Silergy_DFN-3_1.0x0.6mm_SYT13S03SMD" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Datasheet" "https://www.silergy.com/download/downloadFile?ftype=note&id=5304&type=product" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Description" "Silergy dual-channel ultra-low-capacitance unidirectional TVS array, 3.3 V VRWM, 0.42 pF typical, DFN1.0x0.6-3; current part number SY205253SMD" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Manufacturer" "Silergy" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "MPN" "SYT13S03SMD (alias of SY205253SMD)" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_keywords" "TVS ESD USB high-speed dual low-capacitance protection array" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_fp_filters" "Silergy*DFN-3*1.0x0.6mm*SYT13S03SMD*" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(symbol "SYT13S03SMD_0_1"
\t\t\t(rectangle (start -7.62 5.08) (end 7.62 -5.08) (stroke (width 0.254) (type default)) (fill (type background)))
\t\t)
\t\t(symbol "SYT13S03SMD_1_1"
\t\t\t(pin passive line (at -10.16 2.54 0) (length 2.54) (name "I/O_1" (effects (font (size 1.27 1.27)))) (number "1" (effects (font (size 1.27 1.27)))))
\t\t\t(pin passive line (at -10.16 -2.54 0) (length 2.54) (name "I/O_2" (effects (font (size 1.27 1.27)))) (number "2" (effects (font (size 1.27 1.27)))))
\t\t\t(pin power_in line (at 0 -7.62 90) (length 2.54) (name "GND" (effects (font (size 1.27 1.27)))) (number "3" (effects (font (size 1.27 1.27)))))
\t\t)
\t)
`;
const pj320dBSmtSymbol = `\t(symbol "PJ-320D-B-SMT"
\t\t(exclude_from_sim no)
\t\t(in_bom yes)
\t\t(on_board yes)
\t\t(in_pos_files yes)
\t\t(duplicate_pin_numbers_are_jumpers no)
\t\t(property "Reference" "J" (at -10.16 8.89 0) (effects (font (size 1.27 1.27))))
\t\t(property "Value" "PJ-320D-B-SMT" (at 2.54 8.89 0) (effects (font (size 1.27 1.27))))
\t\t(property "Footprint" "PISCIS_Private:AUDIO-SMD_XKB_PJ-320D-B" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Datasheet" "https://atta.szlcsc.com/upload/public/pdf/source/20211009/E6E54FC694C9735EA5149D377CBF5C63.pdf" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Description" "XKB PJ-320D-B-SMT, horizontal surface-mount 3.5 mm 4-pole TRRS audio jack; four independent contacts, no insertion-detect switch" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Manufacturer" "XKB Connection" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "MPN" "PJ-320D-B-SMT" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "LCSC Part" "C2884940" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Pinout" "1=TIP, 4=RING1, 3=RING2, 2=SLEEVE" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_keywords" "audio jack 3.5mm TRRS 4-pole headphone headset SMT horizontal XKB" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_fp_filters" "AUDIO-SMD_XKB_PJ-320D-B" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(symbol "PJ-320D-B-SMT_0_1"
\t\t\t(rectangle (start -7.62 6.35) (end 7.62 -6.35) (stroke (width 0.254) (type default)) (fill (type background)))
\t\t\t(polyline (pts (xy -7.62 3.81) (xy 2.54 3.81) (xy 3.81 2.54)) (stroke (width 0.254) (type default)) (fill (type none)))
\t\t\t(polyline (pts (xy -7.62 1.27) (xy 1.27 1.27) (xy 2.54 0)) (stroke (width 0.254) (type default)) (fill (type none)))
\t\t\t(polyline (pts (xy -7.62 -1.27) (xy 0 -1.27) (xy 1.27 -2.54)) (stroke (width 0.254) (type default)) (fill (type none)))
\t\t\t(polyline (pts (xy -7.62 -3.81) (xy 3.81 -3.81)) (stroke (width 0.254) (type default)) (fill (type none)))
\t\t)
\t\t(symbol "PJ-320D-B-SMT_1_1"
\t\t\t(pin passive line (at -10.16 3.81 0) (length 2.54) (name "" (effects (font (size 1.27 1.27)))) (number "1" (effects (font (size 1.27 1.27)))))
\t\t\t(pin passive line (at -10.16 1.27 0) (length 2.54) (name "" (effects (font (size 1.27 1.27)))) (number "4" (effects (font (size 1.27 1.27)))))
\t\t\t(pin passive line (at -10.16 -1.27 0) (length 2.54) (name "" (effects (font (size 1.27 1.27)))) (number "3" (effects (font (size 1.27 1.27)))))
\t\t\t(pin passive line (at -10.16 -3.81 0) (length 2.54) (name "" (effects (font (size 1.27 1.27)))) (number "2" (effects (font (size 1.27 1.27)))))
\t\t)
\t)
`;
const hxPj3203pSmtSymbol = `\t(symbol "HX_PJ-320-3P_SMT"
\t\t(exclude_from_sim no)
\t\t(in_bom yes)
\t\t(on_board yes)
\t\t(in_pos_files yes)
\t\t(duplicate_pin_numbers_are_jumpers no)
\t\t(property "Reference" "J" (at -10.16 7.62 0) (effects (font (size 1.27 1.27))))
\t\t(property "Value" "HX PJ-320-3P SMT" (at 2.54 7.62 0) (effects (font (size 1.27 1.27))))
\t\t(property "Footprint" "PISCIS_Private:AUDIO-SMD_HX-PJ-320-3P-SMT" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Datasheet" "https://atta.szlcsc.com/upload/public/pdf/source/20231103/647716C6DC531E105FAE0C0F1B835F33.pdf" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Description" "Hanxia HX PJ-320-3P SMT, horizontal surface-mount 3.5 mm 3-pole TRS audio jack; A=SLEEVE, B=RING, C=TIP; no insertion-detect switch. Geometry converted from the 嘉立创EDA/EasyEDA official library and checked against the Hanxia drawing." (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Manufacturer" "hanxia (韩下)" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "MPN" "HX PJ-320-3P SMT" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "LCSC Part" "C19189086" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Pinout" "A=SLEEVE, B=RING, C=TIP" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "Source" "嘉立创EDA/EasyEDA官方库: https://lceda.cn/ ; https://easyeda.com" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_keywords" "audio jack 3.5mm TRS 3-pole headphone mono stereo SMT horizontal Hanxia 韩下" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(property "ki_fp_filters" "AUDIO-SMD_HX-PJ-320-3P-SMT" (at 0 0 0) (hide yes) (effects (font (size 1.27 1.27))))
\t\t(symbol "HX_PJ-320-3P_SMT_0_1"
\t\t\t(rectangle (start -7.62 5.08) (end 7.62 -5.08) (stroke (width 0.254) (type default)) (fill (type background)))
\t\t\t(polyline (pts (xy -7.62 2.54) (xy 2.54 2.54) (xy 3.81 1.27)) (stroke (width 0.254) (type default)) (fill (type none)))
\t\t\t(polyline (pts (xy -7.62 0) (xy 1.27 0) (xy 2.54 -1.27)) (stroke (width 0.254) (type default)) (fill (type none)))
\t\t\t(polyline (pts (xy -7.62 -2.54) (xy 3.81 -2.54)) (stroke (width 0.254) (type default)) (fill (type none)))
\t\t)
\t\t(symbol "HX_PJ-320-3P_SMT_1_1"
\t\t\t(pin passive line (at -10.16 2.54 0) (length 2.54) (name "TIP" (effects (font (size 1.27 1.27)))) (number "C" (effects (font (size 1.27 1.27)))))
\t\t\t(pin passive line (at -10.16 0 0) (length 2.54) (name "RING" (effects (font (size 1.27 1.27)))) (number "B" (effects (font (size 1.27 1.27)))))
\t\t\t(pin passive line (at -10.16 -2.54 0) (length 2.54) (name "SLEEVE" (effects (font (size 1.27 1.27)))) (number "A" (effects (font (size 1.27 1.27)))))
\t\t)
\t)
`;
const mergedSymbol = symbol.replace(/\n\)\n$/, `\n${powerSymbols}\n${w25q128jvpSymbol}${syt13s03smdSymbol}${pj320dBSmtSymbol}${hxPj3203pSmtSymbol})\n`);
fs.writeFileSync(privateSymbols, mergedSymbol);

const syt13s03smdFootprint = `(footprint "Silergy_DFN-3_1.0x0.6mm_SYT13S03SMD"
  (version 20260206)
  (generator "PISCIS-I official-data-derived library generator")
  (layer "F.Cu")
  (descr "Silergy SYT13S03SMD / SY205253SMD, DFN1.0x0.6-3; land pattern from manufacturer datasheet Rev.1.0")
  (tags "Silergy SYT13S03SMD SY205253SMD DFN-3 ESD TVS")
  (property "Reference" "D**" (at 0 -1.25 0) (layer "F.SilkS") (effects (font (size 0.7 0.7) (thickness 0.1))))
  (property "Value" "Silergy_DFN-3_1.0x0.6mm_SYT13S03SMD" (at 0 1.25 0) (layer "F.Fab") (effects (font (size 0.7 0.7) (thickness 0.1))))
  (attr smd)
  (fp_line (start -0.50 -0.30) (end 0.50 -0.30) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start 0.50 -0.30) (end 0.50 0.30) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start 0.50 0.30) (end -0.35 0.30) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start -0.35 0.30) (end -0.50 0.15) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start -0.50 0.15) (end -0.50 -0.30) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_circle (center -0.66 0.42) (end -0.61 0.42) (stroke (width 0.10) (type default)) (fill none) (layer "F.SilkS"))
  (fp_rect (start -0.70 -0.50) (end 0.70 0.50) (stroke (width 0.05) (type default)) (fill none) (layer "F.CrtYd"))
  (fp_text user "\${REFERENCE}" (at 0 0 0) (layer "F.Fab") (effects (font (size 0.25 0.25) (thickness 0.04))))
  (pad "1" smd roundrect (at -0.3875 0.175) (size 0.425 0.20) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.25))
  (pad "2" smd roundrect (at -0.3875 -0.175) (size 0.425 0.20) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.25))
  (pad "3" smd roundrect (at 0.3875 0) (size 0.425 0.55) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.117647))
  (embedded_fonts no)
)
`;
fs.writeFileSync(path.join(privateFootprints, 'Silergy_DFN-3_1.0x0.6mm_SYT13S03SMD.kicad_mod'), syt13s03smdFootprint);

const pj320dBSmtFootprint = `(footprint "AUDIO-SMD_XKB_PJ-320D-B"
  (version 20260206)
  (generator "PISCIS-I XKB-drawing-derived library generator")
  (layer "F.Cu")
  (descr "XKB PJ-320D-B-SMT horizontal 3.5 mm 4-pole TRRS jack; 14x6x5 mm body; land pattern and pin positions derived from manufacturer drawing PJ-320D-X Rev.A0 and EasyEDA/LCSC C2884940 source record")
  (tags "XKB PJ-320D-B-SMT C2884940 3.5mm audio jack TRRS 4-pole SMT horizontal")
  (property "Reference" "J**" (at -1 -5.4 0) (layer "F.SilkS") (effects (font (size 1 1) (thickness 0.15))))
  (property "Value" "AUDIO-SMD_XKB_PJ-320D-B" (at -1 5.4 0) (layer "F.Fab") (effects (font (size 1 1) (thickness 0.15))))
  (attr smd)
  (fp_line (start -8 -2.5) (end -6 -2.5) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -8 2.5) (end -6 2.5) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -8 -2.5) (end -8 2.5) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -6 -3) (end -4.1 -3) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -1.9 -3) (end -1.1 -3) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start 1.1 -3) (end 2.9 -3) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start 5.1 -3) (end 6 -3) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -6 3) (end 4.9 3) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start 6 -3) (end 6 2.4) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -6 -3) (end -6 3) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_circle (center 6.4 3.5) (end 6.6 3.5) (stroke (width 0.12) (type default)) (fill none) (layer "F.SilkS"))
  (fp_circle (center -8 0) (end -7.75 0) (stroke (width 0.12) (type default)) (fill none) (layer "F.Fab"))
  (fp_line (start -8 -2.5) (end -6 -2.5) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start -8 2.5) (end -6 2.5) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start -8 -2.5) (end -8 2.5) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_rect (start -6 -3) (end 6 3) (stroke (width 0.10) (type default)) (fill none) (layer "F.Fab"))
  (fp_rect (start -8.25 -4.65) (end 6.75 4.65) (stroke (width 0.05) (type default)) (fill none) (layer "F.CrtYd"))
  (fp_text user "JACK FACE / PCB EDGE" (at -8.6 0 90) (layer "Dwgs.User") (effects (font (size 0.7 0.7) (thickness 0.1))))
  (fp_text user "\${REFERENCE}" (at 0 0 0) (layer "F.Fab") (effects (font (size 0.8 0.8) (thickness 0.12))))
  (pad "1" smd roundrect (at 5.5 3.5) (size 1.2 2.0) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.20))
  (pad "2" smd roundrect (at -3 -3.5) (size 1.2 2.0) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.20))
  (pad "3" smd roundrect (at 0 -3.5) (size 1.2 2.0) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.20))
  (pad "4" smd roundrect (at 4 -3.5) (size 1.2 2.0) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.20))
  (pad "" np_thru_hole circle (at -4.5 0) (size 1.3 1.3) (drill 1.3) (layers "*.Cu" "*.Mask"))
  (pad "" np_thru_hole circle (at 2.5 0) (size 1.3 1.3) (drill 1.3) (layers "*.Cu" "*.Mask"))
  (embedded_fonts no)
)
`;
fs.writeFileSync(path.join(privateFootprints, 'AUDIO-SMD_XKB_PJ-320D-B.kicad_mod'), pj320dBSmtFootprint);

const hxPj3203pSmtFootprint = `(footprint "AUDIO-SMD_HX-PJ-320-3P-SMT"
  (version 20260206)
  (generator "PISCIS-I EasyEDA-official-source converter")
  (layer "F.Cu")
  (descr "Hanxia HX PJ-320-3P SMT / LCSC C19189086, horizontal 3.5 mm 3-pole TRS jack; 14.10x6.00x5.00 mm; pads and locating holes converted from 嘉立创EDA/EasyEDA官方库 and checked against the Hanxia drawing")
  (tags "Hanxia 韩下 HX PJ-320-3P SMT C19189086 3.5mm audio jack TRS 3-pole SMT horizontal")
  (property "Reference" "J**" (at 0 -5.55 0) (layer "F.SilkS") (effects (font (size 1 1) (thickness 0.15))))
  (property "Value" "AUDIO-SMD_HX-PJ-320-3P-SMT" (at 0 5.55 0) (layer "F.Fab") (effects (font (size 1 1) (thickness 0.15))))
  (attr smd)
  (fp_line (start -7.05 -2.50) (end -4.05 -2.50) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -7.05 2.50) (end -4.05 2.50) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -7.05 -2.50) (end -7.05 2.50) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -4.05 -3.00) (end -1.05 -3.00) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start 0.85 -3.00) (end 7.05 -3.00) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -4.05 3.00) (end -2.80 3.00) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -0.90 3.00) (end 4.05 3.00) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start 5.95 3.00) (end 7.05 3.00) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -4.05 -3.00) (end -4.05 -2.50) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start -4.05 2.50) (end -4.05 3.00) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_line (start 7.05 -3.00) (end 7.05 3.00) (stroke (width 0.15) (type default)) (layer "F.SilkS"))
  (fp_circle (center -2.95 4.65) (end -2.75 4.65) (stroke (width 0.12) (type default)) (fill none) (layer "F.SilkS"))
  (fp_line (start -7.05 -2.50) (end -4.05 -2.50) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start -7.05 2.50) (end -4.05 2.50) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_line (start -7.05 -2.50) (end -7.05 2.50) (stroke (width 0.10) (type default)) (layer "F.Fab"))
  (fp_rect (start -4.05 -3.00) (end 7.05 3.00) (stroke (width 0.10) (type default)) (fill none) (layer "F.Fab"))
  (fp_rect (start -7.30 -5.15) (end 7.30 5.15) (stroke (width 0.05) (type default)) (fill none) (layer "F.CrtYd"))
  (fp_text user "JACK FACE / PCB EDGE" (at -7.55 0 90) (layer "Dwgs.User") (effects (font (size 0.7 0.7) (thickness 0.1))))
  (fp_text user "\${REFERENCE}" (at 1.5 0 0) (layer "F.Fab") (effects (font (size 0.8 0.8) (thickness 0.12))))
  (pad "A" smd roundrect (at -1.85 3.40) (size 1.50 3.00) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.15))
  (pad "B" smd roundrect (at -0.10 -3.40) (size 1.50 3.00) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.15))
  (pad "C" smd roundrect (at 5.00 3.40) (size 1.50 3.00) (layers "F.Cu" "F.Paste" "F.Mask") (roundrect_rratio 0.15))
  (pad "" np_thru_hole circle (at -2.35 0) (size 0.90 0.90) (drill 0.90) (layers "*.Cu" "*.Mask"))
  (pad "" np_thru_hole circle (at 3.65 0) (size 0.90 0.90) (drill 0.90) (layers "*.Cu" "*.Mask"))
  (embedded_fonts no)
)
`;
fs.writeFileSync(path.join(privateFootprints, 'AUDIO-SMD_HX-PJ-320-3P-SMT.kicad_mod'), hxPj3203pSmtFootprint);

const pasteWindows = (ep) => {
  const count = ep === 5 ? 3 : 5;
  const pitch = ep === 5 ? 1.60 : 1.80;
  const size = ep === 5 ? 1.40 : 1.45;
  let result = '';
  for (let row = 0; row < count; row += 1) {
    for (let column = 0; column < count; column += 1) {
      const x = (column - (count - 1) / 2) * pitch;
      const y = (row - (count - 1) / 2) * pitch;
      const half = size / 2;
      result += `\t(fp_rect (start ${(x - half).toFixed(3)} ${(y - half).toFixed(3)}) (end ${(x + half).toFixed(3)} ${(y + half).toFixed(3)}) (stroke (width 0) (type default)) (fill yes) (layer "F.Paste"))\n`;
    }
  }
  return result;
};

const stock = fs.readFileSync(stockFootprint, 'utf8');
for (const ep of [5, 9.74]) {
  const name = `HPM6360IPA2_eLQFP144L_20x20mm_P0.5mm_EP${ep}x${ep}mm`;
  let footprint = stock
    .replace('LQFP-144_20x20mm_P0.5mm', name)
    .replace('LQFP, 144 Pin', `HPMicro HPM6360IPA2 eLQFP144L, 144 pin + exposed VSS pad ${ep}x${ep} mm; package geometry derived from HPM6300 Data Manual Rev2.7 Figure 26`)
    .replace('${KICAD10_3DMODEL_DIR}/Package_QFP.3dshapes/LQFP-144_20x20mm_P0.5mm.step', '${KIPRJMOD}/hardware/libraries/private/PISCIS_Private.3dshapes/LQFP-144_20x20mm_P0.5mm.step');
  const epPad = `\t(fp_text user "EP/VSS ${ep}x${ep} mm - verify against delivered lot" (at 0 0) (layer "F.Fab") (effects (font (size 0.8 0.8) (thickness 0.12))))\n\t(pad "145" smd rect (at 0 0) (size ${ep} ${ep}) (layers "F.Cu" "F.Mask"))\n${pasteWindows(ep)}`;
  footprint = footprint.replace('\t(embedded_fonts no)', `${epPad}\t(embedded_fonts no)`);
  fs.writeFileSync(path.join(privateFootprints, `${name}.kicad_mod`), footprint);
}

console.log(`Created private library: ${privateSymbols}`);
console.log(`Footprints: ${fs.readdirSync(privateFootprints).filter((name) => name.endsWith('.kicad_mod')).length}`);
console.log(`STEP models: ${fs.readdirSync(privateModels).filter((name) => name.endsWith('.step')).length}`);
