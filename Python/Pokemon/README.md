Python Project No. 1: Statistics with Pokemon

**Sources**: https://github.com/cristobalmitchell/pokedex/tree/main

# Aims

- Extract information about each pokemon from PokemonDB (pokemon attributes)
- Create a `.csv` file of pokemon and their information to import into Notion as a database
- Demonstrating statistical analyses of pokemon
- Implement a battle calculator
- Build an aid for team building

# Pokemon’s Attributes & Information

- **Identifiers**: Name, National Dex Number, Description, Sprite
- **Stats**: HP, Attack, Defense, Special Attack, Special Defense, Speed
- **Physical** Attributes: Height, Weight
- **Types**: Primary Type, Secondary Type (Blank if Single Type)
- **Abilities**: Common & Hidden
- **Evolution Line**: Previous Evolutions, Future Evolutions, Alternative Evolutions
- **Tags**: Regional Form (Alolan, Galarian, Hisuian, Paldean), Mega Evolution, Mythical, Legendary, Pseudo-Legendary, Gigantamax

## New Concepts

- Using **Regex** to search through strings
- Sending HTTP requests with `request`

## New Libraries

- `bs4` stands for `Beautiful Soup` , a library for extracting information from `HTML` and `XML` files
- `request` allows you to send HTTP requests
- `re` adds functionality for handling regular expressions
- `pillow` or `PIL` to manipulate image files
