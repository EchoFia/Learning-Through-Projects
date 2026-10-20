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

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Scraping the Information ###

# Initiates the session globally
session = requests.Session()
session.headers.update({"User-Agent": "Mozilla/5.0"})

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

    nt_dict = {"Name": [], "County": [], "Image": []}

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


            nt_dict["Name"].append(header_th.get_text())
            nt_dict["County"].append(county)

    nt_dict["Country"] = ["Wales"] * len(nt_dict["Name"])

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

page_list = get_soup(WAL_WIKI)
# nt_dict = parse_eng_list(page_list)
nt_dict = parse_wal_list(page_list)
nt_df = pd.DataFrame(nt_dict)
nt_df.to_csv("initial.csv", index = False)



# nt_df = pd.read_csv("initial.csv")
# print(nt_df)

# sorter(counties[:5], regions)



"""
So, what are the things that I want in this.... let's see
I will manually add a "visited" and "date visited" section in Notion and 'visited with'

I want:
 - Name of place
 - Country it's in
 - Region it's in (NI and Wales are just one region, Scotland and England have regions)
 - Place to store images ???? !!!!
 - URL ????
 - Brief Desc ????
 - I think opening times will be too annoying
"""