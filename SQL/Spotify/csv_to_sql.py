'''
NOTES: Want 3 databases 
 - One for songs
 - One for artists
 - One that links them
'''

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Set Up ###

import pandas as pd
import numpy as np
# from sqlalchemy import create_engine

import mysql.connector

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### SQL Formatting Functions ###

def format_insert(table, df):
    ''' Formats a pandas DataFrame to be inserted into an SQL table '''

    command = ["INSERT INTO " + table + " VALUES"]

    print(len(df))
    for i in range(len(df)):
        command.append("\t(")

        new_line = list(df.loc[i])
        new_line = ['"' + item.replace('"', '\'') + '"' if isinstance(item, str) else str(item) for item in new_line]
        command[-1] += ', '.join(new_line)

        command[-1] += '),'

    command[-1] = command[-1][:-1] + ';' # Replaces the comma ','

    return '\n'.join(command)

def conv_types(type_):
    ''' Formats datatypes to use in creating an SQL table '''

    ori_types = [str, np.int64, np.float64, np.bool]
    sql_types = ["TEXT", "INT", "FLOAT", "TEXT"]

    for i in range(len(ori_types)):
        if isinstance(type_, ori_types[i]):
            return sql_types[i]

    # RAISE AN ERROR OR PRINT SOMETHING
    return str(type(type_))

def create_table(df, table_name, id_name = "index_id", prim_key_index = None):
    ''' Generates the SQL instruction for creating a table based on a pandas DataFrame. NOTE: Doesn't have functionality for 'FOREIGN KEY' '''

    command = ['CREATE TABLE ' + table_name + ' (']

    headers = df.columns
    for i in range(len(headers)):

        # Certain headers might be 'reserved words' in SQL, and must be surrounded by `
        used_words = ['explicit', 'key']
        if headers[i].lower() in used_words:
            command.append('\t`' + headers[i] + '`')
        else:
            command.append('\t' + headers[i])

        command[-1] += ' ' + conv_types(df.loc[0, headers[i]])

    if "Unnamed: 0" in command[1]:
        command[1] = '\t' + id_name + command[1][11:]

    command.append(');')

    if prim_key_index != None:
        # CHECK THE TYPE
        command[prim_key_index + 1] += " PRIMARY KEY"

    for i in range(1, len(command) - 2):
        command[i] += ','

    return '\n'.join(command)


### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Manipulating the Data ###

# Read in the data
df = pd.read_csv("spotify_data.csv")

# Remove tracks with incomplete information
df = df[df["artists"].notnull()]

# Reindex the tracks to remove any gaps
df = df.reset_index(drop = True)

### Create the DataFrame for Songs ###

# Maybe leave it in???
tracks_df = df.drop("artists", axis = 1)
tracks_df = tracks_df.rename(columns = {"track_id": "track_code"})

### Create the DataFrame for Artists ###

# NOTE: Can maybe calculate artist_genre from the mode of that artists songs ???? !!!!

artists = []

# Stored as a semi-colon separated list
for i in range(len(df)):
    artists += df.loc[i, "artists"].split(";")

# Removes all duplicates (want a unique list)
artists = list(dict.fromkeys(artists))

artists_df = pd.DataFrame(artists, columns = ["artist"])
artists_df.index.name = "artist_id"
### CHECK THIS !!!!! ????

### Create the DataFrame to Link Artists & Songs ###

# artist_to_song = {"artist_id": [], "song_id": []}
# for i in range(len(df)):
#     track_artists = df.loc[i, "artists"].split(";")

#     ### DOUBLE CHECK - MIGHT NEED TO FIND TO GENERALISE?
#     song_id = i

#     for artist in track_artists:

#         artist_id = artists_df.loc[artists_df["artist"] == artist]
#         ### ASSERT ERROR FOR IF MORE THAN 1
#         if len(artist_id) > 1:
#             print("ERROR")

#         artist_id = artist_id.index[0]

#         artist_to_song["song_id"].append(song_id)
#         artist_to_song["artist_id"].append(artist_id)

#     num_prints = 10
#     if i % (len(df) // num_prints) == 0:
#         print(f"{i * num_prints // len(df)}% Complete")

# artist_to_song_df = pd.DataFrame(artist_to_song, columns = ["song_id", "artist_id"])
# artist_to_song_df.to_csv("test.csv", index = False)

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Executing SQL Commands ###

username = input("Enter username: ")
password = input("Enter password: ")

cnx = mysql.connector.connect(
    username = username,
    password = password,
    host = 'localhost',
)

cur = cnx.cursor(buffered = True)

commands = [
    "USE spotify_global;",
    # create_table(tracks_df, "songs", "track_id", 0),
    # format_insert("songs", tracks_df)
]

for command in commands:
    print(command.split('\n')[0])
    cur.execute(command)

    # Print results for queries with outputs
    if command[:6] == "SELECT" or command[:4] == "SHOW":
        for r in cur.fetchall():
            print(r)

    # Ensure to commit changes
    # NOTE: Check if this should be more general, INSERT could be later on ???? !!!!
    if command[:6] == "INSERT":
        cnx.commit()

    print()

cur.close()
cnx.close()