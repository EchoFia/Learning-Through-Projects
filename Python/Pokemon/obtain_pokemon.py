''' 
Scrapes for the pokemon information at the following sites:
 - serebii.net (pokemon details)
 - https://www.pokemon.com/uk/pokedex (descriptions)
 - https://bulbapedia.bulbagarden.net/ (artwork)
'''

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Import Modules ###

# Import standard modules
import time
import re
import requests
from bs4 import BeautifulSoup

# Import other files
from pokemon import MAX_DEX

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Scraping the Information ###

# Initiates the session globally
session = requests.Session()
# UPDATE SESSIONS HEADERS!!!! ????

def get_soup(url, delay = 0.4, retries = 3):
    """ Fetch a URL politely and return parsed HTML (None if HTML doesn't exist) """

    for attempt in range(retries):
        time.sleep(delay)

        try:
            r = session.get(url, timeout = 30)

        except requests.RequestException:
            if attempt == retries - 1:
                raise
            time.sleep(2 ** attempt)
            continue

        if r.status_code == 404:
            return None
        
        if r.status_code >= 500:
            time.sleep(2 ** attempt)
            continue

        r.raise_for_status()
        return BeautifulSoup(r.text, "html.parser")
    
    raise RuntimeError(f"could not fetch {url}")




SEREBII = "https://www.serebii.net"
# Preference order: a Pokémon's data is read from the first dex that has a page for it.
DEX_PATHS = ["/pokedex-sv/", "/pokedex-swsh/", "/pokedex-legends/", "/pokedex-sm/"]

ENTRY_RE = re.compile(r"^\s*#?(\d{3,4})\s+(\S.*)$")


def fetch_dex_index(dex_path):
    """Map national dex number -> (name, page url) for one Serebii game dex."""
    soup = get_soup(SEREBII + dex_path)
    index = {}
    if soup is None:
        return index
    href_re = re.compile(r"^" + re.escape(dex_path) + r"[^/]+(/|\.shtml)$")
    candidates = [(a["href"], a.get_text(" ", strip=True)) for a in soup.find_all("a", href=True)]
    candidates += [(o["value"], o.get_text(" ", strip=True)) for o in soup.find_all("option", value=True)]
    for href, text in candidates:
        m = ENTRY_RE.match(text)
        if m and href_re.match(href):
            index.setdefault(int(m.group(1)), (m.group(2).strip(), SEREBII + href))
    return index


pokemon_pages = {}
for path in DEX_PATHS:
    for num, entry in fetch_dex_index(path).items():
        if num <= MAX_DEX:
            pokemon_pages.setdefault(num, entry)
for num in range(1, MAX_DEX + 1):
    pokemon_pages.setdefault(num, (None, f"{SEREBII}/pokedex-sm/{num:03d}.shtml"))

print(len(pokemon_pages), "pokémon page urls discovered")
print(list(pokemon_pages.keys())[0])
for i in range(MAX_DEX):
    if len(pokemon_pages[i+1]) < 2:
        print(pokemon_pages[i+1])

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Parse the HTML Files ###


