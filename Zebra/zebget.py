# Stufe 1 holt 5 Rätsel > h.txt
import requests
from bs4 import BeautifulSoup

url = "https://www.zebrapuzzles.com/"
headers = {"User-Agent": "Mozilla/5.0"}

resp = requests.get(url, headers=headers, timeout=15)
resp.raise_for_status()

soup = BeautifulSoup(resp.text, "lxml")
fnam='h.txt'
f=open (fnam,'w') 

# --- Dropboxes ---
dropboxes = soup.find_all("select")
allo = {}
for i, select in enumerate(dropboxes, start=1):
    key =  f"db_{i}"
    options = [opt.get_text(strip=True) for opt in select.find_all("option")]
    allo[key] = options

# n mit len 1, dann m* len mit 
n=0
nx=0
for box, opts in allo.items():
    if len(opts)==1:
        n+=1
        for o in opts:
              #print("Aus", o)
              f.write("Aus "+o+"\n")
              
              
    else:
        if n>0:
            nx=n 
            n=0
        if nx>0:
            nx-=1
            for o in opts:
                #print("Opt", o)
                f.write("Opt "+o+"\n")
            #print("End")
            f.write("End\n")

# --- Alle Clues-Blöcke ---
clues_blocks = soup.find_all("div", class_="clues")

for block_idx, block in enumerate(clues_blocks, start=1):
    clues = [li.get_text(strip=True) for li in block.find_all("li")]
    if clues:
        print(f"\n=== {block_idx}:")
        f.write(f"\n=== {block_idx}:\n")
        for i, clue in enumerate(clues, start=1):
            #print("%",clue)
            f.write("% "+clue+"\n")

f.close()