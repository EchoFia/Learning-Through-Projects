''' 
Scrapes for information on national trust locations at the following sites:
 - https://en.wikipedia.org/wiki/List_of_National_Trust_properties_in_England

Prepares .csv for Notion database
'''

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Import Modules ###

# Import standard modules
import time
import requests
from bs4 import BeautifulSoup
import pandas as pd
import os
import random

from selenium import webdriver

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Scraping the Information ###

# Initiates the session globally
session = requests.Session()

# Obtaining the user-agent
# driver = webdriver.Chrome()
# useragent = driver.execute_script("return navigator.userAgent")
# driver.quit()

# useragent = "Mozilla/5.0 (Tahoe; Mac OS 26_6_2)" # AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.6.1 Safari/605.1.15"

useragent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:156.0) Gecko/20100101 Firefox/156.0"


# from fake_useragent import UserAgent

from getuseragent import UserAgent
useragent = UserAgent("all")
theuseragent = useragent.Random()

def get_browser_headers(useragent):
    return {
        'User-Agent': useragent,
        'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
        'Accept-Language': 'en-US,en;q=0.9',
        'Accept-Encoding': 'gzip, deflate, br',
        'Connection': 'keep-alive',
        'Upgrade-Insecure-Requests': '1',
        'Sec-Fetch-Dest': 'document',
        'Sec-Fetch-Mode': 'navigate',
        'Sec-Fetch-Site': 'none',
        'Cache-Control': 'max-age=0',
    }

# session_headers = {'accept-encoding': 'gzip', 'User-Agent': useragent}


session_headers = {
    'User-Agent': theuseragent,
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
    'Accept-Language': 'en-US,en;q=0.5',
    'Accept-Encoding': 'gzip', #, deflate',
    # 'Connection': 'keep-alive',
}

print(theuseragent)

# session_headers = get_browser_headers(theuseragent)

session.headers.update(session_headers)

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

        if r.status_code == 403:
            print("ERROR 403")
            return None

        if r.status_code == 404:
            return None

        # Error 429: Too Many Requests
        if r.status_code == 429:
            print(f"ERROR 429: Sleeping for {r.headers["Retry-After"]} seconds")
            time.sleep(int(r.headers["Retry-After"]))
        
        if r.status_code >= 500:
            time.sleep(2 ** attempt)
            continue

        time.sleep(random.uniform(0.5, 1.5))  # Random jitter

        r.raise_for_status()
        return BeautifulSoup(r.text, "html.parser")
    
    raise RuntimeError(f"could not fetch {url}")

def get_wiki_desc_and_img(url):
    print(url)

    soup = get_soup(url)

    img_url = None
    desc = None

    if soup == None:
        return None, None

    imgs = soup.find_all("img")
    if imgs != None:
        for img in imgs:
            img_url = img["src"]
            if img_url[:4] == "http":
                break

            img_url = img.get('resource')
            if img_url != None:
                if img_url[:4] == "http":
                    break

    for p in soup.find_all("p")[:3]:
        desc = p.get_text().replace('\n', '')
        if desc != "":
            break

    return desc, img_url

    
# d, i = get_wiki_desc_and_img("https://en.wikipedia.org/wiki/Smithcombe,_Sharpenhoe_and_Sundon_Hills")
# print(d)
# print()
# print(i)

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Parsing the National Trust Wiki ###

ENG_WIKI = "https://en.wikipedia.org/wiki/List_of_National_Trust_properties_in_England"
NI_WIKI = "https://en.wikipedia.org/wiki/List_of_National_Trust_properties_in_Northern_Ireland"
WAL_WIKI = "https://en.wikipedia.org/wiki/List_of_National_Trust_properties_in_Wales"
SCOT_WIKI = ""

def parse_eng_list(soup):
    """  """

    nt_dict = {"Name": [], "County": []}

    # Each place is stored into <li> elements, separated by County into <ul> elements
    for ul in soup.find_all("section"):
        # print('=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=')

        county = ul.get_text().split('\n')[0].replace('[edit]', '')

        if county == '':
            continue

        if county == "See also":
            break

        # print(county)
        # nt_names = []
        for li in ul.find_all("li"):
            # nt_names.append(li.get_text())

            nt_dict["Name"].append(li.get_text())
            nt_dict["County"].append(county)

    nt_dict["Country"] = ["England"] * len(nt_dict["Name"])

    return nt_dict

def parse_wal_list(soup):
    """ """

    nt_dict = {"Name": [], "County": [], "Image": [], "URL": []}

    for sec in soup.find_all("section"):

        county = sec.get_text().split('\n')[0].replace('[edit]', '')

        if county == "External links":
            break

        trs = sec.find_all("tr")
        # print(len(trs))

        for tr in trs:

            header_th = tr.find_all("th")

            if header_th == None or header_th == []:
                continue

            header_th = header_th[0]

            if header_th.get_text() == "Site Name":
                continue

            image_url = tr.find("a", class_ = "mw-file-description")
            if image_url != None:
                nt_dict["Image"].append(image_url["href"])
            else:
                nt_dict["Image"].append(None)

            a = tr.find("a", rel = "mw:WikiLink")
            if a != None:
                # REMOVES THEM IF THE LINK IS FLAGGED AS NOT GOING ANYWHERE
                if a["href"].replace("?action=edit&redlink=1", "") != a["href"]:
                    nt_dict["URL"].append(None)
                else:
                    nt_dict["URL"].append(a["href"])
            else:
                nt_dict["URL"].append(None)


            nt_dict["Name"].append(header_th.get_text())
            nt_dict["County"].append(county)

    nt_dict["Country"] = ["Wales"] * len(nt_dict["Name"])
    nt_dict["Region"] = ["Wales"] * len(nt_dict["Name"])

    return nt_dict

def parse_ni_list(soup):
    """ """
    
    nt_dict = {"Name": [], "County": [], "URL": []}

    table = soup.find("table")

    for tr in table.find_all("tr"):

        texts = tr.get_text().split('\n')[1:]

        if texts[0] != "Name":

            nt_dict["Name"].append(texts[0])
            nt_dict["County"].append(texts[2].replace("County ", ""))

            a = tr.find("a")
            if a != None:
                # REMOVES THEM IF THE LINK IS FLAGGED AS NOT GOING ANYWHERE
                if a["href"].replace("?action=edit&redlink=1", "") != a["href"]:
                    nt_dict["URL"].append(None)
                else:
                    nt_dict["URL"].append(a["href"])
            else:
                nt_dict["URL"].append(None)

    nt_dict["Country"] = ["Northern Ireland"] * len(nt_dict["Name"])
    nt_dict["Region"] = ["Northern Ireland"] * len(nt_dict["Name"])
    
    return nt_dict




### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Formatting for Notion ###

counties = ['Bedfordshire', 'Berkshire', 'Bristol', 'Buckinghamshire', 'Cambridgeshire', 'Cheshire', 'Cornwall', 'County Durham', 'Cumbria', 'Derbyshire', 'Devon', 'Dorset', 'Essex', 'Gloucestershire', 'Greater Manchester', 'Hampshire', 'Herefordshire', 'Hertfordshire', 'Isle of Wight', 'Kent', 'Lancashire', 'Leicestershire', 'Lincolnshire', 'Greater London', 'Merseyside', 'Norfolk', 'Northamptonshire', 'Northumberland', 'Nottinghamshire', 'Oxfordshire', 'Shropshire', 'Somerset', 'Staffordshire', 'Suffolk', 'Surrey', 'East Sussex', 'West Sussex', 'Teesside', 'Tyne and Wear', 'Warwickshire', 'West Midlands', 'Wiltshire', 'Worcestershire', 'East Riding of Yorkshire', 'North Yorkshire', 'South Yorkshire', 'West Yorkshire']

regions = ['South West', 'South East', 'Greater London', 'East of England', 'East Midlands', 'West Midlands', 'Yorkshire and the Humber', 'North West', 'North East']

def sorter(unsorted_list, headers):
    """ Allows for manual sorting of a list into a dictionary based on the headers """
    
    sorted_list = {header: [] for header in headers}

    for i in range(len(headers)):
        print(f"{i} : {headers[i]}")
    print()

    for elem in unsorted_list:

        while True:
            try:
                val = int(input(f"Enter the number for {elem}: "))
                sorted_list[headers[val]].append(elem)
                break

            except ValueError:
                print("Must be int")

            except IndexError:
                print("Must be in range")

    print(sorted_list)

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Main ###

INIT_FOLDER = "init_data/"

def main():

    os.makedirs(INIT_FOLDER, exist_ok = True)

    if not os.path.exists(INIT_FOLDER + "eng.csv"):
        eng_soup = get_soup(ENG_WIKI)
        if eng_soup != None:
            eng_dict = parse_eng_list(eng_soup)
            eng_df = pd.DataFrame(eng_dict)
            eng_df.to_csv(INIT_FOLDER + "eng.csv", index = False)

    if not os.path.exists(INIT_FOLDER + "wal.csv"):
        wal_soup = get_soup(WAL_WIKI)
        if wal_soup != None:
            wal_dict = parse_wal_list(wal_soup)
            wal_df = pd.DataFrame(wal_dict)
            wal_df.to_csv(INIT_FOLDER + "wal.csv", index = False)

    if not os.path.exists(INIT_FOLDER + "ni.csv"):
        ni_soup = get_soup(NI_WIKI)
        if ni_soup != None:
            ni_dict = parse_wal_list(ni_soup)
            ni_df = pd.DataFrame(ni_dict)
            ni_df.to_csv(INIT_FOLDER + "ni.csv", index = False)

if __name__ == "__main__":
    main()

# nt_df = pd.read_csv("initial.csv")
# print(nt_df)

# sorter(counties[:5], regions)



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