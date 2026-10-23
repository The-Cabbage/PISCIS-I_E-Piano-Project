import asyncio, importlib.util
from urllib.parse import quote, urlparse

SPEC=importlib.util.spec_from_file_location('collector','/Users/admin/Desktop/Code/PISCIS-I_E-Piano_Project/市场调查资料/build_english_source_library.py')
c=importlib.util.module_from_spec(SPEC); SPEC.loader.exec_module(c)
PRODUCTS=[
 'Yamaha reface CP','Yamaha reface CS','Yamaha reface DX','Yamaha reface YC',
 'Teenage Engineering OP-1 field','Teenage Engineering OP-XY','Sonicware ELZ_1 play',
 'Sonicware LIVEN Ambient Zero','Sonicware LIVEN Texture Lab','Sonicware LIVEN XFM','Sonicware LIVEN 8bit warps',
 'Critter and Guitari 201 Pocket Piano','Critter and Guitari Organelle M','Suzuki Omnichord OM-108',
 'Stylophone Gen X-1','Korg microKORG 2','Arturia MicroFreak','Arturia MiniFreak','ASM Hydrasynth Explorer',
 'Casio CT-S1000V','Roland GO KEYS 5','Akai MPC Key 37','Polyend Synth','Artiphon Chorda',
 'ROLI Piano M','Expressive E Osmose','Tembo musical instrument','Melo-D AI guitar']
SITES=[
 'https://synthanatomy.com/?s={q}',
 'https://www.synthtopia.com/?s={q}',
 'https://www.musicradar.com/search?searchTerm={q}',
 'https://www.attackmagazine.com/?s={q}',
 'https://www.musictech.net/?s={q}',
]
BAD=('/tag/','/category/','/author/','/page/','/search','/feed','/wp-content','javascript:')

async def search_sites(page, product, pages=1):
 out=[]
 for template in SITES:
  try:
   await page.goto(template.format(q=quote(product)),wait_until='domcontentloaded',timeout=26000)
   await page.wait_for_timeout(300)
   host=urlparse(page.url).netloc.removeprefix('www.')
   links=page.locator('article a, h2 a, h3 a')
   seen=set()
   for i in range(min(await links.count(),100)):
    a=links.nth(i); href=await a.get_attribute('href'); title=c.clean(await a.inner_text())
    if not href or not href.startswith('http') or href in seen or any(x in href for x in BAD): continue
    if urlparse(href).netloc.removeprefix('www.') != host: continue
    if len(title)<8: continue
    seen.add(href); out.append({'title':title,'url':c.canonicalize(href),'search_snippet':title,'query':product})
    if len(seen)>=12: break
  except Exception: continue
 return out

c.QUERIES=PRODUCTS
c.search_bing=search_sites
asyncio.run(c.main())
