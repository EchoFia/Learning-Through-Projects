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
import os
from PIL import Image

# Import other files
from pokemon import MAX_DEX

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Scraping the Information ###

POKEMONDB = "https://pokemondb.net/pokedex/"

# Initiates the session globally
session = requests.Session()
# UPDATE SESSIONS HEADERS !!!! ????

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
    """ Takes in the name of the pokemon and returns the url """

    global POKEMONDB ### DO I NEED THIS ???? !!!!

    # For the Mr. Mime evolution line
    name = name.replace('. ', '-').replace('.', '')

    # For Flabébé
    name = name.replace('é', 'e')

    # For Type: Null
    name = name.replace(': ', '-')

    # For the Nidoran line
    name = name.replace("♀", "-f").replace("♂", "-m")

    # For the Farfetch'd line
    name = name.replace('\'', '')

    # Ignoring the previously mentioned pokemon, only paradox pokemon have 2 base names
    # Every other pokemon with 2+ names should be an alternate form of a pokemon
    if has_alt_form:
        name = name.split(' ')

        if name[0] in ["Mega", "Primal", "Alolan", "Galarian", "Hisuian", "Paldean", "Partner"]: # GMax????
            name = name[1]
        else:
            name = name[0]

    # For Mime Jr. & Tapu's & Paradox Mons
    name = name.replace(' ', '-')

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
        if "Mega " in texts[1] or "Primal " in texts[1]: # Space after Mega is important to not accidentally get Yanmega or Meganium
            is_mega = True

        # Check for GMax (Currently no GMax, Eternatus Eternamax is there, but shall ignore)
        is_gmax = False
        if texts[1] == "Eternatus Eternamax":
            continue

        # Check for Regional Forms
        forms = ["Alolan ", "Galarian ", "Hisuian ", "Paldean "]
        is_forms = [False, False, False, False]
        for i in range(len(forms)):
            if forms[i] in texts[1]:
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







def get_value(soup, val, id_ = None):
    """" Scans the pokemon page for specific information """


    if id_ == None:
        divs = soup.find_all("div", class_ = "sv-tabs-panel") # sv for Gen IX
    else:
        divs = soup.find_all("div", class_ = "sv-tabs-panel", id = id_)

    for div in divs:

        # CHANGE TO USE TR's

        ths = div.find_all("th")
        tds = div.find_all("td")

        if len(ths) == 0 or len(tds) == 0:
            continue

        for i in range(min(len(ths), len(tds))):
            if ths[i].get_text() == val:
                return tds[i]


def get_height(soup, id_ = None):
    height = get_value(soup, "Height", id_).get_text()
    height = float(height.split('m')[0])
    return height

def get_weight(soup, id_ = None):
    weight = get_value(soup, "Weight", id_).get_text()
    weight = float(weight.split('kg')[0])
    return weight

def get_abils(soup, id_ = None):
    abils_td = get_value(soup, "Abilities", id_)

    abils = []
    hidd_abil = None

    for a in abils_td.find_all("a"):
        abils.append(a.get_text())

    if len(abils_td.find_all("small")) != 0:
        hidd_abil = abils[-1]
        del abils[-1]

    abils = '|'.join(abils) # Can't have them as a csv cause it's getting saved to a csv

    return abils, hidd_abil

def get_img(soup, df_index, id_ = None):

    if id_ == None:
        div = soup.find("div", class_ = "sv-tabs-panel") # sv for Gen IX
    else:
        div = soup.find("div", class_ = "sv-tabs-panel", id = id_)

    pic = div.find("img")
    if pic == None:
        return None
    return pic["src"]



def find_active_id(soup, name, is_alt_form):
    """  """

    div = soup.find("div", class_ = "sv-tabs-tab-list")

    most_matches = 0
    corr_a = None

    # print(name.split(' '))

    for a in div.find_all("a"):
        # print(a.get_text().split(' '))
        matches = 0

        name_words = name.split(' ')
        for word in name_words:
            # if word + " " in a.get_text() or " " + word in a.get_text():
            if word in a.get_text().split(' '):
                matches += 1

        if matches > most_matches or (matches == most_matches and is_alt_form): # = Just makes it choose the latter, which is the alternate form... explain
            most_matches = matches
            corr_a = a

    if corr_a == None:
        return None
    else:
        return corr_a['href'][1:] # Removes the '#' at the start of the id



def add_mon_info(df_index):
    """  """

    global mons_df

    # Can just grab the same page as the previous, maybe????
    url = find_pokemon_page(*mons_df.loc[df_index, ["name", "has_alt_form"]])
    mon_page = get_soup(url)

    id_ = None
    if mons_df.loc[df_index, "has_alt_form"]:
        id_ = find_active_id(mon_page, *mons_df.loc[df_index, ["name", "is_alt_form"]])
        if id_ == None: 
            print(mons_df.loc[df_index, "name"])
            print("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
        if mons_df.loc[df_index, "is_alt_form"]:
            if int(id_.split('-')[-1]) < 10000:
                print(mons_df.loc[df_index, "name"])
                print("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
                


    mons_df.loc[df_index, "height_m"] = get_height(mon_page, id_)
    mons_df.loc[df_index, "weight_kg"] = get_weight(mon_page, id_)
    mons_df.loc[df_index, ["norm_abils", "hidd_abil"]] = get_abils(mon_page, id_)
    mons_df.loc[df_index, "sprite"] = get_img(mon_page, df_index, id_)

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Tidying up the Naming Convention ###

def pre_clean_up_names(mons_df):

    """
    Problem Pokemon:
     - Megas, Primals, (GMax's), Regional Forms
     - Partner Pikachu & Eevee
     - Mr. Mime, Galarian Mr. Mime, Mr. Rime, Mime Jr.
     - DARMANITANS
     ADD MORE!!!
    """

    for i in range(len(mons_df)):

        name = mons_df.loc[i, "name"]

        # Cleans up Megas & Primals
        if mons_df.loc[i, "is_mega"]:
            mons_df.loc[i, "name"] = ' '.join(mons_df.loc[i, "name"].split(' ')[1:])

        # Cleans up Regional Forms (Except Darmanitan)
        regionals = ["is_alolan", "is_galarian", "is_hisuian", "is_paldean"]
        if any(mons_df.loc[i, regionals]) and "Darmanitan" not in mons_df.loc[i, "name"]:

            if "Mr. Mime " in name:
                mons_df.loc[i, "name"] = ' '.join(mons_df.loc[i, "name"].split(' ')[1:]) # Has to remove 'Mr.' & 'Mime'

            mons_df.loc[i, "name"] = ' '.join(mons_df.loc[i, "name"].split(' ')[1:])

        # Cleans up Partner Pikachu & Eevee, and Ash-Greninja
        if "Partner " in name: # or " Ash-Greninja" in name:
            mons_df.loc[i, "name"] = ' '.join(mons_df.loc[i, "name"].split(' ')[1:])

        # Cleans up Rotom, Kyurem & Necrozma
        if "Rotom " in name or "Kyurem " in name or "Necrozma " in name:
            mons_df.loc[i, "name"] = ' '.join(mons_df.loc[i, "name"].split(' ')[:-1])

        # Cleans up Darmanitan
        mons_df.loc[i, "name"] = mons_df.loc[i, "name"].replace("Darmanitan Galarian ", "Galarian Darmanitan ")

    return mons_df

def post_clean_up_names(mons_df):

    for i in range(len(mons_df)):
        name = mons_df.loc[i, "name"]

        if " Ash-Greninja" in name:
            mons_df.loc[i, "name"] = ' '.join(mons_df.loc[i, "name"].split(' ')[1:])

    return mons_df

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Main ###

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
    "norm_abils",
    "hidd_abil",
    "sprite"
]

mons_df = pd.DataFrame(columns = mon_headers)
page_all = get_soup(f"{POKEMONDB}all")
mons_df = parse_pokedex(page_all)
mons_df = pre_clean_up_names(mons_df)

for i in range(len(mons_df)):
# for i in range(5):
    # print(i)

    if (i % 100) == 0:
        print(i)

    add_mon_info(i)

mons_df = post_clean_up_names(mons_df)

mons_df.to_csv("pokedex.csv", index = False)



# for image in images:

#     img = Image.open(f"images/{image}")

#     img_dims = list(img.size)
#     scale = min((DIMS[i] - 30) / img_dims[i] for i in range(2))
#     img_dims[0] = int(img_dims[0] * scale // 1)
#     img_dims[1] = int(img_dims[1] * scale // 1)

#     new_img = Image.new('RGB', (DIMS[0], DIMS[1]), color = "white")
#     img = img.resize(img_dims) 

#     offset = [int((DIMS[i] - img_dims[i]) // 2 ) for i in range(2)]

#     new_img.paste(img, offset)
#     new_img.save(f"scaled_images/{image}", optimize=True, quality=75)


#### MAKE THE BACKGROUND TRANSPARENT
### POKEMON HAVE BLACK LINES AROUND THEM, ALWAYS, MAKES IT NICE

# os.makedirs("transparent_images", exist_ok = True)
# for image in images:

#     img = Image.open(f"images/{image}")
#     img = img.convert("RGBA")

#     datas = img.getdata()

#     newData = []

#     val = 240
#     for item in datas:
#         if item[0] >= val and item[1] >= val and item[2] >= val:
#             newData.append((val, val, val, 0))
#         else:
#             newData.append(item)

#     img.putdata(newData)
#     img.save(f"transparent_images/{image}", optimize=True, quality=75)





# for i in range(len(mons_df)):
#     # print(i)

#     name = mons_df.loc[i, "name"].replace(' ', '-')

#     url = BULBAPEDIA + f"{i+1:03d}{name}" + ".png"
#     print(url)

#     soup = get_soup(url)

#     if soup == None:
#         print(name)



# mons_df2 = pd.read_csv("new_pokedex2.csv")
# mons_df = clean_up_names(mons_df)
# print(mons_df)

