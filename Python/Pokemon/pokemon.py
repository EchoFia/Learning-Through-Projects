import pandas as pd
import numpy as np

# I'm thinking that I will have a class of Pokemon for an actual individual pokemon used for calculations and things, and then a DataBase to store everything in that can easily be read in due to the 

class Pokemon:

    def __init__(self, nat_index, gen, name, type1, type2, height, weight, hp, attack, defense, sp_attack, sp_defense, speed, desc, common_abilities, hidden_ability, is_mega, is_alolan, is_galarian, is_hisuian, is_paldean, is_alt_form):
        self.nat_index = nat_index
        self.gen = gen
        self.name = name
        self.type1 = type1
        self.type2 = type2
        self.height = height
        self.weight = weight
        self.hp = hp
        self.attack = attack
        self.defense = defense
        self.sp_attack = sp_attack
        self.sp_defense = sp_defense
        self.speed = speed
        self.desc = desc
        self.comm_abs = common_abilities
        self.hidd_ab = hidden_ability

        # Tags
        self.is_mega = is_mega
        self.is_alolan = is_alolan
        self.is_galarian = is_galarian
        self.is_hisuian = is_hisuian
        self.is_paldean = is_paldean
        self.is_alt_form = is_alt_form


# def fetch_pokemons():

#     pokemons = []

#     df = pd.read_csv("pokemon.csv", sep = "\t", encoding = "utf-16")

#     headers = list(df)

#     simple_args = ['national_number', 'gen', 'english_name', 'primary_type', 'secondary_type', 'height_m', 'weight_kg', 'hp', 'attack', 'defense', 'sp_attack', 'sp_defense', 'speed', 'description']

#     for i in range(1025):

#         args = []

#         # Add the characteristics that are straight forward to add
#         for arg in simple_args:
#             args.append(df[arg][i])

#         # Figure out the common & hidden abilities
#         # Hard-coded for 3 abilities
#         common_abilities = [df["abilities_0"][i], df["abilities_1"][i], df["abilities_2"][i]]
#         while np.nan in common_abilities:
#             common_abilities.remove(np.nan)
#         args.append(common_abilities)
#         args.append(df['abilities_hidden'][i])

#         pokemons.append(Pokemon(*args))

#     return pokemons



MAX_DEX = 1025

GEN_BOUNDS = [
    (151, "I"), (251, "II"), (386, "III"), (493, "IV"), (649, "V"),
    (721, "VI"), (809, "VII"), (905, "VIII"), (1025, "IX"),
]


def get_gen(number):
    for bound, gen in GEN_BOUNDS:
        if int(number) <= bound:
            return gen
    raise ValueError(f"national dex number {number} beyond known generations")

def new_fetch_pokemons():

    pokemons = []

    df = pd.read_csv("pokedex.csv")

    simple_args = ['nat_index', 'name', 'type1', 'type2', 'height_m', 'weight_kg', 'hp', 'attack', 'defense', 'sp_attack', 'sp_defense', 'speed']

    tags = ['is_mega', 'is_alolan', 'is_galarian', 'is_hisuian', 'is_paldean', 'is_alt_form'] 

    for i in range(len(df)):

        args = []

        # Add the characteristics that are straight forward to add
        for arg in simple_args:
            args.append(df.loc[i, arg])

        if not pd.isna(df.loc[i, "norm_abils"]):
            common_abilities = df.loc[i, "norm_abils"].split("|")
        else:
            common_abilities = None
        args.append(None) # Description
        args.append(common_abilities)
        args.append(df.loc[i, "hidd_abil"])

        for tag in tags:
            args.append(df.loc[i, tag])

        gen = get_gen(args[0])
        args.insert(1, gen)

        pokemons.append(Pokemon(*args))

    return pokemons