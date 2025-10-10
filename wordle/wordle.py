# -*- coding: utf-8 -*-
# Lösungshilfe für
# 1 https://www.spiegel.de/games/wordle-auf-deutsch-kostenlos-online-spielen-a-cbfa309d-a8ad-4d7d-9234-28b09b945834
# 2 https://wrdl.de/
# 3 https://begriffel.tagesspiegel.de/  identisch wrdl
# 4 https://wordledeutsch.org/    hat umlaute (UEBEN, TRUEB) sonst endlos
# 5 https://www.wördle.de/        sehr geringer Wortschatz
# 6 https://wordle-de.github.io/  nur einmal 24 Stunden


import sys
import os
import time
import pygetwindow as gw
import psutil
import traceback
import re

from inp import inp
from wordledb import ldb
from wordsel import wosel
 
class mini():
    
    def __init__(self):
        self.filename = "worte.txt"
        os.system("title Woddler")
        self.verbose=False
        self.words=['HALLO']
        self.worthauf={}
        self.kann9=False
        self.websnum=0
        self.zeilanz=0  # Anzahl Zeilen
        self.warte = 3  # Wartezeit nach Senden in setWebsite geändert 

        self.breakp=999999      # um Grund für wort ausschluss zu finden
        self.zeigse=False       # zeig infos nach jedem step
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
        self.notin=[]                   # buchstaben nicht drin
        self.drin=[]                    # buchstaben  drin
        self.dort=["","","","",""]      # muss an dieser Stelle
        self.netdort=[[],[],[],[],[]]   # nicht an dieser Stelle
        self.muss2=[]                   # mindestens 2 mal, nur bei spiegel (1)
    
    def setWebsite(self,num,zeig):
        self.sele.website(num,zeig)
        self.kann9 = (num==1)
        self.websnum=num
        if num==4: 
            self.warte=2
        else:
            self.warte=3
            
    def checkbd(self):
        print('Abgleich Keyboard:')
        for k in sorted(self.sele.buval):
            w=self.sele.buval[k] # 'unbek' 'Netdr' 'Drinn' 'Treff'
            if w !='unbek': 
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
        print("cnt2 : ",self.muss2)
    
    def skipNotin(self):
        # geht durch Wortliste bis keine verbotenen mehr im wort
        # returnt WordPointerChanged d.h alle Prüfungen nochmal von vorne
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
            if sk: # Bedingung nicht erfüllt, nimm nächsten
                wpc=True
                self.wordp+=1 
                if self.breakp<self.wordp: raise ValueError('Break Notin')
            else:  # Bedingung erfüllt
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
                  if self.breakp<self.wordp: raise ValueError('Break skipDrin')
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
                if self.breakp<self.wordp: raise ValueError('Break skipMuss2')
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
                if self.breakp<self.wordp: raise ValueError('Break skipDort')
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
                if self.breakp<self.wordp: raise ValueError('Break skipNetdort')
            else:
              return wpc         
          
    def pruf(self):
        # erhöht wordp so lange bis alle Bedingungen ok
        # returnt false wenn wp nicht geändert weil Wort nicht drin oder ok obwohl nicht erfolgreich EISIG ESSIG
        wps=self.wordp
        if wps==0: wps=-1  #erster immer erfolgreich da nix bekannt
        while True:
            self.skipDrin()
            if self.skipNotin(): continue
            if self.kann9:
                if self.skipMuss2(): continue
            if self.skipNetdort(): continue
            if self.skipDort(): continue
            return self.wordp!=wps
    
    def prufalle(self):
        n=10
        print (f"next{n}  ptr   pr  d   buhauf  worthauf")
        savewp=self.wordp
        i=10
        try:
            while i>0:
                self.pruf()
                w=self.words[self.wordp]
                inf=self.db.getWortInfo(w)  #prio          dup          buhauf     worthauf
                print(f"{w} {self.wordp:>5}  {inf[1]:>3} {inf[2]:>2}   {inf[3]:>5}  {inf[4]:>9}")
                self.wordp+=1
                i-=1
        except IndexError as inst:
            pass
            #print ("prufalle "+str(inst))
        self.wordp =savewp    
        
    def buche(self,was,b,pos):
        # verbucht was von buchstabe b an pos
        if was==9:
            if not self.kann9: was=8    # nur Spiegel unterscheidet 7, 8 und 9!
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
        elif was == 7:  # richtig, müssen mindestens 2 sein 
            self.dort[pos]=b
            if b not in self.drin:
                self.drin.append(b)
            if b not in self.muss2:
                self.muss2.append(b)
        elif was == 8:          # richtig aber multiple möglich, Problem hier wenn bu zweimal in Wort und erstes macht 0
              self.dort[pos]=b   
              if b not in self.drin:
                self.drin.append(b)
              if b in self.notin:
                  self.notin.remove(b)
        elif was == 9:          # garantiert dort richtig
            #uffbasse: wenn mehrere gleiche gefunden wird auch 9 gezeigt 
            self.dort[pos]=b
            if b not in self.drin:
                self.drin.append(b)
            for i in range(5):   
                if self.dort[i] != b:
                    if b not in self.netdort[i]:
                        self.netdort[i].append(b)
                else:  #ist gleich muss aber ggf aus netdort raus!
                    if b in self.netdort[i]:
                        print("Hoppla",i,b,self.netdort[i])
                        self.netdort[i].remove(b)
        else:
            print("Was'n was?",was)
            return False
        return True
    
    def neuwort(self):
        while True:
            wort = input("Wort eingeben (nur a–z): ")
            if len(wort)==0:
                self.breakp=99999
                return
            if len(wort)!=5:
                print("Nur Länge 5")
            else:
                if re.fullmatch(r"[a-zA-Z]+", wort):
                    break
                print("Nur a–z")
        wort=wort.upper()
        if wort in self.words:
            pos = self.words.index(wort)
            print(wort,'an',pos,'break set')
            self.breakp=pos
            self.wordp=pos
        else:
            self.words.append(wort)
            self.db.insWort(wort)
            print("Danke:", wort)

    def getmyFeld(self):
        buxs=self.sele.getFeld()
        self.zeilanz=int(len(buxs)/5+1)
        if self.kann9:
            self.sele.getKeybrd()
        for bux in buxs: #[8,bu,spalte]
            if self.kann9: #unterscheidet 7,8,9
                if bux[0]==8:
                    self.sele.getKeywert(bux[1])
                    w=self.sele.buval[bux[1]] # 'unbek' 'Netdr' 'Multi' 'Drinn' 'Einzg'
                    if w=='Einzg': bux[0]=9
                    elif w=='Multi': bux[0]=7
            self.buche(bux[0],bux[1],bux[2])           
            
    def auto(self):
        # returns true if ferdisch
        print ("Automatisch")  
        self.starte()
        while not self.inp.kbhit():
            fehler=''
            try:
                self.getmyFeld()
            except ValueError as e:
                fehler=str(e)
                print('auto',fehler)
                if 'Ferdisch' in str(e):
                    if self.kann9:
                        self.db.setWortPrio(self.words[self.wordp],15)
                    self.db.logge(self.words[self.wordp],self.zeilanz,self.websnum)
                    return True
            print ("%s  %d  "%(self.words[self.wordp],self.wordp),self.dort,self.drin,self.notin)  
            if fehler !='' : raise ValueError(fehler)

            if self.pruf():
                self.sele.sende(self.words[self.wordp])
                if self.zeigse:self.prufalle()
                print("Sende",self.words[self.wordp],end='   ')
                self.inp.sleepOrKey(self.warte)
            else:
                self.wordp+=1  #??
                #raise ValueError('Prufen?')
        return False
    
    def dauerfeuer(self):
        while not self.inp.kbhit():
            self.sele.nachFeld4() 
            if self.auto():
                time.sleep(2)
            else: return
            
    def doit(self):
        numpos=0
        try:
            self.db.updDup(100)      # ggf dup rechnen der neu eingefügten worte
            self.db.updBuHauf(100)   #        
            self.words=self.db.getDataBu() 
            self.worthauf=self.db.getAllWortHauf()
        except Exception as inst:
            print ("keine Datenbank."+str(inst))
            print(traceback.format_exc())
           
            
        print('\nHi. f BuHauf, g WortHauf oder r Text für Wortschatz, dann e zum Aufruf der website',self.sele.puzurl)
        print('dann x zum Lösen oder einzeln o oder  h p k oder manuell k (0 1 8 9) p ')
        while 1:
            try: 
                l=len(self.words)
                if self.wordp>=l: 
                    print("Limit ",l)
                    self.wordp=l-1
                w=self.words[self.wordp]
                if w in self.worthauf:
                    print ("%s (%d) %d of %d"%(w,self.worthauf[w],self.wordp,l),self.dort,self.drin,self.notin,"\x1B[0K")  
                else:
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
                    self.checkbd()                        
                elif tmp=="d":
                    self.skipDrin()              
                elif tmp=="e":
                    self.sele.verbinde() 
                elif tmp=="f":
                    self.words=self.db.getDataBu()  
                    self.worthauf=self.db.getAllWortHauf()  # falls geändert
                    self.starte()
                elif tmp=="g":
                    self.words=self.db.getDataWo()  
                    self.worthauf=self.db.getAllWortHauf()  # falls geändert                    
                    self.starte()
                elif tmp=="h":
                    self.getmyFeld()   
                    self.pruf()
                elif tmp=="H":
                    self.getmyFeld()  
                elif tmp=="i":
                    self.prufalle()
                elif tmp=="I":    
                    self.zeigse= not self.zeigse
                    print ("Zeigse",self.zeigse)                    
                elif tmp=="j":
                    self.sele.getFrames()         
                elif tmp=="k":
                    self.sele.sende(self.words[self.wordp])                    
                elif tmp=="L":      
                    print("Lösche ",w)
                    self.words.remove(w) 
                    self.db.delWort(w)
                    self.sele.refresh()
                elif tmp=="m":
                    self.skipMuss2()
                elif tmp=="n":
                    self.skipNetdort()
                elif tmp=="N":
                    self.neuwort()
                elif tmp=="o":
                    self.getmyFeld()                       
                    self.pruf()
                    self.sele.sende(self.words[self.wordp])                    
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
                elif tmp=="u":
                    self.sele.nachFeld4()                      
                elif tmp=="v":
                    self.verbose= not self.verbose
                    self.sele.verbose=self.verbose
                    print ("Verbose",self.verbose)    
                elif tmp=="x":
                    self.verbose= False
                    self.auto()        
                elif tmp=="y":
                    if self.websnum==4:
                        self.dauerfeuer()
                    else:
                        print ('Dauer nur 4')       
                elif tmp=="z":
                    print("Prio", 0)
                    self.words.remove(w) 
                    self.db.setWortPrio(w,0)
                    self.sele.refresh()
                elif tmp=="+":
                     self.wordp+=1   
                elif tmp=="-":
                    if self.wordp>0: self.wordp-=1   
                elif tmp=="#":
                     self.sele.refresh()
#                elif tmp=="\r":
#                    self.pruf()
#                    self.sele.sende(self.words[self.wordp])      
                else:
                    print(tmp,"? e=Verbinde, f=BuHauf g =WortHauf  r=worte.txt, dann (p=Prüfe, k=Sende, h=hole) oder 0=not,1=in,8=richtig,9=nur dort, q=Quit")
                    print("Specials: s=starte o=einen, x=Spiel, u=popup weg y=dauer (nur 4) Debug: a=Stand, b=Keyboard, c=Abgleich i=prufalle v=verbose")
                    print("Wortschatz: Löschen Neues z=Prio 0 Prios 9: Wh 0,  10: Normal,  15: LWort Spiegel")
            except ValueError as e:
                print('main '+str(e))
                if 'Ferdisch' in str(e):
                    if self.kann9:
                        self.db.setWortPrio(self.words[self.wordp],15)
                    self.db.logge(self.words[self.wordp],self.zeilanz,self.websnum)
            except IndexError:                
                print('Am Ende')
            except Exception as inst:
                print ("main Exception "+str(inst))
                print(traceback.format_exc())
       
                      
if __name__ == "__main__":
    g=mini()
    g.verbose=False
    if len(sys.argv)>1:  #zahl gibt 
        g.setWebsite(int(sys.argv[1]),False) 
    g.starte()   
    g.doit()
    
    
#
#
#
#    