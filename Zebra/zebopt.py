# -*- coding: utf-8 -*-
# Stufe 4 optimiert reihenfolge entsprechend .opt  erzeugt pl aus text 
# 1 einfache Zuweisung, 
# h direkter Vergleich i is +-,  n next, o Oder,   j is <>,
# b between
# valid list wenn alle einer Gruppe abgearbeitet sind

import sys

debug=''
kcnt={} # Anzahl auftreten 
lastgrp={}  # letztes Auftreten der Gruppe
allgrp={}   # alle Auftreten []
props=[]    # Name der Gruppe
werte={}    # für props
zls=[]

if len(sys.argv) == 1:
    print (sys.argv[0],' fil  [deb] is translate kex o ')
    sys.exit(4)
else:
    fnam=sys.argv[1]+'.opt '
if len(sys.argv) == 2:
    debug=''              # z Zeilen ausgeben
else:
    debug=sys.argv[2]  
print ("process", fnam)

def lese():        
    fnam='result.txt'
    print('Lese',fnam)
    with open(fnam,'r') as f:
        z=0
        for line in f:
            t=line.split()  # ['Name', '[4,1,2,3]'] 1. Wert in Sp 4, 2. in Sp 1 ...
            spas = t[1].strip("[]").split(",")
            print(z,props[z],t[0],spas)
            for w in range (len(spas)):
                s=int(spas[w])
                
 #               print(z,s,w+1,end='   ')
 #           print()
            z+=1
    print('Done.')    

with open(fnam) as fin:
    anzlin=0
    for line in fin:
        line.strip()
        if line[0]=='!': #Gruppen
            lx=line[1:].replace('[',' ').replace(']',' ').replace(',','').replace('=','')
            grups=lx.split()
            props.append(grups[0])
            werte[grups[0]]=grups[1:]
            continue 
        if line[0]=='=':
            startlin=int(line[2:])
            for p in props:
                print('Props',p,werte[p])
            continue
        zl =line.split()  #['6', 'n', 'Comedy.Genre', 'Daniel.Name']
        anzlin+=1
        zl.insert(0,anzlin+startlin) #line number in .txt
        if 'z' in debug: print(zl)
        zls.append(zl)
        for i in range(3,len(zl)):
            grp=zl[i].split('.')
            gr=grp[1]
            lino=anzlin+startlin 
            lastgrp[gr]=lino
            if gr in allgrp:
                allgrp[gr].append(lino)
            else:
                allgrp[gr]=[lino]
            if zl[i] in kcnt:
                kcnt[zl[i]]+=1
            else:
                kcnt[zl[i]]=1
print('\nGruppen in Zeile:')                
for key, value in sorted(lastgrp.items(), key=lambda item: item[1]):
    print(key, value, allgrp[key])
    
by_value = [k for k, v in sorted(kcnt.items(), key=lambda item: item[1])]    
#print(by_value)
lese()

# alle mit nur einem:
seq1=[]
seq2=[]
seq3=[]
for zl in zls:
    if zl[2]=='e':
      seq1.append(int(zl[1])-1)  #list by index  
    if zl[2]=='h':
      seq2.append(int(zl[1])-1)  #
for zl in zls:
    if zl[2]=='i':
      seq2.append(int(zl[1])-1)  #
for zl in zls:
    if zl[2]=='n' or zl[2]=='o':
      seq2.append(int(zl[1])-1)  #
for zl in zls:
    if zl[2]=='j':
      seq2.append(int(zl[1])-1)  #
for zl in zls:
    if zl[2]=='b':
      seq3.append(int(zl[1])-1)  #      
for s in seq1:
    print(zls[s])
for s in seq2:
    print(zls[s])
for s in seq3:
    print(zls[s])
anzseq=len(seq1)+len(seq2)+len(seq3)

print('Anz',anzlin,anzseq)