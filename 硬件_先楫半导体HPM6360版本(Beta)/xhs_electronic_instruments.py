#!/usr/bin/env python3
"""Collect public Xiaohongshu search cards with gentle, human-paced browsing."""

from __future__ import annotations

import argparse
import asyncio
import csv
import json
import random
from datetime import datetime
from pathlib import Path
from urllib.parse import quote

from playwright.async_api import Page, async_playwright


KEYWORD = "电声乐器"


async def pause(page: Page, low: int = 700, high: int = 1800) -> None:
    await page.wait_for_timeout(random.randint(low, high))


async def gentle_scroll(page: Page) -> None:
    # Small variable scrolls emulate ordinary reading without fingerprint spoofing.
    for _ in range(random.randint(2, 4)):
        await page.mouse.wheel(0, random.randint(350, 850))
        await pause(page, 550, 1400)


async def collect_cards(page: Page, limit: int) -> list[dict[str, str]]:
    items: dict[str, dict[str, str]] = {}
    for _ in range(12):
        cards = page.locator("section.note-item, .note-item")
        for i in range(await cards.count()):
            card = cards.nth(i)
            link = card.locator("a[href*='/explore/'], a[href*='/search_result/']").first
            if not await link.count():
                continue
            href = await link.get_attribute("href") or ""
            if not href:
                continue
            if href.startswith("/"):
                href = "https://www.xiaohongshu.com" + href
            title_node = card.locator(".title, [class*='title']").first
            author_node = card.locator(".author, [class*='author'], .name").first
            title = (await title_node.inner_text()).strip() if await title_node.count() else ""
            author = (await author_node.inner_text()).strip() if await author_node.count() else ""
            if title:
                items[href] = {"title": title, "author": author, "url": href}
            if len(items) >= limit:
                return list(items.values())[:limit]
        await gentle_scroll(page)
    return list(items.values())[:limit]


async def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--limit", type=int, default=10)
    parser.add_argument("--headed", action="store_true")
    parser.add_argument("--output", type=Path, default=Path("xhs_electronic_instruments.json"))
    args = parser.parse_args()

    async with async_playwright() as pw:
        browser = await pw.chromium.launch(headless=not args.headed)
        context = await browser.new_context(
            locale="zh-CN",
            timezone_id="Asia/Shanghai",
            viewport={"width": 1365, "height": 850},
        )
        page = await context.new_page()
        url = f"https://www.xiaohongshu.com/search_result?keyword={quote(KEYWORD)}&source=web_search_result_notes"
        await page.goto(url, wait_until="domcontentloaded", timeout=60_000)
        await pause(page, 1800, 3200)

        body = await page.locator("body").inner_text()
        blocked_words = ("验证码", "安全验证", "登录后查看", "安全限制", "IP存在风险")
        if any(word in body for word in blocked_words):
            await page.screenshot(path="xhs_blocked.png", full_page=False)
            print("页面要求登录或验证；已停止，未尝试绕过。")
            await browser.close()
            return 2

        items = await collect_cards(page, args.limit)
        captured_at = datetime.now().astimezone().isoformat(timespec="seconds")
        for item in items:
            item["keyword"] = KEYWORD
            item["captured_at"] = captured_at

        args.output.write_text(json.dumps(items, ensure_ascii=False, indent=2), encoding="utf-8")
        csv_path = args.output.with_suffix(".csv")
        with csv_path.open("w", newline="", encoding="utf-8-sig") as f:
            writer = csv.DictWriter(f, fieldnames=["title", "author", "url", "keyword", "captured_at"])
            writer.writeheader()
            writer.writerows(items)
        await page.screenshot(path="xhs_search_result.png", full_page=False)
        await browser.close()
        print(f"采集 {len(items)} 条，JSON: {args.output}，CSV: {csv_path}")
        return 0 if len(items) >= args.limit else 3


if __name__ == "__main__":
    raise SystemExit(asyncio.run(main()))
