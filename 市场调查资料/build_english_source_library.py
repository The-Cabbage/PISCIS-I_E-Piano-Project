from __future__ import annotations

import asyncio
import json
import re
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from urllib.parse import parse_qs, unquote, urlparse

from playwright.async_api import async_playwright, TimeoutError as PlaywrightTimeoutError


QUERIES = [
    # Core products
    "Korg Volca review synth English",
    "Korg Volca Sample review English",
    "Korg Volca Drum review English",
    "Korg Volca FM review English",
    "Korg Volca Modular review English",
    "Korg Volca forum discussion",
    "Yamaha reface CP review English",
    "Yamaha reface CP long term review",
    "Yamaha reface CP forum discussion",
    "Teenage Engineering PO-33 review English",
    "PO-33 K.O long term review",
    "PO-33 sampler forum discussion",
    "Neural DSP Quad Cortex review English",
    "Quad Cortex long term review",
    "Quad Cortex forum discussion",
    "Neural DSP Nano Cortex review",
    # Portable instruments / grooveboxes
    "portable groovebox review sampler synthesizer",
    "best portable sampler review groovebox",
    "Teenage Engineering OP-Z review",
    "Teenage Engineering OP-1 Field review",
    "Teenage Engineering EP-133 KO II review",
    "Sonicware SmplTrek review",
    "Sonicware Liven review portable groovebox",
    "Roland P-6 sampler review",
    "Roland AIRA Compact review",
    "Yamaha SEQTRAK review",
    "Ableton Move review hardware",
    "Novation Circuit Rhythm review",
    "Elektron Model Samples review",
    "Dirtywave M8 tracker review",
    "Woovebox review groovebox",
    "Polyend Tracker Mini review",
    "Critter Guitari Organelle review",
    "CHOMPI sampler review",
    "1010music nanobox review sampler",
    "Bastl Microgranny review sampler",
    "Korg NTS-1 review DIY synth",
    "Artiphon Orba review musical instrument",
    "Torso S-4 sculpting sampler review",
    # Geek toy / cyberdeck / DIY
    "music cyberdeck synthesizer",
    "Raspberry Pi music cyberdeck synth",
    "DIY portable synthesizer cyberdeck",
    "open source hardware musical instrument",
    "experimental electronic musical instrument hardware",
    "playful electronic musical instrument design",
    "game console synthesizer instrument",
    "chiptune hardware synthesizer review",
    # AI-native audio / instruments
    "AI musical instrument hardware",
    "AI native audio instrument",
    "neural audio hardware instrument",
    "generative AI musical instrument review",
    "AI guitar hardware review",
    "AI effects pedal review",
    "neural sampling instrument",
    "DDSP musical instrument hardware",
    "NSynth Super musical instrument",
    "Roland Project Lydia neural sampling",
    "Polyend Endless AI pedal",
    "intelligent musical instruments research",
    "machine learning new interfaces musical expression",
    # Additional named competitors and design references
    "Elektron Digitakt II review",
    "Elektron Digitone review portable synth",
    "Synthstrom Deluge review",
    "Akai MPC Live II review portable",
    "Akai MPC One Plus review",
    "Roland SP-404MKII review",
    "Roland MC-101 review groovebox",
    "Roland SH-4d review",
    "Roland J-6 chord synth review",
    "Roland T-8 beat machine review",
    "Roland S-1 tweak synth review",
    "Korg Electribe 2 sampler review",
    "Korg NTS-3 kaoss pad review",
    "Korg Drumlogue review",
    "Korg Monotron Delay review",
    "Teenage Engineering OP-XY review",
    "Teenage Engineering EP-1320 review",
    "Teenage Engineering EP-40 review",
    "Yamaha MODX review portability workflow",
    "Yamaha Reface CS review",
    "Yamaha Reface DX review",
    "Yamaha Reface YC review",
    "Sonicware Liven Texture Lab review",
    "Sonicware Liven LoFi 12 XT review",
    "Sonicware ELZ_1 play review",
    "Modal Skulpt SE review portable synth",
    "Modal CraftSynth 2 review",
    "IK Multimedia UNO Synth Pro review",
    "IK Multimedia TONEX pedal review AI machine modeling",
    "Kemper Profiler Player review",
    "Line 6 HX Stomp review",
    "Fender Tone Master Pro review",
    "Headrush Core review amp modeler",
    "Positive Grid Spark review smart amp",
    "Positive Grid Spark 2 AI review",
    "MOD Dwarf review open source pedal",
    "Empress ZOIA review modular pedal",
    "Hologram Microcosm review",
    "Bela Pepper DIY synth review",
    "Bela Gliss instrument interface",
    "Daisy Seed synthesizer projects",
    "Monome Norns review",
    "Monome Norns Shield review",
    "Norns open source musical instrument",
    "Playtronica TouchMe review instrument",
    "MIDI controller expressive instrument review LinnStrument",
    "ROLI Seaboard Block review",
    "Expressive E Osmose review",
    "Artiphon Chorda review",
    "Aodyo Anyma Phi review physical modeling synth",
    "Neutone Morpho review AI hardware",
    "Magenta DDSP VST musical instrument",
    "Google Magenta NSynth Super article",
    "AI music hardware instrument NIME",
    "NIME intelligent musical instrument paper",
    "timbre transfer real time musical instrument",
    "neural amp capture comparison Quad Cortex Tonex Kemper",
    "portable music workstation hardware review",
    "pocket synthesizer review battery powered",
    "experimental sampler instrument review",
]

BLOCKED_DOMAINS = {
    "amazon.com", "ebay.com", "aliexpress.com", "pinterest.com",
    "facebook.com", "instagram.com", "tiktok.com",
}

DATE_KEYS = (
    "article:published_time", "datePublished", "datepublished",
    "date", "datecreated", "pubdate", "publishdate",
)


def clean(value: str | None) -> str:
    return re.sub(r"\s+", " ", value or "").strip()


def canonicalize(url: str) -> str:
    parsed = urlparse(url)
    if "bing.com" in parsed.netloc and parsed.path.startswith("/ck/a"):
        target = parse_qs(parsed.query).get("u", [""])[0]
        if target.startswith("a1"):
            try:
                import base64
                target = base64.b64decode(target[2:] + "===").decode("utf-8")
            except Exception:
                pass
        if target.startswith("http"):
            url = target
            parsed = urlparse(url)
    path = re.sub(r"/$", "", parsed.path) or "/"
    return parsed._replace(query="", fragment="", path=path).geturl()


def allowed(url: str) -> bool:
    try:
        host = urlparse(url).netloc.lower().removeprefix("www.")
        return url.startswith("http") and not any(host == d or host.endswith("." + d) for d in BLOCKED_DOMAINS)
    except Exception:
        return False


def classify(url: str) -> str:
    host = urlparse(url).netloc.lower()
    if "reddit.com" in host or "forum" in host or "gearspace" in host or "thegearpage" in host:
        return "Community discussion"
    if "arxiv.org" in host or ".edu" in host or "acm.org" in host or "ieee.org" in host:
        return "Research"
    if any(x in host for x in ("korg.com", "yamaha.com", "teenage.engineering", "neuraldsp.com", "roland.com")):
        return "Official / manufacturer"
    return "English article / review"


def tags_for(text: str) -> list[str]:
    t = text.lower()
    groups = {
        "Korg Volca": ("volca",),
        "Yamaha reface": ("reface",),
        "PO-33 / Teenage Engineering": ("po-33", "po33", "pocket operator"),
        "Neural DSP": ("neural dsp", "quad cortex", "nano cortex"),
        "Portable groovebox": ("groovebox", "sampler", "op-z", "seqtrak", "smpltrek", "dirtywave", "woovebox", "circuit rhythm"),
        "CyberDeck / DIY": ("cyberdeck", "raspberry pi", "diy synth", "open source hardware"),
        "AI-native audio": (" ai ", "neural", "machine learning", "ddsp", "nsynth", "intelligent musical"),
    }
    return [name for name, needles in groups.items() if any(n in f" {t} " for n in needles)] or ["Adjacent product"]


async def search_bing(page, query: str, pages: int = 2) -> list[dict]:
    found = []
    for index in range(pages):
        first = index * 10 + 1
        url = f"https://www.bing.com/search?q={query.replace(' ', '+')}&first={first}&setlang=en-US"
        try:
            await page.goto(url, wait_until="domcontentloaded", timeout=25000)
            await page.wait_for_timeout(500)
            cards = page.locator("li.b_algo")
            count = await cards.count()
            for i in range(count):
                card = cards.nth(i)
                link = card.locator("h2 a").first
                href = await link.get_attribute("href")
                title = clean(await link.inner_text())
                snippet = clean(await card.inner_text())
                if href and title:
                    href = canonicalize(href)
                    if allowed(href):
                        found.append({"title": title, "url": href, "search_snippet": snippet, "query": query})
        except Exception:
            continue
    return found


async def search_one(context, query: str, semaphore: asyncio.Semaphore) -> list[dict]:
    async with semaphore:
        page = await context.new_page()
        try:
            return await search_bing(page, query, pages=1)
        finally:
            await page.close()


async def extract_article(context, candidate: dict, semaphore: asyncio.Semaphore) -> dict:
    async with semaphore:
        page = await context.new_page()
        row = dict(candidate)
        try:
            await page.goto(candidate["url"], wait_until="domcontentloaded", timeout=22000)
            row["final_url"] = canonicalize(page.url)
            page_title = clean(await page.title())
            if page_title:
                row["title"] = page_title
            data = await page.evaluate("""() => {
              const pick = (sel, attr='content') => document.querySelector(sel)?.getAttribute(attr) || '';
              const metas = {};
              for (const m of document.querySelectorAll('meta')) {
                const k = (m.getAttribute('property') || m.getAttribute('name') || m.getAttribute('itemprop') || '').toLowerCase();
                if (k) metas[k] = m.getAttribute('content') || '';
              }
              const times = [...document.querySelectorAll('time')].map(x => x.getAttribute('datetime') || x.textContent || '').filter(Boolean);
              const ld = [...document.querySelectorAll('script[type="application/ld+json"]')].map(x => x.textContent || '');
              const desc = metas['description'] || metas['og:description'] || metas['twitter:description'] || '';
              const body = (document.querySelector('article') || document.querySelector('main') || document.body)?.innerText || '';
              return {metas, times, ld, desc, body: body.slice(0, 7000), lang: document.documentElement.lang || ''};
            }""")
            date = ""
            date_source = ""
            for key, value in data["metas"].items():
                if any(k.lower() in key for k in DATE_KEYS) and value:
                    date, date_source = clean(value), f"meta:{key}"
                    break
            if not date:
                for raw in data["ld"]:
                    match = re.search(r'"datePublished"\s*:\s*"([^"]+)"', raw, re.I)
                    if match:
                        date, date_source = match.group(1), "JSON-LD:datePublished"
                        break
            if not date and data["times"]:
                date, date_source = clean(data["times"][0]), "time element"
            row.update({
                "published": date or "Not stated",
                "date_source": date_source or "Not available",
                "description": clean(data["desc"])[:600],
                "excerpt": clean(data["body"])[:1200],
                "language": clean(data["lang"]) or "unknown",
                "status": "ok",
            })
        except PlaywrightTimeoutError:
            row.update({"published": "Not stated", "date_source": "Timeout", "description": "", "excerpt": "", "language": "unknown", "status": "timeout"})
        except Exception as exc:
            row.update({"published": "Not stated", "date_source": "Error", "description": "", "excerpt": "", "language": "unknown", "status": f"error: {clean(str(exc))[:120]}"})
        finally:
            await page.close()
        row["source"] = urlparse(row.get("final_url", row["url"])).netloc.removeprefix("www.")
        row["type"] = classify(row.get("final_url", row["url"]))
        row["tags"] = tags_for(" ".join([row.get("title", ""), row.get("description", ""), row.get("query", "")]))
        return row


async def main() -> None:
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=True)
        context = await browser.new_context(locale="en-US", user_agent="Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 Chrome/126 Safari/537.36")
        search_semaphore = asyncio.Semaphore(8)
        search_batches = await asyncio.gather(*(search_one(context, query, search_semaphore) for query in QUERIES))
        candidates = [item for batch in search_batches for item in batch]
        existing_path = Path("english_source_library.json")
        if existing_path.exists():
            for row in json.loads(existing_path.read_text(encoding="utf-8")):
                candidates.append({
                    "title": row.get("title", ""),
                    "url": row.get("final_url") or row.get("url", ""),
                    "search_snippet": row.get("search_snippet", ""),
                    "query": row.get("query", "previous crawl"),
                })
        print(f"search complete queries={len(QUERIES)} candidates={len(candidates)}", flush=True)
        deduped = {}
        for item in candidates:
            deduped.setdefault(canonicalize(item["url"]), item)
        # Keep a generous pool so that blocked and undated pages do not prevent 200+ delivery.
        pool = list(deduped.values())[:420]
        semaphore = asyncio.Semaphore(8)
        rows = []
        for start in range(0, len(pool), 40):
            batch = await asyncio.gather(*(extract_article(context, c, semaphore) for c in pool[start:start+40]))
            rows.extend(batch)
            print(f"extract {len(rows)}/{len(pool)}", flush=True)
        await browser.close()

    # Prefer successfully opened, English-looking, content-rich pages.
    rows.sort(key=lambda r: (r["status"] != "ok", r["published"] == "Not stated", len(r.get("excerpt", "")) < 100, r["source"], r["title"]))
    Path("english_source_library.json").write_text(json.dumps(rows, ensure_ascii=False, indent=2), encoding="utf-8")

    lines = [
        "# English Article & Community Source Library (200+)", "",
        f"> Generated: {datetime.now(timezone.utc).date().isoformat()}  ",
        f"> Unique sources collected: **{len(rows)}**. Dates are taken from page metadata, JSON-LD, or visible `<time>` elements. `Not stated` means no reliable publication date was exposed by the page.", "",
        "## Fields", "",
        "- **Published**: publication date as exposed by the page; not the crawl date.",
        "- **Type**: English article/review, research, official/manufacturer, or community discussion.",
        "- **Tags**: product family or opportunity-space labels.", "",
        "## Sources", "",
        "| # | Published | Type | Tags | Source | Title and URL | Summary |", "|---:|---|---|---|---|---|---|",
    ]
    for i, r in enumerate(rows, 1):
        title = clean(r["title"]).replace("|", "\\|")
        summary = clean(r.get("description") or r.get("search_snippet") or r.get("excerpt"))[:280].replace("|", "\\|")
        tags = ", ".join(r["tags"]).replace("|", "\\|")
        url = r.get("final_url") or r["url"]
        lines.append(f'| {i} | {r["published"]} | {r["type"]} | {tags} | {r["source"]} | [{title}]({url}) | {summary} |')
    lines += ["", "## Coverage", "", f"- By type: `{dict(Counter(r['type'] for r in rows))}`", f"- By tag: `{dict(Counter(t for r in rows for t in r['tags']))}`", ""]
    Path("英语文章与社区来源库_200plus.md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"unique": len(rows), "dated": sum(r["published"] != "Not stated" for r in rows), "ok": sum(r["status"] == "ok" for r in rows)}), flush=True)


if __name__ == "__main__":
    asyncio.run(main())
