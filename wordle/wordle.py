# -*- coding: utf-8 -*-
# Lösungshilfe für
# 1 https://www.spiegel.de/games/wordle-auf-deutsch-kostenlos-online-spielen-a-cbfa309d-a8ad-4d7d-9234-28b09b945834
# 2 https://wrdl.de/
# 3 https://begriffel.tagesspiegel.de/  identisch wrdl
# https://www.wördle.de/
# https://wordle-de.github.io/
# https://wordledeutsch.org/

import sys
import os
import time
import pygetwindow as gw
import traceback

from inp import inp
from wordledb import ldb

from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys

class mini():
    
    def __init__(self):
        self.filename = "worte.txt"
        self.driverloc = r'D:/greed/chromedriver.exe'
        self.arg="user-data-dir=C:\\Users\\hh\\AppData\\Local\\Google\\Chrome\\chess"
        self.base_url = "https://www.wortlisten.com/worter5buchstaben" # oder 
        os.system("title Wordler")
        self.inp=inp()
        self.driver=None
        self.puzurl=""
        self.buref={}   # Reference element zum Auslesen eines Buchstabens aus keyboard der website
        self.buval={}   # Stand jedes Buchstabens auf keyboard
        self.page=1
        self.verbose=False
        self.words=['HALLO']
        self.kann8=False
        self.website(3,False)  #Default
        self.warte = 3  # Wartezeit nach Senden
        try:
            self.db=ldb()  
        except Exception as inst:
            print ("Ohne mysql!! "+str(inst))
            self.db = None
                    
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
    
    def startDriver(self):
        service = Service(executable_path=self.driverloc)
        options = webdriver.ChromeOptions()
        options.add_argument('--ignore-certificate-errors')
        options.add_argument('--ignore-ssl-errors')
        options.add_argument("--window-size=1300x1080")
        options.add_experimental_option('excludeSwitches', ['enable-logging'])
        options.add_argument(self.arg)        

        self.driver = webdriver.Chrome(service=service,options=options)
        self.driver.implicitly_wait(0.5)
        self.driver.set_window_size(700,1080)
        time.sleep(3)   #
        wins = gw.getWindowsWithTitle("Wordler")
        if wins:
            try:
                wins[0].restore()
                wins[0].activate()
            except Exception as inst:
                #print ("Keine Ahnung warum: "+str(inst))
                pass
        
    def website(self,num,zeig=True):
        self.puznum=num
        if num==1:
            self.puzurl="https://www.spiegel.de/games/wordle-auf-deutsch-kostenlos-online-spielen-a-cbfa309d-a8ad-4d7d-9234-28b09b945834"
            self.kann8=True
        elif num==2:
            self.puzurl="https://wrdl.de/"
            self.kann8=False
        elif num==3:
             self.puzurl="https://begriffel.tagesspiegel.de/"
             self.kann8=False
        elif num==4:
            self.puzurl="https://wordledeutsch.org/"
            self.kann8=False
        else:
            print(num ,"ist kein gültiger Wert für website.")
            sys.exit(4)
        if zeig: print(self.puzurl)
                  
    def verbinde(self):
        if self.driver is None:
            self.startDriver()
        self.driver.get(self.puzurl)
    
    def hole(self,w):
        for b in w:
            if b in self.buref:
                cla = self.buref[b].get_attribute("class").split()[0] 
                print(b,cla[4:8])
        
    def sende(self,w):
        actions = ActionChains(self.driver) 
        actions.send_keys(w)
        actions.send_keys(Keys.RETURN)
        actions.perform()
                 
    def getKeybrd(self):  # brauchmernetmehr
        print("Hole Keyboard")
        for r in range(1,4):    #3 Zeilen
            zeile=self.driver.find_element(By.CSS_SELECTOR, f".keyboard-row:nth-child({r})")
            divs = zeile.find_elements(By.TAG_NAME, "div")
            print(r,'Anzahl div',len(divs))
            for d in divs:
                cla = d.get_attribute("class").split()[0] #key-default, key-fail, key-success
                if cla[:4]=='key-':
                    self.buval[d.text]=cla[4:8]
                    self.buref[d.text]=d
                #print(d.text,cla)

    def getFeld(self):
        print("Hole Feld")
        for r in range(1,7):    #6 Zeilen
            zeile=self.driver.find_element(By.CSS_SELECTOR, f".row:nth-child({r})")
            divs = zeile.find_elements(By.TAG_NAME, "div")
            #print(r,'Anzahl div',len(divs))
            p=0
            succnt=0
            for d in divs:
                if d.text!='': #Buchstabe des Feldes
                    cla = d.get_attribute("class").split()[0] #cell-default, cell-fail, cell-near, cell-success, bei cell_focus ist der nächste Eintrag
                    wrt=cla[5:9]
                    if self.verbose: print(r,p,d.text,wrt)
                    if wrt=='fail':
                        self.buche(0,d.text,p)
                    elif wrt=='near':
                        self.buche(1,d.text,p)
                    elif wrt=='succ':
                        self.buche(8,d.text,p)
                        succnt+=1
                        if succnt==5: raise ValueError('Ferdisch.')
                p+=1
                
    def checke(self):
        print('Abgleich:')
        for k in sorted(self.buval):
            w=self.buval[k]
            tx=k+'  '+w+'  '
            if k in self.notin: 
                tx+='N '
            if k in self.drin: 
                tx+='D '
            if k in self.dort:
                tx+='T '
            print(tx)
            
    def spiBtn1(self):
        self.driver.switch_to.frame(1)
        el = self.driver.find_element(By.CSS_SELECTOR, ".hg-row:nth-child(1) > .hg-button:nth-child(1)")
        print(el.Text)

        print("Attributes:")
        for k, v in attrs.items():
            print(f"  {k}: {v}")
         
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
        # verbucht was von buchstabe an pos
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
            print("Was'n das?",was)
            return False
        return True
        
    def auto(self):
        print ("Automatisch")  
        self.starte()
        while not self.inp.kbhit():
            self.getFeld()
            print ("%s  %d  "%(self.words[self.wordp],self.wordp),self.dort,self.drin,self.notin)  
            self.pruf()
            self.sende(self.words[self.wordp])
            self.inp.sleepOrKey(self.warte)

    
    def doit(self):
        numpos=0
        while 1:
            try: 
                l=len(self.words)
                if l <2:
                    print('Hi, g oder r für Wortschatz, dann e zum Aufruf der website',self.puzurl)
                    print('dann x zum Lösen oder manuell k (0 1 8 9) p ')
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
                    self.getKeybrd()                        
                elif tmp=="c":
                    self.checke()                        
                elif tmp=="d":
                    self.skipDrin()              
                elif tmp=="e":
                    self.verbinde() 
                elif tmp=="f":
                    self.doFix() 
                elif tmp=="g":
                    self.words=self.db.getData()  
                    self.starte()
                elif tmp=="h":
                    self.getFeld()   
                elif tmp=="i":
                    self.skipNotin()
                elif tmp=="k":
                    self.sende(self.words[self.wordp])                    
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
                    if self.driver is not None: self.driver.quit()
                    print ("See you")
                    sys.exit(0)
                elif tmp=="r":
                    self.fromfile()                                               
                elif tmp=="s":
                    self.starte()                        
                elif tmp=="v":
                    self.verbose= not self.verbose
                    print ("Verbose",self.verbose)    
                elif tmp=="x":
                    self.verbose= False
                    self.auto()                    
                elif tmp=="+":
                     self.wordp+=1   
                elif tmp=="-":
                    if self.wordp>0: self.wordp-=1   
                elif tmp=="\r":
                    self.pruf()
                    self.sende(self.words[self.wordp])      
                else:
                    print(tmp,"? e=Verbinde, g=Hole von DB, dann (p=Prüfe, k=Sende, h=hole) oder 0=not,1=in,8=richtig,9=nur dort, q=Quit")
            except ValueError as e:
                print(e)
            except Exception as inst:
                print ("main Exception "+str(inst))
                print(traceback.format_exc())
       
                      
if __name__ == "__main__":
    g=mini()
    if len(sys.argv)>1:  #zahl gibt 
        g.website(int(sys.argv[1]))
    g.verbose=False
    g.starte()   
    g.doit()
    
    
#
#
#
#    