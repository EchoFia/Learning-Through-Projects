''' Exports the pokedex in a .csv format ready to be imported as a Notion database '''

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Import Modules ###

# Import standard modules
import numpy as np
import pandas as pd
import math
import requests
from PIL import Image
import time
import os

# Import other files
from pokemon import new_fetch_pokemons

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Scraping for Information ###

# Initiates the session globally
session = requests.Session()

def scrape_image(filename, url, DIMS):
    """ Fetches the sprite image and resizes to use in Notion """

    r = session.get(url, timeout = 30)

    if r.ok:
        with open(filename, "wb") as f:
            f.write(r.content)

        # Scale the image to fit for notion
        img = Image.open(filename).convert("RGBA")
        
        img_dims = list(img.size)
        scale = min((DIMS[i] - 30) / img_dims[i] for i in range(2))
        img_dims[0] = int(img_dims[0] * scale // 1)
        img_dims[1] = int(img_dims[1] * scale // 1)
    
        background = Image.new('RGB', (DIMS[0], DIMS[1]), color = "white")
        img = img.resize(img_dims) 
    
        offset = [int((DIMS[i] - img_dims[i]) // 2 ) for i in range(2)]
    
        background.paste(img, offset, img)
        background.save(filename, optimize=True, quality=75)

    # Prevents spamming requests
    time.sleep(0.2)

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Pandas DataFrame ###

# Retrieve all the information on each pokemon
# pokemons = fetch_pokemons()
pokemons = new_fetch_pokemons()

notion_headers = [
    "National Dex Number", 
    "Gen", "Name", "Type", 
    "Common Abilities", 
    "Hidden Ability", 
    "HP", 
    "Atk", 
    "Def", 
    "Sp. Atk", 
    "Sp. Def", 
    "Spd", 
    "Height", 
    "Weight", 
    "Description", 
    "Sprite"
]


os.makedirs("images/", exist_ok=True)
mons_df = pd.read_csv("pokedex.csv")
DIMS = [1500, 600]
for i in range(len(mons_df)):
    break
# for i in range(430, 450):
    # print(i)

    if i % 100 == 0:
        print(i)

    image_url = mons_df.loc[i, "sprite"]

    # CHECK IF is.nan

    scrape_image(f"images/img{i}.png", image_url, DIMS)



notion_df = pd.DataFrame(columns = notion_headers)

# Add every pokemon to the DataFrame
for i in range(len(pokemons)):
    print(i)
    mon = pokemons[i]

    mon_type = str(mon.type1)
    if mon_type[1] != None:
        mon_type += ',' + str(mon.type2)

    mon_abs = None
    print(mon.comm_abs)
    if mon.comm_abs != None:
        mon_abs = ','.join(mon.comm_abs)
    mon_sprite = f"https://raw.githubusercontent.com/EchoFia/Learning-Through-Projects/refs/heads/main/Python/Pokemon/images/img{i}.png"


    pokemon_data = [
        mon.nat_index,
        mon.gen,
        mon.name,
        mon_type,
        mon_abs,
        mon.hidd_ab,
        mon.hp,
        mon.attack,
        mon.defense,
        mon.sp_attack,
        mon.sp_defense,
        mon.speed,
        mon.height,
        mon.weight,
        mon.desc,
        mon_sprite
    ]

    # Append the new 'pokemon_data' to the DataFrame
    notion_df.loc[i] = pokemon_data

notion_df.to_csv("notion_pokedex.csv", index = False)