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
ans=[ '',
{'a' : 7, 'b' : 10, 'c' : 5, 'd' : 3, 'e' : 4},
{'a' : 12, 'b' : 0, 'c' : 8, 'd' : 10, 'e' : 3},
{'a' : 0, 'b' : 8, 'c' : 5, 'd' : 3, 'e' : 7},
{'a' : 5, 'b' : 10, 'c' : 6, 'd' : 8, 'e' : 7},
{'a' : 'd', 'b' : 'c', 'c' : 'b', 'd' : 'e', 'e' : 'a'},
{'a' : 2, 'b' : 3, 'c' : 4, 'd' : 1, 'e' : 0},
{'a' : 'c', 'b' : 'd', 'c' : 'b', 'd' : 'a', 'e' : 'e'},
{'a' : 12, 'b' : 10, 'c' : 11, 'd' : 9, 'e' : 8},
{'a' : 12, 'b' : 8, 'c' : 0, 'd' : 2, 'e' : 6},
{'a' : 5, 'b' : 1, 'c' : 3, 'd' : 10, 'e' : 7},
{'a' : 1, 'b' : 5, 'c' : 3, 'd' : 9, 'e' : 4},
{'a' : 4, 'b' : 6, 'c' : 8, 'd' : 9, 'e' : 11},
]
sol=['','_','a','a','a','a','a','a','a','a','a','a','a']
def puzz():
    for dg in range (runs):
        nextsol()
        if 's' in debug: print (sol)

# 1. Which is the only question that has the same answer as this one?
        ist=only_same(1)
        #print('1.  ',ist,sol)
        if not check(1,ist):continue
# 2. Which is the next question that has the same answer as this one?
        ist=closest_after(sol[2],2)
        #print('2.  ',ist,sol)
        if not check(2,ist):continue
# 3. Which is the closest question before #10 that has the answer A?
        ist=closest_before('a',10)
        #print('3.  ',ist,sol)
        if not check(3,ist):continue
# 4. Which is the closest question after #4 that has the answer C?
        ist=closest_after('c',4)
        #print('4.  ',ist,sol)
        if not check(4,ist):continue
# 5. Which is the least common answer?
        ist=min_common()
        #print('5.  ',ist,sol)
        if not check(5,ist):continue
# 6. How many questions after #7 have answer D?
        print('??? Enhance srqeva for question 6')
        #print('6.  ',ist,sol)
        if not check(6,ist):continue
# 7. What is the answer to the question #5?
        ist=sol[5]
        #print('7.  ',ist,sol)
        if not check(7,ist):continue
# 8. Which is the last question that has the same answer as this one?
        print('??? Enhance srqeva for question 8')
        #print('8.  ',ist,sol)
        if not check(8,ist):continue
# 9. How many questions have answer E?
        ist=sol.count('e')
        #print('9.  ',ist,sol)
        if not check(9,ist):continue
# 10. How many questions have consonant as the answer?
        print('??? Enhance srqeva for question 10')
        #print('10.  ',ist,sol)
        if not check(10,ist):continue
# 11. Which is the previous question that has the same answer as this one?
        print('??? Enhance srqeva for question 11')
        #print('11.  ',ist,sol)
        if not check(11,ist):continue
# 12. Which is the first question that has the same answer as this one?
        print('??? Enhance srqeva for question 12')
        #print('12.  ',ist,sol)
        if not check(12,ist):continue

        print(dg,'Gefunden',sol)
        fout=open('result.txt','w')
        for i in range(1,len(sol)): fout.write(sol[i]+' ')
        fout.write('\n'); fout.close()
        break
    print(dg,' sol =',sol)
if __name__ == '__main__': puzz()
