# -*- coding: utf-8 -*-
#Erzeugt py aus text für srq
# ! Kommentare
# # 2. How many questions have answer E? ->  ist=sol.count('e') if not check(2,ist):continue
#  % = letzer wert von Zeile, ~ nach #, z,B, Which is the closest question before #5 that has the answer A?
#  ~ nummer der aktuellen frage
# sonst rausschreiben

quenum=0        # aktuelle Frage
ind='        '  # Einrückung 2fach
ind1='    '     # Einrückung 1fach
ques=[]         # Frage
pyth=[]         # erzeugtes py
ques.append("What is the answer to the question")       #1
pyth.append("ist=sol[~]")
ques.append("How many questions have answer")           #2
pyth.append("ist=sol.count('%')")
ques.append("Which is the first question with answer")  #3
pyth.append("ist=sol.count('%')\n"+ind+"if ist > 0: ist = sol.index('%')")
ques.append("Which is the last question with answer")   #4
pyth.append("ist=sol.count('%')\n"+ind+"if ist > 0: ist = len(sol) - 1 - sol[::-1].index('%')")
ques.append("How many letters away is the answer to this question from the answer to question")
pyth.append("ist=letters_away(%,~)")
ques.append("Which is the closest question after")      #6
pyth.append("ist=closest_after('%',~)")
ques.append("Which is the next question that has the same")      #6
pyth.append("ist=closest_after(sol[~],~)")
ques.append("Which is the closest question before")     #7
pyth.append("ist=closest_before('%',~)")
ques.append("Which are the only 2 consecutive questions")      #8
pyth.append("ist=find_gleiche(2)")
ques.append("Which are the only 4 consecutive questions")      #8
pyth.append("ist=find_gleiche(4)")
ques.append("Which is the least common answer")      #9
pyth.append("ist=min_common()")
ques.append("the only question that has the same answer as this") #10
pyth.append("ist=only_same(~)")

import sys

prolog= """\
# -*- coding: utf-8 -*-
import sys
if len(sys.argv) > 1:
    debug=sys.argv[1]
else:
    debug='c'
if len(sys.argv) > 2:
    runs=int(sys.argv[2])
else:
    runs=10000   
print (sys.argv[0],'debug',debug,'for',runs)    

from collections import Counter    

def nextsol():
    global sol
    i=1
    while True:
        if i in fixed: i += 1 
        while True:
            n=inc[sol[i]]
            if n=='_':  #carry
                sol[i]='a'
                i+=1
                break
            else:
                sol[i]=n
                if ans[i][n] != 'X':               
                    return
                        
def check(q,ist):
    exp=ans[q][sol[q]]
    if 'c' in debug: print(q,sol[q],' ',exp,'<>',ist)            
    return exp==ist
    
def letters_away(qa,qb):
    aa=sol[qa]
    ab=sol[qb]
    return abs(ord(aa)-ord(ab))

def indexe(wert, start=0, end=None):
    if end is None: end = len(sol)
    return [i for i, x in enumerate(sol) if x == wert and start <= i < end]

def closest_before(wert,pos):
    ind=indexe(wert,1,pos)
    if ind==[]: return 0
    return ind[len(ind)-1]

def closest_after(wert,pos):
    ind=indexe(wert,pos+1)
    if ind==[]: return 0
    return ind[0] 
 
def find_gleiche(n):
    # findet genau n aufeinander folgenden Frage-Nummern mit gleicher antwort aber nur einmal vorkommend
    gleiche = []
    i = 0
    while i <= len(sol) - n:
        count = 1
        while i + count < len(sol) and sol[i + count] == sol[i]:
            count += 1
        if count == n:
            gleiche.append(i)
        i += count  # überspringe den gerade untersuchten Run
    print('Gleiche',gleiche)
    if len(gleiche)!= 1:
        return('')
    else:
        p=gleiche[0]
        if n == 2:
            return f"{p} and {p+1}"
        else:
            return ", ".join(str(p+i) for i in range(n-1)) + f" and {p+n-1}"

def min_common():
    zaehler = Counter(sol[1:])
    max_count = max(zaehler.values())
    min_count = min(zaehler.values())
    selten = [elem for elem, count in zaehler.items() if count == min_count]
    if len(selten) == 1: return(selten[0])
    else: return (0)

def only_same(qu):
    anz=sol.count(sol[qu])
    if anz != 2: return 0
    ind = [i for i, v in enumerate(sol) if v == sol[qu] and i != qu]
    #print(qu,'onlysame',ind,sol)
    return ind[0]

inc={'_':'a','a':'b','b':'c','c':'d','d':'e','e':'_'}      # dep nans      
fixed=[]   # <- manuell
"""
    
if len(sys.argv) == 1:
    print (sys.argv[0],' fil  [deb] is ')
    sys.exit(4)
else:
    fnam=sys.argv[1]
if len(sys.argv) == 2:
    debug='z'
else:
    debug=sys.argv[2]
    
print ("process", fnam+'.txt nach',fnam+'.py, debug',debug)
retco=0

with open(fnam+'.txt') as fin:
    fout=open(fnam+'.py','w')  
    linno=0
    fout.write(prolog)
    
    for line in fin:
        line = line.strip()
        linno+=1
        if line[0] == '!' : continue
        if line[0] != "#":
            fout.write(line+"\n")    
            continue
        else:
            quenum+=1
            if (quenum==1):
                fout.write("def puzz():\n")
                fout.write(ind1+"for dg in range (runs):\n")
                fout.write(ind+"nextsol()\n")
                fout.write(ind+"if 's' in debug: print (sol)\n\n")
            fout.write(line+"\n")    
            if 'z' in debug: print(line)

            anum=0
            for que in ques:
                #print(quenum,"->",que)
                tx="print('??? Enhance srqeva for question "+str(quenum)+"')"
                pos=line.find(que)
                if pos != -1:
                    rest=line[pos + len(que):]
                    if 'r' in debug: print('rest',rest+'<')
                    i=rest.find('#')  # hier steht ggf Fragenummer die nachher ~ ersetzt
                    qnr=str(quenum)
                    if i>=0:
                        qn=rest[i+1:].split()
                        #print('i bei',i, qn)
                        qnr=qn[0]
                        if qnr[-1]=='?':
                            qnr=qnr[:-1]
                    tx=pyth[anum]
                    #print('qnr',qnr, tx)
                    # ende von rest steht B? oder #10?
                    if len(rest) > 1: 
                        if rest[-2] in "ABCDE":
                            rest=rest[-2].lower()
                            tx=tx.replace('%',rest)
                    #print('vor repl',qnr,tx)
                    tx=tx.replace('~',qnr)
                    print("found  ",tx)
                    break
                anum+=1
            tx=tx.replace('~',str(quenum))
            fout.write(ind+tx+"\n")
            fout.write(ind+"#print('"+str(quenum)+".  ',ist,sol)\n")
            fout.write(ind+'if not check('+str(quenum)+',ist):continue\n')                   
      
    fout.write("\n"+ind+"print(dg,'Gefunden',sol)\n")
    fout.write(ind+"fout=open('result.txt','w')\n")
    fout.write(ind+"for i in range(1,len(sol)): fout.write(sol[i]+' ')\n")
    fout.write(ind+"fout.write('\\n'); fout.close()\n")
    fout.write(ind+"break\n")
    fout.write(ind1+"print(dg,' sol =',sol)\n")
    fout.write("if __name__ == '__main__': puzz()\n")
    fout.close()   
