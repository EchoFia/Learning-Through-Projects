''' Exports the pokedex in a .csv format ready to be imported as a Notion database '''

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Import Modules ###

# Import standard modules
import numpy as np
import pandas as pd
import math

# Import other files
from pokemon import fetch_pokemons

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Functions ###

def list_to_string(arr):
    """ Converts an array to a comma-separated string for Notion's 'Multi-Select' category """
    string = arr[0]
    for i in range(1, len(arr)):

        # If the element is 'nan', then ignore
        if type(arr[i]) == float: 
            continue 

        string += ", " + arr[i]

    return string

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Pandas DataFrame ###

# Retrieve all the information on each pokemon
pokemons = fetch_pokemons()

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

notion_df = pd.DataFrame(columns = notion_headers)

# Add every pokemon to the DataFrame
for i in range(len(pokemons)):
    mon = pokemons[i]

    mon_type = list_to_string([mon.type1, mon.type2])
    mon_abs = list_to_string(mon.comm_abs)
    mon_sprite = f"https://assets.pokemon.com/assets/cms2/img/pokedex/detail/{i+1:03d}.png"

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