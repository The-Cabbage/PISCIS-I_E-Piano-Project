import json
from collections import Counter,defaultdict
from pathlib import Path
from urllib.parse import urlparse

PRODUCTS=[
('Yamaha reface CP','Mini keys','Standalone stage electric piano','reface'),('Yamaha reface CS','Mini keys','Standalone virtual-analog synth','reface'),('Yamaha reface DX','Mini keys','Standalone FM synth','reface'),('Yamaha reface YC','Mini keys','Standalone combo organ','reface'),('Teenage Engineering OP-1 field','Slim piano keyboard','Standalone synth/sampler/workstation','op-1'),('Teenage Engineering OP-XY','Slim piano keyboard','Standalone sequencer/synth/sampler','op-xy'),('Sonicware ELZ_1 play','Mini keys','Battery-powered standalone synth','elz'),('Sonicware LIVEN Ambient Zero','Step keys','Standalone ambient groove synth','ambient'),('Sonicware LIVEN Texture Lab','Step keys','Standalone granular synth/effects instrument','texture lab'),('Sonicware LIVEN XFM','Step keys','Standalone FM groove synth','liven xfm'),('Sonicware LIVEN 8bit warps','Step keys','Standalone chiptune synth','8bit warps'),('Critter and Guitari 201 Pocket Piano','Wooden piano keys','Portable standalone synth','pocket piano'),('Critter and Guitari Organelle M','Maple keys','Open-source standalone instrument','organelle'),('Suzuki Omnichord OM-108','Chord buttons + touch strum plate','Standalone electronic chord instrument','omnichord'),('Stylophone Gen X-1','Stylus touch keyboard','Standalone analog pocket synth','stylophone'),('Korg microKORG 2','Mini keys','Standalone performance/vocoder synth','microkorg'),('Arturia MicroFreak','Capacitive poly-aftertouch keyboard','Standalone hybrid synth','microfreak'),('Arturia MiniFreak','Slim keys','Standalone polyphonic hybrid synth','minifreak'),('ASM Hydrasynth Explorer','Poly-aftertouch mini keys','Standalone wavemorphing synth','hydrasynth'),('Casio CT-S1000V','61 full-size keys','Standalone vocal synthesis keyboard','ct-s1000v'),('Roland GO KEYS 5','61 velocity keys','Standalone arranger/creative keyboard','go:keys'),('Akai MPC Key 37','37 full-size keys + pads','Standalone sampling workstation','mpc key'),('Polyend Synth','Pad-key isomorphic layout','Standalone multi-engine synth','polyend synth'),('Artiphon Chorda','Capacitive multi-gesture surface','Standalone synth/looper/controller','chorda'),('ROLI Piano M','Illuminated MPE keys','Expressive keyboard + app ecosystem','piano m'),('Expressive E Osmose','3D expressive keys','Standalone expressive synth','osmose'),('Tembo musical instrument','Play-oriented note surface','Standalone family music instrument','tembo'),('Melo-D AI guitar','Guided fret/keyboard-like notes','Generative-AI playable instrument','melo-d')]
aliases={p:[a] for p,_,_,a in PRODUCTS}; aliases['Yamaha reface YC']=['reface','reface yc'];aliases['Roland GO KEYS 5']=['go:keys','go keys']
raw=json.load(open('/tmp/english_source_library.json')); by=defaultdict(list)
for x in raw:
 text=' '.join([x.get('title',''),x.get('description',''),x.get('excerpt','')]).lower()
 x['relevance']='Exact/model mention' if any(a in text for a in aliases[x['query']]) else 'Adjacent discovery'
 by[x['query']].append(x)
selected=[]
for p,_,_,_ in PRODUCTS:
 strong=[x for x in by[p] if x['relevance'].startswith('Exact')]
 adjacent=[x for x in by[p] if not x['relevance'].startswith('Exact')]
 take=strong+adjacent[:max(0,8-len(strong))]
 selected.extend(take)
seen={};
for x in selected:seen.setdefault(x.get('final_url') or x['url'],x)
rows=list(seen.values());rows.sort(key=lambda x:(x['query'],x['relevance']!='Exact/model mention',x.get('published')=='Not stated',x['title']))
counts=Counter(x['query'] for x in rows); exact=Counter(x['query'] for x in rows if x['relevance'].startswith('Exact'))
Path('/tmp/keyboard_instrument_sources.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8')
lines=['# Keyboard-like Complete Playable Instruments — Competitor & English Source Library','',f'> Generated: 2026-08-31  ',f'> Products: **{len(PRODUCTS)}** · Unique sources: **{len(rows)}** · Exact/model-match sources: **{sum(exact.values())}** · Sources exposing publication time: **{sum(x.get("published")!="Not stated" for x in rows)}**.','',
'## Scope and evidence labels','', '- Included interaction: physical keys, touch keys, isomorphic pad keys, chord/strum surfaces, or guided fret-style note interfaces.','- Products are standalone or near-standalone playable instruments; pure effects processors and ordinary MIDI-only controllers are excluded.','- **Exact/model mention**: the model or unambiguous family name appears in the page title, description, or extracted body.','- **Adjacent discovery**: returned by an English specialist publication’s product-specific site search but does not contain a strong model match; retained for category/positioning discovery and clearly labeled.','- `Not stated` means the page exposed no reliable publication time; the crawl date is never substituted.','',
'## Product shortlist (28)','', '| # | Product | Keyboard-like interaction | Complete-instrument role | Sources | Exact matches |','|---:|---|---|---|---:|---:|']
for i,(p,inter,role,_) in enumerate(PRODUCTS,1):lines.append(f'| {i} | {p} | {inter} | {role} | {counts[p]} | {exact[p]} |')
lines += ['','## Design implications','', '- The most repeatable portable formula is **small keys + internal sound engine + battery/speaker + immediate sequencing or recording**.','- Step/pad keys save space, but need velocity, aftertouch, scale locking, chord memory or visual feedback to feel like an instrument rather than a controller.','- OP-1 field, MPC Key 37 and Organelle M represent polished closed workstation, production workstation and open programmable instrument respectively.','- Osmose, Hydrasynth Explorer, MicroFreak and Piano M make expressive sensing the primary product story.','- CT-S1000V and Melo-D suggest that AI/vocal-generation layers work best when deterministic note-level control remains available.','',
'## English reports, reviews and community/video-adjacent sources','', '| # | Published | Evidence | Product/query | Type | Source | Title and URL | Summary |','|---:|---|---|---|---|---|---|---|']
for i,x in enumerate(rows,1):
 title=' '.join(x['title'].split()).replace('|','\\|'); summ=' '.join((x.get('description') or x.get('search_snippet') or x.get('excerpt') or '').split())[:260].replace('|','\\|');url=x.get('final_url') or x['url']
 lines.append(f'| {i} | {x.get("published","Not stated")} | {x["relevance"]} | {x["query"]} | {x.get("type","English article / review")} | {x.get("source",urlparse(url).netloc)} | [{title}]({url}) | {summ} |')
Path('/tmp/键盘式完整可演奏乐器_竞品与英语来源库_200plus.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print({'products':len(PRODUCTS),'sources':len(rows),'exact':sum(exact.values()),'dated':sum(x.get('published')!='Not stated' for x in rows),'ok':sum(x.get('status')=='ok' for x in rows)},flush=True)
