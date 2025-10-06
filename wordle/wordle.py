# -*- coding: utf-8 -*-
# Lösungshilfe für
# 1 https://www.spiegel.de/games/wordle-auf-deutsch-kostenlos-online-spielen-a-cbfa309d-a8ad-4d7d-9234-28b09b945834
# 2 https://wrdl.de/
# 3 https://begriffel.tagesspiegel.de/  identisch wrdl
# 4 https://wordledeutsch.org/
# 5 https://www.wördle.de/        sehr geringer Wortschatz
# 6 https://wordle-de.github.io/  nur einmal 24 Stunden


import sys
import os
import time
import pygetwindow as gw
import psutil
import traceback

from inp import inp
from wordledb import ldb
from wordsel import wosel
 
class mini():
    
    def __init__(self):
        self.filename = "worte.txt"
        os.system("title Woddler")
        self.verbose=False
        self.words=['HALLO']
        self.kann9=False
        self.warte = 3  # Wartezeit nach Senden

        self.inp=inp()       
        try:
            self.db=ldb()  
        except Exception as inst:
            print ("Ohne mysql!! "+str(inst))
            self.db = None
        self.sele=wosel()
        self.sele.website(3,False)  #Default            

    def starte(self):
        print("Starte")
        self.wordp=0
        self.zeilen=['']
        self.zeilnum=0
        self.notin=[]                   # buchstaben nicht drin
        self.drin=[]                    # buchstaben  drin
        self.dort=["","","","",""]      # muss an dieser Stelle
        self.netdort=[[],[],[],[],[]]   # nicht an dieser Stelle
        self.muss2=[]                   # mindestens 2 mal, nur bei spiegel
         
    def checke(self):
        print('Abgleich Keyboard:')
        for k in sorted(self.sele.buval):
            w=self.sele.buval[k]
            tx=k+'  '+w+'  '
            if k in self.notin: 
                tx+='N '
            if k in self.drin: 
                tx+='D '
            if k in self.dort:
                tx+='T '
            print(tx)            
         
    def fromfile(self):
        print("Lese von",self.filename)
        with open(self.filename, "r") as f:
            self.words= [line[:5] for line in f]
        self.starte()
            
    def tofile(self,tx):
        #tx=tx.encode('utf-8')
        x=tx.split()
        with open('New'+self.filename, "a") as f:
            for wo in x:
                try:
                    wo.encode("ascii")
                    f.write(wo+"\n")                
                except UnicodeEncodeError:
                    print(wo,end=",")
    
    def showAll(self):
        print("drin : ",self.drin) 
        print("!drin: ",self.notin)
        print("dort : ",self.dort)
        print("!dort: ",self.netdort)
        #print("cnt2",self.muss2)
    
    def skipNotin(self):
        # geht durch Wortliste bis keine verbotenen mehr im wort
        if self.verbose: print("\nskipNotin",end=': ')
        wpc=False
        while True:
            w=self.words[self.wordp]
            if self.verbose: print(w,end=' ')
            sk=False
            for b in self.notin:
                if b in w:
                    sk=True
                    break
            if sk:
                wpc=True
                self.wordp+=1 
            else:
                return wpc               
                
    def skipDrin(self):
        # geht durch Wortliste bis alle die drin sein müssen drin sind
        if self.verbose: print("\nskipDrin",end=': ')
        wpc=False
        while True:
              w=self.words[self.wordp]
              if self.verbose: print(w,end=' ')
              sk=False
              for b in self.drin:
                  if b not in w:
                      sk=True
                      break
              if sk:
                  wpc=True
                  self.wordp+=1 
              else:
                  return wpc
    
    def skipMuss2(self):
        # nur Spiegel
        if self.verbose: print("\nskipMuss2")
        if len(self.muss2)==0: return False
        b=self.muss2[0]
        wpc=False
        while True:
            w=self.words[self.wordp]
            if self.verbose: print(b," ",w)

            cnt=0 
            for i in range(5):
                if w[i]==b:
                    cnt+=1
            if cnt < 2:    
                wpc=True
                self.wordp+=1 
            else:
              return wpc
              
    def skipDort(self):
         # geht durch Wortliste bis alle die an bestimmter stelle sein müssen dort sind
        if self.verbose: print("\nDort   ",end=': ')
        wpc=False
        while True:
            w=self.words[self.wordp]
            if self.verbose: print(w,end=' ')
            sk=False
            for p in range (5):
                if self.dort[p]!="":
                    if self.dort[p]!=w[p]:
                        sk=True
                        break
            if sk:
                wpc=True
                self.wordp+=1 
            else:
                return wpc                     
            
    def skipNetdort(self):
        # geht durch Wortliste bis alle die nicht an bestimmter stelle sein dürfen auch nicht dort sind
        if self.verbose: print("\nNetdort",end=': ')
        wpc=False
        while True:
            w=self.words[self.wordp]
            if self.verbose: print(w)
            sk=False
            for p in range (5):
                if w[p] in self.netdort[p]:
                    if self.verbose: print(p,w[p],self.netdort[p],end=' ')
                    sk=True
                    break
            if sk:
                wpc=True
                self.wordp+=1 
            else:
              return wpc         
          
    def pruf(self):
        while True:
            self.skipDrin()
            if self.skipNotin(): continue
            #if self.skipMuss2(): continue
            if self.skipNetdort(): continue
            if self.skipDort(): continue
            return
    
    def buche(self,was,b,pos):
        # verbucht was von buchstabe b an pos
        if was==9:
            if not self.kann9: was=8    # nur Spiegel unterscheidet 8 und 9!
        if self.verbose: print("Bu",was,'für',b,'an',pos)
        if was == 0: # garantiert nicht drin, Problem mehrfaches Auftreten
            if b not in self.notin:
                if b not in self.drin:
                    if b not in self.dort:
                        self.notin.append(b)
        elif was == 1: # drin aber nicht dort
            if b not in self.drin:
                self.drin.append(b)
            if b not in self.netdort[pos]:
                self.netdort[pos].append(b)
        elif was == 7:  # loesche aus notin
            if b in self.notin:
                self.notin.remove(b)
        elif was == 8:          # richtig aber multiple möglich
              self.dort[pos]=b   
              #if b not in self.muss2:
              #    self.muss2.append(b)
        elif was == 9:          # garantiert nur dort richtig
            self.dort[pos]=b
            for i in range(5):
                if i != pos:
                    if b not in self.netdort[i]:
                        self.netdort[i].append(b)
        else:
            print("Was'n was?",was)
            return False
        return True
    
    def getmyFeld(self):
        buxs=self.sele.getFeld()
        if self.verbose: print(buxs)
        for bux in buxs:
            self.buche(bux[0],bux[1],bux[2])
        
    def auto(self):
        print ("Automatisch")  
        self.starte()
        while not self.inp.kbhit():
            self.getmyFeld()
            print ("%s  %d  "%(self.words[self.wordp],self.wordp),self.dort,self.drin,self.notin)  
            self.pruf()
            self.sele.sende(self.words[self.wordp])
            print("Sende",self.words[self.wordp],end='   ')
            self.inp.sleepOrKey(self.warte)
 
    def doit(self):
        numpos=0
        print('Hi, g oder r für Wortschatz, dann e zum Aufruf der website',self.sele.puzurl)
        print('dann x zum Lösen oder einzeln k h p  oder manuell k (0 1 8 9) p ')
        while 1:
            try: 
                l=len(self.words)
                if self.wordp>=l: 
                    print("Limit ",l)
                    self.wordp=l-1
                w=self.words[self.wordp]
                print ("%s  %d of %d"%(w,self.wordp,l),self.dort,self.drin,self.notin,"\x1B[0K")  
                if numpos>0: print("\x1B["+str(numpos)+"C",end="",flush=True)
                tmp= self.inp.getch()  
                print (tmp,end="",flush=True)
                if tmp.isnumeric():
                    if numpos<5:
                        print("\x1B[A\r",end="") #one line up to begin
                        if self.buche(int(tmp),w[numpos],numpos): numpos+=1
                    continue
                if tmp=="\x08":
                    if numpos>0: numpos-=1
                    continue
                numpos=0 
                print()
                if tmp=="a":
                    self.showAll()
                elif tmp=="b":
                    self.sele.getKeybrd()                        
                elif tmp=="c":
                    self.checke()                        
                elif tmp=="d":
                    self.skipDrin()              
                elif tmp=="e":
                    self.sele.verbinde() 
                elif tmp=="f":
                    self.sele.getFrames() 
                elif tmp=="g":
                    self.words=self.db.getData()  
                    self.starte()
                elif tmp=="h":
                    self.getmyFeld()   
                elif tmp=="i":
                    self.skipNotin()
                elif tmp=="k":
                    self.sele.sende(self.words[self.wordp])                    
                elif tmp=="L":      
                    print("Lösche ",w)
                    self.words.remove(w) 
                    self.db.delwort(w)
                elif tmp=="m":
                    self.skipMuss2()
                elif tmp=="n":
                    self.skipNetdort()
                elif tmp=="p":
                    self.pruf()
                elif tmp=="q":
                    self.sele.quit()
                    print ("See you")
                    sys.exit(0)
                elif tmp=="r":
                    self.fromfile()                                               
                elif tmp=="s":
                    self.starte()                        
                elif tmp=="v":
                    self.verbose= not self.verbose
                    self.sele.verbose=self.verbose
                    print ("Verbose",self.verbose)    
                elif tmp=="x":
                    self.verbose= False
                    self.auto()                    
                elif tmp=="+":
                     self.wordp+=1   
                elif tmp=="-":
                    if self.wordp>0: self.wordp-=1   
                elif tmp=="#":
                     self.sele.refresh()
                elif tmp=="\r":
                    self.pruf()
                    self.sele.sende(self.words[self.wordp])      
                else:
                    print(tmp,"? e=Verbinde, g=Worte von DB, dann (p=Prüfe, k=Sende, h=hole) oder 0=not,1=in,8=richtig,9=nur dort, q=Quit")
                    print("Debug: a=Stand, b=Keyboard, c=Abgleich f=Frames v=verbose")
            except ValueError as e:
                print(e)
            except Exception as inst:
                print ("main Exception "+str(inst))
                print(traceback.format_exc())
       
                      
if __name__ == "__main__":
    g=mini()
    g.verbose=False
    if len(sys.argv)>1:  #zahl gibt 
        g.sele.website(int(sys.argv[1]),False)
    g.words=g.db.getData()  
    g.starte()   
    g.doit()
    
    
#
#
#
#    