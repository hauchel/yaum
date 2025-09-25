# -*- coding: utf-8 -*-
# Stufe 3 erzeugt pl
# auswahl
# =
# text
import sys
import re

if len(sys.argv) == 1:
    print (sys.argv[0],' fil  [deb]')
    sys.exit(4)
else:
    fnam=sys.argv[1]
if len(sys.argv) == 2:
    debug=False
else:
    debug=True
    
pfad = "C:\\users\\hh\\Documents\\Prolog\\"
print ("process", fnam+'.txt nach ',pfad)

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
    fout=open(pfad+fnam+'.pl','w')
    for line in fin:
        line = line.strip()
        if debug: print(line)
        if mod == 0:
            if line[:1] == '=':  #Trenner
                mod = 1
                if debug:
                    print(lins)
                    print (prop)
                    print('Kex=',kex)
                    print('Vnam',vn)
                    print('VPro',vpro)
                t='puzz('
                for p in prop: t+=p+','
                t=t[:-1]+')'
                puznam=t
                print(t+'.')    #call in prolog
                t+=':-\n'
                fout.write(t)
                for t in lins: fout.write(t+'\n')
                
            else:
                values=line.split()
                npn=len(values)-1
                if npos == 0:
                    npos=npn
                else:
                    if (npos != npn): print ("\n--------> Differen npos ",npos,npn,values)
                #print(npos,'Values=',values)
                li=values[0].capitalize()
                pro=li
                prop.append(pro)
                li+=' = ['
                for i in range(1,len(values)):
                    k=values[i].lower()
                    if k in kexse:
                        print("\n--------> Duplicate kex ",k,values)
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
                    vn[k]=vna
                    vpro[vna]=pro
                    li+=vna+', '
                li=li[:-2]+'],'
                lins.append(li)
        else:
            if line != '':
                if line[0] != '%': line='%'+line
            line=line.lower()
            treffer = []
            for wort in kex:
                for m in re.finditer(re.escape(wort), line):
                    treffer.append((m.start(), wort))
            # Nach Position sortieren
            fux = [wort for _, wort in sorted(treffer, key=lambda x: x[0])]
            #print (line)
            fund=[]
            t=' '
            for f in fux:
                fund.append(vn[f])
                t+=f+':'+vn[f]+'('+vpro[vn[f]]+') ';
            fout.write(line+t+'\n')
            fund.append('???')
            fund.append('???')
            #print (fund)
            
            if "first position" in line:
                fout.write(fund[0]+" = 1,\n")
            elif "second position" in line:
                fout.write(fund[0]+" = 2,\n")
            elif "third position" in line or  "position number 3" in line:
                fout.write(fund[0]+" = 3,\n")
            elif "fourth position" in line:
                fout.write(fund[0]+" = 4,\n")
            elif "last position" in line:
                fout.write(fund[0]+" = "+str(npos)+",\n")
            elif "one of the ends" in line:
                fout.write("("+fund[0]+"=1;"+fund[0]+"="+str(npos)+"),\n")                
            elif "is immediately after" in line or "immediately to the right" in line or "exactly to the right" in line:
                fout.write("ele("+fund[0]+"),ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" + 1 ,\n")
            elif "immediately before" in line or "directly to the left" in line or "exactly to the left" in line:
                fout.write("ele("+fund[0]+"),ele("+fund[1]+"), "+fund[0]+" is "+fund[1]+" - 1 ,\n")
            elif "somewhere to the left" in line:
                fout.write("ele("+fund[0]+"), ele("+fund[1]+"), "+fund[0]+" < "+fund[1]+" ,\n")
            elif "somewhere to the right" in line:
                fout.write("ele("+fund[0]+"), ele("+fund[1]+"), "+fund[0]+" > "+fund[1]+" ,\n")                
            elif "somewhere between" in line:
                fout.write("in_between("+fund[1]+","+fund[0]+","+fund[2]+") ,\n")
            elif "next to"  in line:
                fout.write("next_to("+fund[0]+","+fund[1]+") ,\n")
# extensions     
            elif "first" in line:    
                fout.write(fund[0]+" = 1,\n")
            elif "second" in line:
                fout.write(fund[0]+" = 2,\n")
            elif "third" in line:
                fout.write(fund[0]+" = 3,\n")
            elif "fourt" in line:
                fout.write(fund[0]+" = 4,\n")   
            elif "fifth" in line:
                fout.write(fund[0]+" = 5,\n")                   
            else :
                if line!='': fout.write(fund[0]+" = "+fund[1]+" ,\n") #hoffentlich
            #print(line)
    # 
    anz=len(prop)
    for i in range(anz): 
        t="valid_list("+prop[i]+"),\n"
        fout.write(t)
    for i in range(anz):
        t=prop[i]+'         '
        t=t[:12]
        t="format('"+t+"~w~n', ["+prop[i]+"]) ,\n"
        fout.write(t)    
          
    fout.write("write('Done').\n")        
        
    fout.write('\nmax_ele('+str(npos)+').\n')
    for i in range(1,npos+1):
        fout.write('ele('+str(i)+'). ')
#
    fout.write('\n\nstart :-\n')
#   puzz(Shirt,Name,Impersonation,Prize),
    fout.write(puznam+' ,\n')
#   format('Cart = ~w, Husband = ~w, Wife = ~w, Room = ~w,',Shirt,Name,Location,Position),
    fout.write('fail.\n')
	

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
