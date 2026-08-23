from __future__ import annotations

import json
from pathlib import Path

from playwright.sync_api import sync_playwright


URLS = [
    "https://www.korg.com/us/products/dj/volca_sample2/",
    "https://usa.yamaha.com/products/music_production/synthesizers/reface/reface_cp.html",
    "https://teenage.engineering/products/po-33",
    "https://neuraldsp.com/quad-cortex",
    "https://www.reddit.com/r/synthesizers/comments/vks0s4/what_do_people_think_of_the_yamaha_reface_cp/",
    "https://www.reddit.com/r/teenageengineering/comments/1j2fnc0/is_the_po33_still_relevant_in_2025/",
    "https://www.reddit.com/r/NeuralDSP/comments/162d30f/quad_cortex_brutally_honest_opinions/",
    "https://www.dogsonacid.com/threads/korg-volca-appreciation-thread.820110/",
    "https://www.soundonsound.com/reviews/sonicware-smpltrek",
    "https://www.soundonsound.com/reviews/teenage-engineering-op-z",
    "https://hackaday.com/2021/09/09/musical-cyberdeck-is-part-synth-part-midi-controller-and-all-cool/",
    "https://www.musicradar.com/reviews/neural-dsp-quad-cortex",
    "https://guitar.com/reviews/electric-guitar/hands-on-tempolor-melo-d-review/",
    "https://arxiv.org/abs/2604.23583",
]


def clean(text: str) -> str:
    return " ".join(text.split())


def main() -> None:
    rows = []
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        context = browser.new_context(
            locale="en-US",
            user_agent=(
                "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) "
                "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126 Safari/537.36"
            ),
        )
        page = context.new_page()
        for url in URLS:
            row = {"url": url}
            try:
                page.goto(url, wait_until="domcontentloaded", timeout=30000)
                row["final_url"] = page.url
                row["title"] = clean(page.title())
                desc = page.locator('meta[name="description"]').get_attribute("content")
                row["description"] = clean(desc or "")
                body = clean(page.locator("body").inner_text(timeout=10000))
                row["excerpt"] = body[:3500]
                row["status"] = "ok"
            except Exception as exc:
                row["status"] = "error"
                row["error"] = clean(str(exc))[:500]
            rows.append(row)
        browser.close()
    Path("playwright_market_sources.json").write_text(
        json.dumps(rows, ensure_ascii=False, indent=2), encoding="utf-8"
    )
    print(json.dumps({"count": len(rows), "ok": sum(r["status"] == "ok" for r in rows)}))


if __name__ == "__main__":
    main()
