import fs from "node:fs/promises";
import { fileURLToPath } from "node:url";
import { Workbook, SpreadsheetFile } from "@oai/artifact-tool";

const outputDir = fileURLToPath(new URL("./", import.meta.url));
const dataPath = fileURLToPath(new URL("../../sales_market_data.json", import.meta.url));
const rows = JSON.parse(await fs.readFile(dataPath, "utf8"));

const wb = Workbook.create();
const summary = wb.worksheets.add("Sales Summary");
const data = wb.worksheets.add("Source Data");
const method = wb.worksheets.add("Methodology");
wb.comments.setSelf({ displayName: "User" });

const navy = "#14213D";
const blue = "#2F6BFF";
const paleBlue = "#EAF1FF";
const paleYellow = "#FFF4CC";
const paleGreen = "#E7F6EC";
const paleRed = "#FDECEC";
const gray = "#667085";
const lightBorder = "#D0D5DD";

// Source Data sheet
const headers = [
  "Product", "Category", "Status", "Price USD", "Price Type",
  "Public Monthly Units", "Monthly Sales Note", "Sales Proxy", "Proxy Value",
  "Proxy Type", "English Coverage Count", "Price Source URL", "Sales Source URL",
  "Coverage Source", "Confidence", "Scenario Monthly Units (Input)",
  "Scenario Monthly Revenue USD"
];
const matrix = rows.map(r => [
  r.product, r.category, r.status, r.price_usd, r.price_type,
  r.public_monthly_units, r.monthly_sales_note, r.sales_proxy, r.proxy_value,
  r.proxy_type, r.english_coverage_count, r.price_source, r.sales_source,
  r.coverage_source, r.confidence, null, null
]);
data.getRange("A1:Q1").values = [headers];
data.getRange(`A2:Q${rows.length + 1}`).values = matrix;
data.getRange("Q2").formulas = [["=IF(AND(D2>0,P2>0),D2*P2,\"\")"]];
data.getRange(`Q2:Q${rows.length + 1}`).fillDown();
data.tables.add(`A1:Q${rows.length + 1}`, true, "SalesSourceTable");
data.freezePanes.freezeRows(1);
data.freezePanes.freezeColumns(1);
data.showGridLines = false;
data.getRange("A1:Q1").format = {
  fill: navy, font: { color: "#FFFFFF", bold: true },
  wrapText: true, verticalAlignment: "center",
  borders: { preset: "outside", style: "medium", color: navy }
};
data.getRange(`A2:Q${rows.length + 1}`).format = {
  verticalAlignment: "top",
  borders: { insideHorizontal: { style: "thin", color: lightBorder } }
};
data.getRange(`D2:D${rows.length + 1}`).format.numberFormat = "$#,##0.00";
data.getRange(`F2:F${rows.length + 1}`).format.numberFormat = "#,##0";
data.getRange(`I2:I${rows.length + 1}`).format.numberFormat = "#,##0";
data.getRange(`K2:K${rows.length + 1}`).format.numberFormat = "#,##0";
data.getRange(`P2:P${rows.length + 1}`).format = { fill: paleYellow, numberFormat: "#,##0" };
data.getRange(`Q2:Q${rows.length + 1}`).format = { fill: paleGreen, numberFormat: "$#,##0" };
data.getRange(`O2:O${rows.length + 1}`).conditionalFormats.add("containsText", { text: "High", format: { fill: paleGreen, font: { color: "#166534" } } });
data.getRange(`O2:O${rows.length + 1}`).conditionalFormats.add("containsText", { text: "Low", format: { fill: paleRed, font: { color: "#991B1B" } } });
data.getRange("A:Q").format.wrapText = true;
const widths = [28, 27, 30, 14, 28, 18, 38, 42, 14, 24, 18, 44, 44, 30, 12, 23, 25];
for (let i = 0; i < widths.length; i++) data.getRangeByIndexes(0, i, rows.length + 1, 1).format.columnWidth = widths[i];
data.getRange("1:1").format.rowHeight = 44;
data.getRange(`2:${rows.length + 1}`).format.rowHeight = 48;

// Summary sheet
summary.showGridLines = false;
summary.getRange("A1:H2").merge();
summary.getRange("A1").values = [["Portable Music Hardware — Sales & Coverage Snapshot"]];
summary.getRange("A1:H2").format = { fill: navy, font: { color: "#FFFFFF", bold: true, size: 20 }, verticalAlignment: "center" };
summary.getRange("A3:H3").merge();
summary.getRange("A3").values = [["Public US pricing and sales proxies captured 2026-08-23. Monthly unit sales are not disclosed for the reviewed SKUs."]];
summary.getRange("A3:H3").format = { fill: paleBlue, font: { color: navy, italic: true }, wrapText: true };

summary.getRange("A5:B5").merge(); summary.getRange("A5").values = [["Products tracked"]];
summary.getRange("C5:D5").merge(); summary.getRange("C5").values = [["Median listed price"]];
summary.getRange("E5:F5").merge(); summary.getRange("E5").values = [["High-confidence rows"]];
summary.getRange("G5:H5").merge(); summary.getRange("G5").values = [["Public monthly-unit values"]];
summary.getRange("A6:B7").merge(); summary.getRange("A6").formulas = [["=COUNTA('Source Data'!A2:A13)"]];
summary.getRange("C6:D7").merge(); summary.getRange("C6").formulas = [["=MEDIAN('Source Data'!D2:D13)"]];
summary.getRange("E6:F7").merge(); summary.getRange("E6").formulas = [["=COUNTIF('Source Data'!O2:O13,\"High\")"]];
summary.getRange("G6:H7").merge(); summary.getRange("G6").formulas = [["=COUNT('Source Data'!F2:F13)"]];
summary.getRange("A5:H5").format = { fill: blue, font: { color: "#FFFFFF", bold: true }, horizontalAlignment: "center" };
summary.getRange("A6:H7").format = { fill: "#F8FAFC", font: { color: navy, bold: true, size: 18 }, horizontalAlignment: "center", verticalAlignment: "center", borders: { preset: "outside", style: "thin", color: lightBorder } };
summary.getRange("C6:D7").format.numberFormat = "$#,##0";

summary.getRange("A9:H9").merge(); summary.getRange("A9").values = [["Pricing and market signals"]];
summary.getRange("A9:H9").format = { fill: navy, font: { color: "#FFFFFF", bold: true, size: 13 } };
summary.getRange("A10:H10").values = [["Product", "Price USD", "Coverage", "Proxy Value", "Proxy Type", "Confidence", "Scenario Units", "Scenario Revenue"]];
summary.getRange("A10:H10").format = { fill: blue, font: { color: "#FFFFFF", bold: true }, wrapText: true };
for (let i = 0; i < rows.length; i++) {
  const sr = i + 11;
  const dr = i + 2;
  summary.getRange(`A${sr}:H${sr}`).formulas = [[
    `='Source Data'!A${dr}`, `=IF('Source Data'!D${dr}>0,'Source Data'!D${dr},\"\")`, `='Source Data'!K${dr}`,
    `=IF('Source Data'!I${dr}>0,'Source Data'!I${dr},\"\")`, `='Source Data'!J${dr}`, `='Source Data'!O${dr}`,
    `=IF('Source Data'!P${dr}>0,'Source Data'!P${dr},\"\")`, `=IF('Source Data'!Q${dr}>0,'Source Data'!Q${dr},\"\")`
  ]];
}
summary.getRange(`A11:H${10 + rows.length}`).format = { borders: { insideHorizontal: { style: "thin", color: lightBorder } } };
summary.getRange(`B11:B${10 + rows.length}`).format.numberFormat = "$#,##0.00";
summary.getRange(`C11:D${10 + rows.length}`).format.numberFormat = "#,##0";
summary.getRange(`G11:G${10 + rows.length}`).format = { fill: paleYellow, numberFormat: "#,##0" };
summary.getRange(`H11:H${10 + rows.length}`).format = { fill: paleGreen, numberFormat: "$#,##0" };
summary.getRange("A24:H25").merge();
summary.getRange("A24").values = [["Interpretation: review counts, ratings, saves and secondary-market data are demand proxies—not unit sales. Use the yellow Scenario Units cells in Source Data to enter distributor or marketplace sell-through; revenue updates automatically."]];
summary.getRange("A24:H25").format = { fill: paleYellow, font: { color: "#7A4E00" }, wrapText: true, verticalAlignment: "center" };
summary.freezePanes.freezeRows(3);
const sw = [30, 14, 14, 14, 26, 13, 17, 20];
for (let i = 0; i < sw.length; i++) summary.getRangeByIndexes(0, i, 25, 1).format.columnWidth = sw[i];
summary.getRange("10:10").format.rowHeight = 34;

// Methodology sheet
method.showGridLines = false;
method.getRange("A1:F2").merge(); method.getRange("A1").values = [["Methodology, Definitions & Data Limitations"]];
method.getRange("A1:F2").format = { fill: navy, font: { color: "#FFFFFF", bold: true, size: 18 }, verticalAlignment: "center" };
const notes = [
  ["Metric", "Definition / use", "What it is not", "Update guidance", "Primary source", "Confidence rule"],
  ["Price USD", "Captured US manufacturer or retailer price as of 2026-08-23", "Global MSRP or tax-inclusive worldwide price", "Refresh before pricing decisions", "Manufacturer / Sweetwater / Equipboard", "High when current retailer page exposes price"],
  ["Public Monthly Units", "Only units explicitly exposed by a platform or seller", "An estimate inferred from reviews", "Populate from sell-through, Amazon Sales Trend or channel reports", "No reviewed SKU exposed a stable value", "Blank means unavailable, not zero"],
  ["Sales Proxy", "Review count, rating count, saves, price-history or secondary-market signal", "Comparable unit sales across channels", "Use within a product/channel, not as a universal conversion", "Retailer and marketplace pages", "Medium unless SKU/channel ambiguity exists"],
  ["English Coverage Count", "Exact model-name matches in the 242-source English library", "Total web article count or paid-media spend", "Regenerate after expanding the source library", "英语文章与社区来源库_200plus.md", "Sample-bound metric"],
  ["Scenario Monthly Units", "Editable planning assumption", "Observed market fact", "Replace with distributor/platform data", "User input", "Yellow cells"],
  ["Scenario Revenue", "Price × Scenario Monthly Units", "Net revenue, gross margin or channel receipts", "Add discount, returns and channel margin for financial planning", "Workbook formula", "Formula-driven"]
];
method.getRange("A4:F10").values = notes;
method.getRange("A4:F4").format = { fill: blue, font: { color: "#FFFFFF", bold: true }, wrapText: true };
method.getRange("A5:F10").format = { wrapText: true, verticalAlignment: "top", borders: { insideHorizontal: { style: "thin", color: lightBorder } } };
for (let i = 0; i < 6; i++) method.getRangeByIndexes(0, i, 10, 1).format.columnWidth = [24, 42, 35, 38, 40, 24][i];
method.getRange("4:4").format.rowHeight = 34;
method.getRange("5:10").format.rowHeight = 64;
method.freezePanes.freezeRows(4);

const summaryCheck = await wb.inspect({ kind: "table", range: "Sales Summary!A1:H25", include: "values,formulas", tableMaxRows: 25, tableMaxCols: 8 });
console.log(summaryCheck.ndjson);
const errorCheck = await wb.inspect({ kind: "match", searchTerm: "#REF!|#DIV/0!|#VALUE!|#NAME\\?|#N/A", options: { useRegex: true, maxResults: 100 }, summary: "formula error scan" });
console.log(errorCheck.ndjson);

for (const [name, range] of [["Sales Summary", "A1:H25"], ["Source Data", `A1:Q${rows.length + 1}`], ["Methodology", "A1:F10"]]) {
  const preview = await wb.render({ sheetName: name, range, scale: 1.2, format: "png" });
  await fs.writeFile(`${outputDir}/preview_${name.replace(/ /g, "_")}.png`, new Uint8Array(await preview.arrayBuffer()));
}

const out = await SpreadsheetFile.exportXlsx(wb);
await out.save(`${outputDir}/竞品销售与宣发情况.xlsx`);
console.log(`${outputDir}/竞品销售与宣发情况.xlsx`);
