# -*- coding: utf-8 -*-
# Trägt Häufigkeitswerte von http://www.ids-mannheim.de/fileadmin/kl/derewo/derewo-v-ww-bll-320000g-2012-12-31-1.0.zip
# in worte Tabelle ein
# Aufbau: Rentenbeiträge	Rentenbeitrag	NN	5981
# nimmt nur Grundform [1] mit Länge 5
# da Grundform öfter vorkommt wird Maximum genommen

import sys
from wordledb import ldb

verbo='n'
db=ldb()  
linnum=0            #
hauf={}
indb=[]             # nur worte schon in DB

if len(sys.argv) < 2:
    fnam='ids.txt'
else:
    fnam=sys.argv[1]
print (sys.argv[0],"process", fnam)
 
indb=db.getWortHauf()
with open(fnam, "r", encoding="utf-8") as fin:
    for line in fin:
        linnum+=1
        beg=line.strip().split()
        w=beg[1]
        if len(w) != 5: continue
        if any(ord(ch) > 127 for ch in w):
            #print("Nicht-ASCII-Zeichen", beg) 
            continue
        w=w.upper()
        h=int(beg[3].split('.', 1)[0])
        if w in hauf:
           if h > hauf[w]: hauf[w]=h
        else:
            hauf[w]=h
    #if linnum >20: break

print('Anz Zeilen',linnum)

# Nach Wert absteigend sortieren
sorted_items = sorted(hauf.items(), key=lambda x: x[1], reverse=True)
# In Datei schreiben
net=0
drin=0
with open('ergeb.txt', "w", encoding="utf-8") as fout:
    print('Nicht in DB:')
    for key, value in sorted_items:
        if key in indb:
            fout.write(f"{key}  {value}\n")
            db.setWortHauf(key,value)
            drin+=1
        else: 
            net+=1
            if 'n' in verbo:
                print(key,end=' ')
                if net%10==0: print()
print('\nZeilen',linnum,'in DB',drin,' nicht ',net)            
    
