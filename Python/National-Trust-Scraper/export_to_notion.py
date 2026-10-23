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

"""
So, what are the things that I want in this.... let's see
I will manually add a "visited" and "date visited" section in Notion and 'visited with'

I want:
 - Name of place
 - County
 - Region it's in (NI and Wales are just one region, Scotland and England have regions)
 - Country
 - URL ???? if there is one (WHat to do if there's 2???)
 - IMG
 - Brief Desc ????
"""

import requests
# from PIL import Image
session = requests.Session()
os.makedirs("images/", exist_ok = True)

useragent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:156.0) Gecko/20100101 Firefox/156.0"

# print(useragent)

session_headers = {
    'User-Agent': useragent,
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
    'Accept-Language': 'en-US,en;q=0.5',
    'Accept-Encoding': 'gzip', #, deflate',
    # 'Connection': 'keep-alive',
}

def scrape_image(filename, url):
    """ Fetches the sprite image and resizes to use in Notion """

    url = url.replace("?utm_source=en.wikipedia.org&utm_campaign=index&utm_content=original", "")
    print(url)
    r = session.get(url, timeout = 30)

    if r.ok:
        with open(filename, "wb") as f:
            f.write(r.content)

        # Scale the image to fit for notion
        # img = Image.open(filename).convert("RGBA")
        
        # img_dims = list(img.size)
        # scale = min((DIMS[i] - 30) / img_dims[i] for i in range(2))
        # img_dims[0] = int(img_dims[0] * scale // 1)
        # img_dims[1] = int(img_dims[1] * scale // 1)
    
        # background = Image.new('RGB', (DIMS[0], DIMS[1]), color = "white")
        # img = img.resize(img_dims) 
    
        # offset = [int((DIMS[i] - img_dims[i]) // 2 ) for i in range(2)]
    
        # background.paste(img, offset, img)
        # background.save(filename, optimize=True, quality=75)

    else:
        print("A")

    # Prevents spamming requests
    time.sleep(0.2)


df = pd.read_csv("notion_nt_list.csv")
scrape_image("images/test.png", df.loc[0, "Image"])
for i in range(len(df)):
    break
    if not pd.isna(df.loc[i, "Description"]):
        # print((df.loc[i, "Description"])[2])
        if (df.loc[i, "Description"])[2] == "°":
            print(df.loc[i, "Name"])
# df.to_csv("notion_nt_list.csv", index = False)

# https://upload.wikimedia.org/wikipedia/commons/e/e1/Trig_point%2C_on_Dunstable_Downs_-_geograph.org.uk_-_1440937.jpg?utm_source=en.wikipedia.org&utm_campaign=index&utm_content=original

# https://en.wikipedia.org/wiki/File:Portstewart_Strand,_Co._Derry_-_Londonderry_-_geograph.org.uk_-_155430.jpg#/media/File:Portstewart_Strand,_Co._Derry_-_Londonderry_-_geograph.org.uk_-_155430.jpg

# https://upload.wikimedia.org/wikipedia/commons/f/f2/Wellbrook_Beetling_Mill%2CCookstown_-_geograph.org.uk_-_1823890.jpg?utm_source=en.wikipedia.org&utm_campaign=index&utm_content=original

# https://en.wikipedia.org/wiki/File:Wellbrook_Beetling_Mill,Cookstown_-_geograph.org.uk_-_1823890.jpg


# https://upload.wikimedia.org/wikipedia/commons/d/d6/Portstewart_Strand%2C_Co._Derry_-_Londonderry_-_geograph.org.uk_-_155430.jpg?utm_source=en.wikipedia.org&utm_campaign=index&utm_content=original