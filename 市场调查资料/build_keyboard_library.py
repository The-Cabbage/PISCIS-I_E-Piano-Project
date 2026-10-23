import asyncio, importlib.util, json, re
from collections import Counter
from pathlib import Path
from urllib.parse import quote, urlparse, urljoin
from playwright.async_api import async_playwright

SPEC=importlib.util.spec_from_file_location('collector','/Users/admin/Desktop/Code/PISCIS-I_E-Piano_Project/市场调查资料/build_english_source_library.py')
c=importlib.util.module_from_spec(SPEC);SPEC.loader.exec_module(c)
PRODUCTS=[
 ('Yamaha reface CP','Mini keys','Standalone stage electric piano','reface'),('Yamaha reface CS','Mini keys','Standalone virtual-analog synth','reface'),('Yamaha reface DX','Mini keys','Standalone FM synth','reface'),('Yamaha reface YC','Mini keys','Standalone combo organ','reface'),
 ('Teenage Engineering OP-1 field','Slim piano keyboard','Standalone synth/sampler/workstation','op-1'),('Teenage Engineering OP-XY','Slim piano keyboard','Standalone sequencer/synth/sampler','op-xy'),
 ('Sonicware ELZ_1 play','Mini keys','Battery-powered standalone synth','elz'),('Sonicware LIVEN Ambient Ø','Step keys','Standalone ambient groove synth','ambient'),('Sonicware LIVEN Texture Lab','Step keys','Standalone granular synth/effects instrument','texture lab'),('Sonicware LIVEN XFM','Step keys','Standalone FM groove synth','liven xfm'),('Sonicware LIVEN 8bit warps','Step keys','Standalone chiptune synth','8bit warps'),
 ('Critter & Guitari 201 Pocket Piano','Wooden piano keys','Portable standalone synth','pocket piano'),('Critter & Guitari Organelle M','Maple keys','Open-source standalone instrument','organelle'),('Suzuki Omnichord OM-108','Chord buttons + touch strum plate','Standalone electronic chord instrument','omnichord'),('Stylophone Gen X-1','Stylus touch keyboard','Standalone analog pocket synth','stylophone'),('Korg microKORG 2','Mini keys','Standalone performance/vocoder synth','microkorg'),
 ('Arturia MicroFreak','Capacitive poly-aftertouch keyboard','Standalone hybrid synth','microfreak'),('Arturia MiniFreak','Slim keys','Standalone polyphonic hybrid synth','minifreak'),('ASM Hydrasynth Explorer','Poly-aftertouch mini keys','Standalone wavemorphing synth','hydrasynth'),('Casio CT-S1000V','61 full-size keys','Standalone vocal synthesis keyboard','ct-s1000v'),('Roland GO:KEYS 5','61 velocity keys','Standalone arranger/creative keyboard','go:keys'),('Akai MPC Key 37','37 full-size keys + pads','Standalone sampling workstation','mpc key'),('Polyend Synth','Pad-key isomorphic layout','Standalone multi-engine synth','polyend synth'),('Artiphon Chorda','Capacitive multi-gesture surface','Standalone synth/looper/controller instrument','chorda'),('ROLI Piano M','Illuminated MPE keys','Expressive keyboard with app ecosystem','piano m'),('Expressive E Osmose','3D expressive keys','Standalone expressive synth','osmose'),('Tembo','Play-oriented note surface','Standalone family music instrument','tembo'),('Melo-D','Fret/keyboard-like guided note interface','Generative-AI playable instrument','melo-d')]

ALIASES={p:[a] for p,_,_,a in PRODUCTS}
media=json.load(open('/tmp/english_source_library.json'))
direct=[]
for x in media:
 p=x['query']; title=x.get('title','').lower(); a=ALIASES.get(p,[p.lower()])
 if any(k in title for k in a): direct.append(x)

async def main():
 async with async_playwright() as pw:
  b=await pw.chromium.launch(headless=True);ctx=await b.new_context(locale='en-US')
  sem=asyncio.Semaphore(6)
  async def find(product):
   async with sem:
    pg=await ctx.new_page(); out=[]
    try:
     await pg.goto('https://www.youtube.com/results?search_query='+quote(product+' review demo'),wait_until='domcontentloaded',timeout=30000)
     await pg.wait_for_timeout(1000); links=pg.locator('a#video-title')
     for i in range(min(8,await links.count())):
      a=links.nth(i);href=await a.get_attribute('href');title=c.clean(await a.get_attribute('title') or await a.inner_text())
      if href and title:out.append({'title':title,'url':urljoin('https://www.youtube.com',href.split('&')[0]),'search_snippet':title,'query':product})
    except Exception:pass
    finally:await pg.close()
    return out
  batches=await asyncio.gather(*(find(p) for p,_,_,_ in PRODUCTS)); cand=[x for bch in batches for x in bch]
  uniq={c.canonicalize(x['url']):x for x in cand}
  rows=[]; esem=asyncio.Semaphore(7); vals=list(uniq.values())
  for i in range(0,len(vals),40):
   rows+=await asyncio.gather(*(c.extract_article(ctx,x,esem) for x in vals[i:i+40]));print('youtube extract',len(rows),'/',len(vals),flush=True)
  await b.close()
 return rows

videos=asyncio.run(main())
for x in videos:x['type']='Video review / community'
allrows={c.canonicalize(x.get('final_url') or x['url']):x for x in direct+videos}
rows=list(allrows.values());rows.sort(key=lambda x:(x['query'],x.get('published')=='Not stated',x['title']))
Path('/tmp/keyboard_instrument_sources.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8')
counts=Counter(x['query'] for x in rows)
lines=['# Keyboard-like Complete Playable Instruments — Competitor & English Source Library','',
 '> Generated: 2026-08-31  ',f'> Products: **{len(PRODUCTS)}** · Unique related sources: **{len(rows)}** · Sources exposing a publication/upload time: **{sum(x.get("published")!="Not stated" for x in rows)}**.','',
 '## Scope and screening rules','',
 '- Included: physical keys, touch-key surfaces, isomorphic pad keys, chord/strum interfaces, or guided fret-style note interfaces that support direct performance.','- Prioritized: standalone or near-standalone instruments capable of producing a musically complete performance, not pure effects processors.','- Publication time comes from page metadata, JSON-LD, visible `<time>` elements, or video upload metadata. `Not stated` means the page did not expose a reliable date.','',
 '## Product shortlist','',
 '| # | Product | Keyboard-like interaction | Instrument role | Related sources |','|---:|---|---|---|---:|']
for i,(p,interaction,role,_) in enumerate(PRODUCTS,1):lines.append(f'| {i} | {p} | {interaction} | {role} | {counts[p]} |')
lines += ['','## Design implications','',
 '- The strongest portable pattern is **small keys + internal sound engine + battery/speaker + immediate recording or sequencing**.','- Step-key and pad-key layouts reduce depth and cost, but need velocity, aftertouch, chord memory, scale locking, or strong visual feedback to feel like an instrument rather than a controller.','- OP-1 field, MPC Key 37 and Organelle M demonstrate three different definitions of “complete”: polished closed workstation, production workstation, and open programmable instrument.','- Osmose, Hydrasynth Explorer, MicroFreak and Piano M show that expressive key sensing can be a primary product story, not merely a specification.','- CT-S1000V and Melo-D are useful AI-native references: AI or vocal/generative layers work best when the player retains deterministic note-level control.','',
 '## English reports, reviews and community/video discussions','',
 '| # | Published / uploaded | Type | Product | Source | Title and URL | Summary |','|---:|---|---|---|---|---|---|']
for i,x in enumerate(rows,1):
 title=c.clean(x['title']).replace('|','\\|');summary=c.clean(x.get('description') or x.get('search_snippet') or x.get('excerpt'))[:280].replace('|','\\|');url=x.get('final_url') or x['url']
 lines.append(f'| {i} | {x.get("published","Not stated")} | {x.get("type","English article / review")} | {x["query"]} | {x.get("source",urlparse(url).netloc)} | [{title}]({url}) | {summary} |')
Path('/tmp/键盘式完整可演奏乐器_竞品与英语来源库_200plus.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print({'products':len(PRODUCTS),'sources':len(rows),'dated':sum(x.get('published')!='Not stated' for x in rows),'media':len(direct),'video':len(videos)},flush=True)
