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

from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys
""" Gedächtnis wie ein Sieb:
find_element(By.ID, ‘id’)
find_element(By.NAME, ‘name’)
find_element(By.XPATH, ‘xpath’)
find_element(By.LINK_TEXT, ‘link_text’)
find_element(By.PARTIAL_LINK_TEXT, ‘partial_link_text’)
find_element(By.TAG_NAME, ‘tag_name’)
find_element(By.CLASS_NAME, ‘class_name’)
find_element(By.CSS_SELECTOR, ‘css_selector’)
x.getAttribute()
x.click()
x.getText()
"""    
class mini():
    
    def __init__(self):
        self.filename = "worte.txt"
        self.driverloc = r'D:/greed/chromedriver.exe'
        self.arg="user-data-dir=C:\\Users\\hh\\AppData\\Local\\Google\\Chrome\\chess"
        self.base_url = "https://www.wortlisten.com/worter5buchstaben" # oder 
        os.system("title Woddler")
        self.warte = 3  # Wartezeit nach Senden
        self.inp=inp()
        self.driver=None
        self.puzurl=""
        self.buref={}   # Reference element zum Auslesen eines Buchstabens aus keyboard der website
        self.buval={}   # Stand jedes Buchstabens auf keyboard
        self.page=1
        self.verbose=False
        self.words=['HALLO']
        self.kann9=False
        self.website(3,False)  #Default
        try:
            self.db=ldb()  
        except Exception as inst:
            print ("Ohne mysql!! "+str(inst))
            self.db = None
                    
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
        #for i in range(3):
        #    cpu_usage = psutil.cpu_percent(interval=1)
        #    print(f"CPU usage: {cpu_usage}%")
            
    def getFeld1(self):
        #Spiegel
        try:
            self.driver.switch_to.frame(0)   
        except:
            #print("Switch Frame Failed")
            pass
        game=self.driver.find_element(By.ID, "game")
        cells=game.find_element(By.CLASS_NAME,"cell-container")
        divs = cells.find_elements(By.TAG_NAME, "div")
        #print('Anzahl div cells',len(divs))
        i=0         # 0 2 4 6 8  10 12
        zeile=1     # 1           2
        spalte=0    # 0 1 2 3 4   0  1
        while i < len(divs):
            el  = divs[i]   # 0 Class: cell first-row first-col has-userinput-value is-not-solved
            #print(f"{i}Tag: {el.tag_name}, Class: {el.get_attribute('class')}, ID: {el.get_attribute('id')}, text {el.text}<")
            if el.text !='':
                clns=el.get_attribute('class').split()
                if self.verbose: print(zeile,i,el.text,end='  ')
                if 'flipping–incorrect' in clns:
                    if self.verbose:  print('incorrect')
                    self.buche(0,el.text,spalte)
                elif 'flipping-success' in clns:
                    if self.verbose: print('success')
                    self.buche(8,el.text,spalte)
                elif 'flipping-almost' in clns:
                    if self.verbose: print('almost')
                    self.buche(1,el.text,spalte)
                else:
                    print('Keine Ahnung:',zeile,i,el.text.clns)
                    raise ValueError('Ferdisch:')
            i+=2
            spalte+=1
            if i % 10 == 0:
                zeile+=1
                spalte=0
  
    def getFeld2(self):
        #wdl
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
                        if succnt==5: raise ValueError('Ferdisch:')
                p+=1
                
    def getFeld4(self):
        # wordledeutsch
        tiles = self.driver.find_elements(By.CSS_SELECTOR, "game-tile")
        print('Anzahl game-tile',len(tiles))
        rows=self.driver.find_elements(By.CLASS_NAME, "row")
        print('Anzahl rows',len(rows))
        for r in rows:
            tiles = r.find_elements(By.CSS_SELECTOR, "game-tile")
            print('Anzahl tiles',len(tiles))
            for tile in tiles:
                letter = tile.get_attribute("letter")
                evaluation = tile.get_attribute("evaluation")
                reveal = tile.get_attribute("reveal")
                print(f"Letter: {letter}, Evaluation: {evaluation}, Reveal: {reveal}")
                
                
    def getFeld(self):
        print("Hole Feld ",self.puznum)
        if self.puznum==1:
            self.getFeld1()
        elif self.puznum==2:            
            self.getFeld2()
        elif self.puznum==3:            
            self.getFeld2()
        else:
            print("Hole geht nicht für ",self.puznum)

    def getKeybrd1(self): 
        # spiegel
        try:
            self.driver.switch_to.frame(0)   
        except:
            #print("Switch Frame Failed")
            pass
        game=self.driver.find_element(By.ID, "game")
        zeilen=game.find_elements(By.CLASS_NAME,"hg-row")
        #print('Anzahl zeilen',len(zeilen))
        r=1
        for zeile in zeilen:    #3 Zeilen
            #print("Zeile",r)
            divs = zeile.find_elements(By.TAG_NAME, "div")
            #print(r,'Anzahl div',len(divs))
            r+=1
            for d in divs:
                cla = d.get_attribute("class")
                bu = d.get_attribute("data-skbtn")
                if self.verbose: print(bu,cla)
                if len(bu)==1:
                    self.buref[bu]=d
                    clas=cla.split()  #hg-button hg-standardBtn status-1
                    clas.append('unbek')
                    if clas[2]=='status-1':
                        self.buval[bu]='Netdrin'
                    elif clas[2]=='status-2':
                        self.buval[bu]='Drin   '
                    elif clas[2]=='status-3':
                        self.buval[bu]='Treffer'
                    else:
                        self.buval[bu]='unbek  '
                

    def getKeybrd2(self):  # brauchmernetmehrdanichtssagend
        for r in range(1,4):    #3 Zeilen
            zeile=self.driver.find_element(By.CSS_SELECTOR, f".hg-row:nth-child({r})")
            divs = zeile.find_elements(By.TAG_NAME, "div")
            print(r,'Anzahl div',len(divs))
            for d in divs:
                cla = d.get_attribute("class").split()[0] #key-default, key-fail, key-success
                if cla[:4]=='key-':
                    self.buval[d.text]=cla[4:8]
                    self.buref[d.text]=d
                #print(d.text,cla)
    
    def getKeybrd(self):
        print("Hole Keyboard ",self.puznum)
        if self.puznum==1:
            self.getKeybrd1()
        elif self.puznum==2:            
            self.getKeybrd2()
        elif self.puznum==3:            
            self.getKeybrd2()
        else:
            print("geht nicht für ",self.puznum)

                
    def website(self,num,zeig=True):
        self.puznum=num
        if num==1:
            self.puzurl="https://www.spiegel.de/games/wordle-auf-deutsch-kostenlos-online-spielen-a-cbfa309d-a8ad-4d7d-9234-28b09b945834"
            self.kann9=True
        elif num==2:
            self.puzurl="https://wrdl.de/"
            self.kann9=False
        elif num==3:
             self.puzurl="https://begriffel.tagesspiegel.de/"
             self.kann9=False
        elif num==4:
            self.puzurl="https://wordledeutsch.org/"
            self.kann9=False
        elif num==5:
            self.puzurl=" https://www.wördle.de/"
            self.kann9=False
        elif num==6:
            self.puzurl="https://wordle-de.github.io/"
            self.kann9=False
        else:
            print(num ,"ist kein gültiger Wert für website.")
            sys.exit(4)
        if zeig: print(self.puzurl)
                  
    def verbinde(self):
        if self.driver is None:
            self.startDriver()
            time.sleep(1)
            try:
                console = gw.getWindowsWithTitle("Woddler")[0]
                console.activate()
            except Exception as inst:
                #print ("Keine Ahnung warum: "+str(inst))
                pass

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
        
    def analy(self,eles):
        print(f"analy {len(eles)} elements")
        try:
            i=0
            for el in eles:
                print(f"{i}Tag: {el.tag_name}, Class: {el.get_attribute('class')}, ID: {el.get_attribute('id')}")
                children = el.find_elements(By.XPATH, "./*")
                for child in children:
                    print(f"children Tag: {child.tag_name}, Class: {child.get_attribute('class')}, ID: {child.get_attribute('id')}")
                
        except Exception as inst:
            print ("analy Exception "+str(inst))

    def get_all_attributes(self, element):
        attributes = self.driver.execute_script(
        """
        var el = arguments[0];
        var attrs = {};
        for (var i = 0; i < el.attributes.length; i++) {
            attrs[el.attributes[i].name] = el.attributes[i].value;
        }
        return attrs;
        """,
        element
    )
        print (attributes)
        return attributes
    
    def getFrames(self):
        iframes = self.driver.find_elements(By.TAG_NAME, "iframe")
        frames = self.driver.find_elements(By.TAG_NAME, "frame")
        print(f"Number of iframes: {len(iframes)}")
        print(f"Number of frames:  {len(frames)}")
        for i in range(len(iframes)):
            print("iframe",i)
            try:
                self.driver.switch_to.frame(i)
                elements = self.driver.find_elements(By.CSS_SELECTOR, "body > *")
                print(f"Found {len(elements)} direct elements inside frame")
                self.analy(elements)
            except Exception as inst:
                print ("Frame",i,"Exception "+str(inst)[:60])
                 
   
    def checke(self):
        print('Abgleich Keyboard:')
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
        
    def auto(self):
        print ("Automatisch")  
        self.starte()
        while not self.inp.kbhit():
            self.getFeld()
            print ("%s  %d  "%(self.words[self.wordp],self.wordp),self.dort,self.drin,self.notin)  
            self.pruf()
            self.sende(self.words[self.wordp])
            print("Sende",self.words[self.wordp],end='   ')
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
                    self.getFrames() 
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
                elif tmp=="#":
                     self.driver.get(self.driver.current_url)    #refresh falls ungültiges Wort
                elif tmp=="\r":
                    self.pruf()
                    self.sende(self.words[self.wordp])      
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
        g.website(int(sys.argv[1]),False)
    g.starte()   
    g.doit()
    
    
#
#
#
#    