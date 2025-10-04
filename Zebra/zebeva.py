# -*- coding: utf-8 -*-
# Stufe 3 erzeugt pl aus text 
# mode 0 
#   auswahl
#   '   ' '   '  know: wenn erster dann ersetze zweiter 
#   =
# mode 1
#   text
# ! Kommentare
#

prologpfad = "C:\\users\\hh\\Documents\\Prolog\\"   # \\ da aus win
resultdatei ="D:/zebra/result.txt"        # / im aktuellen dir da aus prolog

import sys
import re

know = {}
debug=False

def check_substrings(lst):
    #print('Prüfe',lst)
    for i, w1 in enumerate(lst):
        for j, w2 in enumerate(lst):
            if i != j and w1 in w2:
                print(f"\n-----> Konflikt?: '{w1}' steckt in '{w2}'")

def add_know (line):
    global know
    teile = re.findall(r"'(.*?)'", line)
    if len(teile) == 2:
        know [teile[0].lower()]= teile[1].lower()
    if 't' in debug: print('Know',teile)

def check_know (line):
    global know
    linneu=line
    for alt, neu in know.items():
        #print(alt,neu)
        linneu = linneu.replace(alt, neu)
    if line != linneu:
        if 't' in debug: print('\nneu:',linneu, '\nalt:',line)
    return linneu

    
    
        
if len(sys.argv) == 1:
    print (sys.argv[0],' fil  [deb] is translate kex o ')
    sys.exit(4)
else:
    fnam=sys.argv[1]
if len(sys.argv) == 2:
    debug=''
else:
    debug=sys.argv[2]
    
print ("process", fnam+'.txt nach ',prologpfad)

npos=0          # anzahl Spalten = Position
kex=[]          # Suchbegriff all lowercase
kexse=set()     # to avoid duplicates
vn={}           # Name des Suchbegriffs (uppercase, 
vpro={}         # propery des Varnam
prop=[]         #
lins=[]         # 
puznam=''       # Aufruf
mod=0

with open(fnam+'.txt') as fin:
    fout=open(prologpfad+fnam+'.pl','w')
    fopt=open(fnam+'.opt','w')
    linno=0
    condno=0
    for line in fin:
        line = line.strip()
        linno+=1
        if line[0] == '!' : continue
        if line[0] == "'" : 
            add_know(line)
            continue
        if mod == 0:
            if line[:1] == '=':  #Trenner
                mod = 1
                if 'Age' in prop:
                    idx = prop.index('Age')*npos # des kleinsten Age
                    add_know("'youngest' 'Age "+kex[idx]+"'")
                    idx+=npos-1;
                    add_know("'oldest' 'Age "+kex[idx]+"'")
                if 'k' in debug:
                    #print(lins)
                    print ('\nProp =',prop)
                    print ('\nPos =',npos)
                    print('\nKnow =',know)
                    print('\nKex =',kex)
                    print('\nVnam =',vn)
                    print('\nVPro =',vpro)
                check_substrings(kex)
                t='puzz('
                for p in prop: t+=p+','
                t=t[:-1]+')'
                puznam=t
                t+=':-\n'
                fout.write(t)
                for t in lins: 
                    fout.write(t+'\n')
                    fopt.write('! '+t+'\n')
                fopt.write('= '+str(linno)+'\n')     #to fix line numbers                    
                
            else: #variablen namen
                values=line.split()
                npn=len(values)-1
                if npos == 0:
                    npos=npn
                else:
                    if (npos != npn): print ("\n--------> Differen npos ",npos,npn,values)
                #print(npos,'Values=',values)
                li=values[0].capitalize().replace(".", "")
                pro=li
                prop.append(pro)
                li+=' = ['
                for i in range(1,len(values)):
                    k=values[i].lower()
                    if k in kexse:
                        print("\n--------> Severe, Stop!! Duplicate kex ",pro,k,values)
                    else:
                        kexse.add(k)
                    kex.append(k) 
                    #generate vn for item found
                    if k[0]=='$':
                        vna=li[:2]+k[1:]
                    elif k[0].isdigit():
                        vna=li[:2]+k
                    else:
                        vna=k.capitalize()
                    vna=vna.replace("-", "")
                    vna=vna.replace("%", "")
                    vna=vna.replace(",", "")
                    vna=vna.replace(":", "")
                    vna=vna.replace("'", "")
                    vna=vna.replace('"', "")
                    vn[k]=vna
                    vpro[vna]=pro
                    li+=vna+', '
                li=li[:-2]+'],'
                lins.append(li)
        else: # conditions
            condno +=1
            if line != '':
                if line[0] != '%': line='% '+line
            line=line.lower()
            line=check_know(line)
            treffer = []
            for wort in kex:
                w1=' '+wort  # suchbegriff isoliert
                for m in re.finditer(re.escape(w1), line):
                    treffer.append((m.start(), wort))
            # Nach Position sortieren
            #print('Treffer',treffer)
            fux = [wort for _, wort in sorted(treffer, key=lambda x: x[0])]
            #print (line)
            fund=[]
            t=' '
            txopt=' '
            for f in fux:
                fund.append(vn[f])
                t+=f+':'+vn[f]+'('+vpro[vn[f]]+') ';
                txopt+=' '+vn[f]+'.'+vpro[vn[f]] 
            fout.write('% ' + str(condno) +line[1:] + t +'\n')
            #print('txopt',txopt)
            fund.append('???')
            fund.append('???')
            #print (fund)
            tx=''
            ftyp='X'    
            # e einfache Zuweisung, 
            # o Oder, i is +-,  j is <>, n next, h direkter Vergleich
            # b between
            if "first position" in line:
                tx=tx=fund[0]+" = 1"
                ftyp='e'
            elif "second position" in line:
                tx=fund[0]+" = 2"
                ftyp='e'
            elif "third position" in line or  "position number 3" in line:
                tx=fund[0]+" = 3"
                ftyp='e'
            elif "fourth position" in line:
                tx=fund[0]+" = 4"
                ftyp='e'
            elif "last position" in line:
                tx=fund[0]+" = "+str(npos)+""
                ftyp='e'
            elif "one of the ends" in line:
                tx="("+fund[0]+"=1;"+fund[0]+"="+str(npos)+")"                
                ftyp='o'
            elif "is immediately after" in line or "immediately to the right" in line \
                or "exactly to the right" in line or "immediately follow" in line:                
                #tx="ele("+fund[0]+"),ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" + 1 "
                tx="ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" + 1 "
                ftyp='i'
            elif "immediately before" in line or "directly to the left" in line \
                or "exactly to the left" in line or "immediately to the left" in line:
                #tx="ele("+fund[0]+"),ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" - 1 "
                tx="ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" - 1 "
                ftyp='i'
            elif "to the left" in line:
                tx="ele("+fund[0]+"), ele("+fund[1]+"), "+fund[0]+" < "+fund[1]+" "
                ftyp='j'
            elif "to the right" in line:
                tx="ele("+fund[0]+"), ele("+fund[1]+"), "+fund[0]+" > "+fund[1]+" "                
                ftyp='j'
            elif "somewhere between" in line:
                tx="in_between("+fund[1]+","+fund[0]+","+fund[2]+") "
                ftyp='b'
            elif "next to"  in line or "adjacent to" in line or "beside" in line:
                tx="next_to("+fund[0]+","+fund[1]+") "
                ftyp='n'
# extensions     
            elif "first" in line:    
                tx=fund[0]+" = 1"
                ftyp='e'
            elif "second" in line:
                tx=fund[0]+" = 2"
                ftyp='e'
            elif "third" in line or "middle" in line or "centre" in line:
                tx=fund[0]+" = 3"
                ftyp='e'
            elif "four" in line or "position 4" in line:
                tx=fund[0]+" = 4"   
                ftyp='e'
            elif "fifth" in line:
                tx=fund[0]+" = 5"  
                ftyp='e'                
            elif " between " in line:
                tx="in_between("+fund[1]+","+fund[0]+","+fund[2]+") "     
                ftyp='b'
            elif " after " in line:
                tx="ele("+fund[0]+"),ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" + 1 "
                ftyp='i'
            else :
                if line!='': tx=fund[0]+" = "+fund[1]+" , %? " #hoffentlich
                ftyp='h'
            
            if tx !='':
                fout.write(tx+" ,\n")
                fopt.write(f"{condno} {ftyp} "+txopt+"\n")
                if '???' in tx:
                    print("\n",linno,line)
                    print(tx)
                    
                
    fopt.close()
    anz=len(prop)

    for i in range(anz): 
        t="valid_list("+prop[i]+"),\n"
        fout.write(t)

    fout.write("File = '"+resultdatei+"',\n")
    fout.write("open(File, write, Stream),\n")
    for i in range(anz):
        t=prop[i]+'         '
        t=t[:10]+" ~w~n', ["+prop[i]+"]) ,\n"
        fout.write("format('"+t)    
        fout.write("format(Stream,'"+t)    
    fout.write("close(Stream),\n")      
    fout.write("writeln('Done').\n")        
        
    fout.write('\nmax_ele('+str(npos)+').\n')
    for i in range(1,npos+1):
        fout.write('ele('+str(i)+'). ')
#
    fout.write('\n\nstart :-\n')
    fout.write(puznam+' .\n')
    #fout.write('fail.\n')  nur eine Lösung, aufruf start. oder  time(start).
	

    tx="""																																																						
valid_list(List) :-
    all_in_range(List),
    all_distinct(List).

% Alle Elemente im Bereich 1..
all_in_range([]).
all_in_range([X|Xs]) :-
    max_ele(M), 
    between(1, M, X),
    all_in_range(Xs).

% Alle Elemente verschieden
all_distinct([]).
all_distinct([X|Xs]) :-
    \\+ member(X, Xs),
    all_distinct(Xs).

in_between(A,B,C) :-
    ele(A),
    ele(B),
    A < B,
    ele(C),
    B < C.

next_to(A,B)  :-
    ele(A),
    ele(B),
    (A is  B + 1 ; A is B - 1).
        
    """
    fout.write(tx)
    fout.close()   
