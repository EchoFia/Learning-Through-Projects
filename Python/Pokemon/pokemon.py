import pandas as pd
import numpy as np

class Pokemon:

    def __init__(self, nat_index, gen, name, type1, type2, height, weight, hp, attack, defense, sp_attack, sp_defense, speed, desc, common_abilities, hidden_ability):
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
        self.is_mega = False # CHANGE



def fetch_pokemons():


    pokemons = []

    df = pd.read_csv("pokemon.csv", sep = "\t", encoding = "utf-16")

    headers = list(df)

    simple_args = ['national_number', 'gen', 'english_name', 'primary_type', 'secondary_type', 'height_m', 'weight_kg', 'hp', 'attack', 'defense', 'sp_attack', 'sp_defense', 'speed', 'description']

    for i in range(1025):

        args = []

        # Add the characteristics that are straight forward to add
        for arg in simple_args:
            args.append(df[arg][i])

        # Figure out the common & hidden abilities
        # Hard-coded for 3 abilities
        common_abilities = [df["abilities_0"][i], df["abilities_1"][i], df["abilities_2"][i]]
        while np.nan in common_abilities:
            common_abilities.remove(np.nan)
        args.append(common_abilities)
        args.append(df['abilities_hidden'][i])

        pokemons.append(Pokemon(*args))

    return pokemons