''' 
Scrapes for the pokemon information at the following sites:
 - serebii.net (pokemon details)
 - https://pokemondb.net/pokedex/all
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
import pandas as pd
import numpy as np

# Import other files
from pokemon import MAX_DEX

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Scraping the Information ###

POKEMONDB = "https://pokemondb.net/pokedex/"

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

def find_pokemon_page(name, has_alt_form):

    global POKEMONDB

    # If the name has extra details (i.e. Basculin blue-striped), need to extract the pokemon's name
    # Some pokemon have 2 names (the paradox pokemon), but all others have one, and paradox pokemon have no alt. forms
    # Can safely splice the name of the pokemon and take the first word if has_alt_form is true

    if has_alt_form:
        name = name.split(' ')

        if name[0] in ["Mega", "Alolan", "Galarian", "Hisuian", "Paldean"]: # GMax????
            name = name[1]
        else:
            name = name[0]

    return POKEMONDB + name        

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Parsing the Pokemon DB ###

def parse_pokedex(soup):
    """ Parses https://pokemondb.net/pokedex/all to get most information of each pokemon and alternate forms """

    # Fetch global variables
    global mon_headers

    # Initiate the DataFrame
    mons_df = pd.DataFrame(columns = mon_headers)

    # Each pokemon's row in the table is stored in a <tr>
    index = 0
    for mon in soup.find_all("tr"):

        # Each detail for the pokemon is stored in a <td>
        tds = mon.find_all("td")

        # If the <tr> block has other than 10 <td> sections, it's not a pokemon (there's only one extra at the start)
        if len(tds) != 10:
            continue

        # Find all the text from the table's row
        texts = [td.get_text() for td in tds]

        # Format the 'types'
        types = texts[2].split(' ')
        if len(types) == 1: # If mono-type
            types.append(np.nan)

        # Check for Mega
        is_mega = False
        if "Mega " in texts[1]: # Space after Mega is important to not accidentally get Yanmega or Meganium
            texts[1] = ' '.join(texts[1].split(' ')[1:])
            is_mega = True

        # Check for GMax (Currently no GMax)
        is_gmax = False

        # Check for Regional Forms
        forms = ["Alolan ", "Galarian ", "Hisuian ", "Paldean "]
        is_forms = [False, False, False, False]
        for i in range(len(forms)):
            if forms[i] in texts[1]:
                texts[1] = ' '.join(texts[1].split(' ')[1:])
                is_forms[i] = True

        # Check for alternate forms (including all the previous ones)
        is_alt_form = False
        has_alt_form = False
        if index != 0:
            if int(texts[0]) == mons_df.loc[index - 1, "nat_index"]:
                is_alt_form = True
                has_alt_form = True
                mons_df.loc[index - 1, "has_alt_form"] = True

        mons_df.loc[index, "nat_index"] = int(texts[0]) # Naturally removes the '\n' in its string
        mons_df.loc[index, "name"] = texts[1]
        mons_df.loc[index, "type1"] = types[0]
        mons_df.loc[index, "type2"] = types[1]

        stats = ["hp", "attack", "defense", "sp_attack", "sp_defense", "speed"]
        for i in range(6):
            mons_df.loc[index, stats[i]] = int(texts[i + 4])

        mons_df.loc[index, "is_mega"] = is_mega
        mons_df.loc[index, "is_gmax"] = is_gmax
        mons_df.loc[index, "is_alolan"] = is_forms[0]
        mons_df.loc[index, "is_galarian"] = is_forms[1]
        mons_df.loc[index, "is_hisuian"] = is_forms[2]
        mons_df.loc[index, "is_paldean"] = is_forms[3]
        mons_df.loc[index, "is_alt_form"] = is_alt_form
        mons_df.loc[index, "has_alt_form"] = has_alt_form

        index += 1

    return mons_df



def get_height(soup, id_ = None):

    if id_ == None:
        divs = soup.find_all("div", class_ = "sv-tabs-panel") # sv for Gen IX
    else:
        divs = soup.find_all("div", class_ = "sv-tabs-panel", id_ = id_)

    for div in divs:

        # CHANGE TO USE TR's

        ths = div.find_all("th")
        tds = div.find_all("td")

        if len(ths) == 0 or len(tds) == 0:
            continue

        for i in range(min(len(ths), len(tds))):
            if ths[i].get_text() == "Height":
                height = float(tds[i].get_text().split('m')[0]) # Bit annoying with the special space, hence splitting at m

                return height


def find_active_id(soup, name):

    for div in soup.find_all("div", class_ = "sv-tabs-tab-list"):
        for a in div.find_all("a"):
            if a.get_text() == name:
                return a['href']


def add_mon_info(df_index):

    global mons_df

    # Can just grab the same page as the previous, maybe????
    url = find_pokemon_page(*mons_df.loc[df_index, ["name", "has_alt_form"]])
    mon_page = get_soup(url)

    if mons_df.loc[df_index, "has_alt_form"]:
        id_ = find_active_id(mon_page, mons_df.loc[df_index, "name"])
        print(id_)


    mons_df.loc[df_index, "height_m"] = get_height(mon_page)

    



mon_headers = [
    "nat_index", 
    "name", 
    "type1", 
    "type2", 
    "hp",
    "attack", 
    "defense", 
    "sp_attack", 
    "sp_defense", 
    "speed", 
    "is_mega", 
    "is_gmax", 
    "is_alolan", 
    "is_galarian", 
    "is_hisuian", 
    "is_paldean", 
    "is_alt_form", 
    "has_alt_form",
    "height_m",
    "weight_kg",
    "norm_abs",
    "hidd_ab"
]

mons_df = pd.DataFrame(columns = mon_headers)
page_all = get_soup(f"{POKEMONDB}all")
mons_df = parse_pokedex(page_all)

# for i in range(len(mons_df)):
for i in range(5):

    if (i % 100) == 0:
        print(i)

    add_mon_info(i)

print(mons_df)