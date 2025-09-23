# -*- coding: utf-8 -*-
# Stufe 3 findet Schlüssel in 
# 
#
import sys
import re

if len(sys.argv) != 2:
    print (sys.argv[0],' fil')
    sys.exit(4)
else:
    fnam=sys.argv[1]
pfad = "C:\\users\\hh\\Documents\\Prolog\\"
print ("process", fnam+'.txt nach ',pfad)

npos=0      # anzahl Spalten = Position
kex=[]      #
prop=[]     #
lins=[]     # 
with open(fnam+'.txt') as fin:
    fout=open(pfad+fnam+'.pl','w')
    for line in fin:
        line = line.strip()
        if line[:1] != '%':
            if line[:1] == '=':  #Trenner
                #print(lins)
                #print (prop)
                #print(kex)
                fout.write("puzz(")
                t=''
                for p in prop: t+=p+','
                t=t[:-1]+') :-\n'
                fout.write(t)
                for t in lins: fout.write(t+'\n')
                
            else:
                lins.append(line)
                t=line.split(' ',1)[0]
                if t !='': prop.append(t)
                match = re.search(r"\[(.*?)\]", line)
                if match:
                    values = [v.strip() for v in match.group(1).split(",")]
                    npos=len(values)
                    kex.extend(values)
                    print(npos,values)
        else:
            fout.write(line+'\n')
            treffer = []
            for wort in kex:
                for m in re.finditer(re.escape(wort), line):
                    treffer.append((m.start(), wort))
            # Nach Position sortieren
            fund = [wort for _, wort in sorted(treffer, key=lambda x: x[0])]
            #print (line)
            #print(fund)
            fund.append('???')
            fund.append('???')
            #print (fund)
            if "first position" in line:
                fout.write(fund[0]+" = 1,\n")
            elif "second position" in line:
                fout.write(fund[0]+" = 2,\n")
            elif "third position" in line:
                fout.write(fund[0]+" = 3,\n")
            elif "fourth position" in line:
                fout.write(fund[0]+" = 4,\n")
            elif "last position" in line:
                fout.write(fund[0]+" = "+str(npos)+",\n")
            elif "is immediately after" in line:
                fout.write("ele("+fund[0]+"),ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" + 1 ,\n")
            elif "one of the ends" in line:
                fout.write("("+fund[0]+"=1;"+fund[0]+"="+str(npos)+"),\n")
            elif "immediately before " in line:
                fout.write("ele("+fund[0]+"),ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" - 1 ,\n")
            elif "somewhere to the left" in line:
                fout.write("ele("+fund[0]+"), ele("+fund[1]+"), "+fund[0]+" < "+fund[1]+" ,\n")
            elif "somewhere to the right" in line:
                fout.write("ele("+fund[0]+"), ele("+fund[1]+"), "+fund[0]+" > "+fund[1]+" ,\n")                
            elif "somewhere between" in line:
                fout.write("in_between("+fund[1]+","+fund[0]+","+fund[2]+") ,\n")
            elif "next to"  in line:
                 fout.write("next_to("+fund[0]+","+fund[1]+") ,\n")
            else :
                fout.write(fund[0]+"  =  "+fund[1]+" ,\n")
            #print(line)
    # 
    anz=len(prop)
    for i in range(anz): 
        t="valid_list("+prop[i]
        if i==anz-1:
            t+=').'
        else:
            t+='),'
        print(t)
        fout.write(t+'\n')
    fout.write('\nmax_ele('+str(npos)+').\n')
    for i in range(1,npos+1):
        fout.write('ele('+str(i)+'). ')
    
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
    \+ member(X, Xs),
    all_distinct(Xs).

in_between(A,B,C) :-
    ele(A),
    ele(B),
    ele(C),
    A < B,
    B < C.

next_to(A,B)  :-
    ele(A),
    ele(B),
    (A is  B + 1 ; A is B - 1).
        
    """
    fout.write(tx)
    fout.close()   
