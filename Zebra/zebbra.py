# Stufe 1 holt 5 Rätsel > h.txt
# von https://www.brainzilla.com/logic/zebra/christmas-gifts/
import requests
from bs4 import BeautifulSoup
import sys

if len(sys.argv) == 1:
    url = "https://www.brainzilla.com/logic/zebra/christmas-gifts/"
else:
    url=sys.argv[1]

print("getting ",url)
resp = requests.get(url)
resp.raise_for_status()

# Parser: lxml
soup = BeautifulSoup(resp.text, "lxml")

for sel in soup.find_all("select"):
    print("\nDrop")
    for opt in sel.find_all("option"):
        txt = opt.text.strip()
        print(txt,end = ' ')
print("Fertig!")

