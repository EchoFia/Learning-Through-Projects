'''  '''

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Import Modules ###

# Import standard modules
import pandas as pd
import time
import os

from scrape_nt_wiki import INIT_FOLDER

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Pandas DataFrame ###


notion_headers = [
    ""
]

raw_df = pd.read_csv(INIT_FOLDER + "wal.csv")

notion_df = raw_df

notion_df.to_csv("notion_nt_list.csv", index = False)