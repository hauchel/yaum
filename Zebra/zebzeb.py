# -*- coding: utf-8 -*-
# Interface to Zebra puzzles on
import sys
import os
import pygetwindow as gw
from inp import inp
import time
import random
import traceback
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver import ActionChains
from selenium.webdriver.support.ui import Select
    
class mini():
    
    def __init__(self):
        self.tagtab=['','640yPFXM','FvGZRfdG','IskYgVJs','ueyPebRM','iHKszShF','0zQrzuF9','vJan5gzI','VuZTt8Hy','2MpRpQtw','bfWKokYv','WCpe23GC','Zn8dMPLA','hYpclPe5']
        self.driver=None 
        self.inp=inp()
        self.num=0
        self.texte=[]
        self.links=[]       #für brainzilla
        self.linknum = 0
        self.props=[]
        self.zeilen=[]
        self.verbose=0
        self.baseurl="https://www.zebrapuzzles.com"
        self.url=self.baseurl
        self.diff=''
        self.diffn=''
        self.tag=''
        self.setdiff(0)
        self.set_tag(0)
        self.spanum=1       # s
        self.selcols={}     # für jede Spalte enthält []sels nach getsels
        self.zeinum=1        # z für 
        self.rulOpts=0      # name von optionen falls space: 0 erstes, 1 zweites 2 Verbinden durch -
     
    def startDriver(self):
        service = Service(executable_path=r'D:/greed/chromedriver.exe')
        options = webdriver.ChromeOptions()
        options.add_experimental_option('excludeSwitches', ['enable-logging'])
        options.add_argument(r"--user-data-dir=C:\temp") 
        options.add_argument('--ignore-certificate-errors')
        options.add_argument('--ignore-ssl-errors')
        options.add_experimental_option('excludeSwitches', ['enable-logging'])
        arg="user-data-dir=C:\\Users\\hh\\AppData\\Local\\Google\\Chrome\\chess"
        options.add_argument(arg)        
        self.driver = webdriver.Chrome(service=service,options=options)
        self.driver.implicitly_wait(1)
        self.driver.set_window_size(800,1080)
   
    def starte(self):
        os.system("title Zebra")
        if self.driver is None:
            self.startDriver()
            time.sleep(1)
            console = gw.getWindowsWithTitle("Zebra")[0]
            console.activate()
        print("started")
    
    def linksel(self,num):
        if len(self.links) <= num:
            print('zu viele',num)
            return
        self.url=self.links[num]
        self.linknum=num
        print('url is ',self.url)   
        self.driver.get(self.url)        
    
    def sammle(self,url,anz=20):
        if anz == 0:
            print ('Zeige links')
            i=0
            for l in self.links:
                print(i,l)
                i+=1
            return        
        self.links=[]
        print ('sammle',anz,' von ',url)
        self.driver.get(url)
        time.sleep(1)
        ber=self.driver.find_element(By.CLASS_NAME,"col-lg-8")
        anchors =ber.find_elements(By.TAG_NAME, "a")
        print('gefunden',len(anchors))
        found=0
        for a in anchors:
            href = a.get_attribute("href") #Persian Rugs https://www.brainzilla.com/logic/zebra/persian-rugs/
            teil = href.split("/logic/zebra/")[-1]
            print(teil,'      ',href)
            if teil !='': 
                 print(found,href)
                 self.links.append(href)
                 found+=1
                 if found>anz: return self.links
        return self.links
    
    def machurl(self):
        if self.tag=='':
            self.url=self.baseurl
            return
        self.url=self.baseurl+'/p/'+self.tag+'/'
        if self.diff !='':
            self.url+='#'+self.diff
        print(self.url)
        
    def setdiff(self,dif):
        diftab=['','very-easy','easy','medium','hard','very-hard']
        if dif>=len(diftab): dif=0
        self.diff=diftab[dif]
        self.diffn=str(dif)
        print('diff',self.diffn,self.diff)
        self.machurl()
        
    def set_tag(self,ta):
        if (ta==0): 
            for i in range (len(self.tagtab)):
                print(f"{i:>3}  {self.tagtab[i]} ")
        if ta<len(self.tagtab):
            self.tag=self.tagtab[ta]
        else:
            self.tag=self.tagtab[0]
        self.machurl()
    
    def schreib_tag(self):
        with open('tage.txt','w',encoding="utf-8",errors="ignore") as ftag:
            for tag in self.tagtab:
                ftag.write(tag+'\n')
        print('Tage geschrieben')
    
    def lies_tag(self):
        with open('tage.txt') as ftag:
            self.tagtab=[]
            for line in ftag:
                self.tagtab.append(line.strip())
    

    def info(self):
        print("\nInfo:")
        print('G:',self.tag,' D:',self.diffn,' n#:',self.linknum,' = ',self.url)
        akt=self.driver.current_url
        aki=akt.split('/')
        print(len(aki),aki)
        print('p:',self.props)
        print('c:',self.selcols.keys())
        for t in self.zeilen:
            print(t)
        print('Anzahl t:',len(self.texte))
        #print('=')
        #for t in self.texte:
        #    print(t)
    
    def schreibe(self):        
        fnam='h'
        print('Schreibe',fnam+'.txt')
        with open(fnam+'.txt','w',encoding="utf-8",errors="ignore") as f:
            f.write('! from '+self.driver.current_url+'\n')
            for t in self.zeilen:
                f.write(t+'\n')
            f.write('=\n')
            for t in self.texte:
                f.write(t+'\n')
        print('Done. rufe zebeva ',fnam)
        subprocess.run(["python", "zebeva.py", fnam])
  
    def lese(self,machs=True):        
        fnam='result.txt'
        print(fnam)
        with open(fnam,'r') as f:
            z=0
            for line in f:
                t=line.split()  # ['Name', '[4,1,2,3]'] 1. Wert in Sp 4, 2. in Sp 1 ...
                spas = t[1].strip("[]").split(",")
                print(z,self.props[z],t,spas)
                for w in range (len(spas)):
                    s=int(spas[w])
                    print(z,s,w+1,end='   ')
                    if machs: self.eintrag(z,s,w+1)
                print()
                z+=1
        print('Done.')    
    
    def holeBox(self):
        print('Box ',self.url)
        self.driver.get(self.url)
    
    def propBox(self):
        game = self.driver.find_element(By.ID, "game")
        cols=game.find_elements(By.CLASS_NAME, "column")
        print('cols',len(cols))
        # props
        pros=cols[0].find_elements(By.TAG_NAME, "li")
        print('pros',len(pros))
        self.props=[]
        for p in pros:
            t=p.text.replace(" ", "")
            print(t) 
            self.props.append(t)
            
    def getsels(self):
        game = self.driver.find_element(By.ID, "game")
        cols=game.find_elements(By.CLASS_NAME, "column")
        self.selcols={}
        for spa  in range(1,len(cols)):
            sels=cols[spa].find_elements(By.TAG_NAME, "select")
            print('Spalte',spa,'sels',len(sels))
            self.selcols[spa]=sels
    
    def eintrag(self,zei,spa,wert,detail=False):
        print('Zei',zei,'spa',spa,' =>',wert,'<')
        select_obj = Select(self.selcols[spa][zei])
        if detail:
            for opt in select_obj.options:
                print(f"{opt.get_attribute('value')} | {opt.text}")
        current_text = select_obj.first_selected_option.text
        print("Text:", current_text,end=' ')
        select_obj.select_by_index(wert) 
        new_text = select_obj.first_selected_option.text
        print("New :", new_text)
    
    def evalBox(self,spa=1):
        game = self.driver.find_element(By.ID, "game")
        cols=game.find_elements(By.CLASS_NAME, "column")
        if spa < 1: spa=1
        if spa >len(cols): spa=len(cols)
        #print('Spalte',spa,'cols',len(cols))
        sels=cols[spa].find_elements(By.TAG_NAME, "select")
        print('sels',len(sels))
        self.selcols[spa]=sels
        self.zeilen=[]
        z=0
        for s in sels:
            select_obj = Select(s)
            zeil=' '
            for opt in select_obj.options:
                #print(f"{opt.get_attribute('value')} | {opt.text}")
                tx=opt.text
                t=tx.split()
                if len(t)==1:
                    zeil+=t[0] + ' '
                elif len(t) > 1:        # Verhalten bei space... : wenn Zeile == z verwende  self.rulOpts 
                    print(z,'options',t)
                    if self.zeinum==z:
                        print('Ersetze')
                        if self.rulOpts <2:
                            zeil+=t[self.rulOpts] + ' '
                        elif self.rulOpts==2 :
                            zeil+= t[0]+'-'+t[1] + ' '
                        else:
                            print('Invalid x',self.rulOpts)
                    else:
                        zeil+=t[0] + ' '
      
            current_text = select_obj.first_selected_option.text
            if z < len(self.props):
                zeil=self.props[z]+zeil
                z+=1
            print(zeil)
            self.zeilen.append(zeil)
            
    def holeText(self):
        game = self.driver.find_element(By.ID, "game")
        clue=game.find_element(By.CLASS_NAME, "clues")
        elems = clue.find_elements(By.TAG_NAME, "li")
        self.texte=[]
        for e in elems:
            t = e.text.strip()
            if not t:
                continue
            if  t.endswith(".") :
                print(len(t),t,'<')
                self.texte.append(t)
    
    def onego(self):
        self.holeText()
        self.propBox()
        self.getsels() 
        self.evalBox() 
        self.info()
        print("write if ok")
        
    def doit(self):
        zl=1 
        sp=1
        isnum=False
        while 1:
            try: 
                print (self.num,end=">",flush=True)
                tmp= self.inp.getch()  
                #self.scan8=True    # falls geändert
                #self.scan8=False
                print (tmp)
                if tmp.isnumeric():
                    if isnum:
                        self.num=self.num*10+int(tmp)
                    else:
                        self.num=int(tmp)
                        isnum=True
                    continue
                isnum=False
                if tmp=="a":
                    self.starte()
                elif tmp=="b":
                    self.holeBox()
                elif tmp=="c":
                    self.getsels()                    
                elif tmp=="g": 
                    self.set_tag(self.num)
                elif tmp=="G": 
                    self.lies_tag()
                elif tmp=="d": 
                    self.setdiff(self.num)
                elif tmp=="e":
                    self.evalBox(self.spanum) 
                elif tmp=="i": 
                    self.info()
                elif tmp=="j" or tmp=="J": 
                    self.eintrag(self.zeinum,self.spanum,self.num,tmp=='J')
                elif tmp=="l" or tmp=='L': 
                    self.lese(tmp=='l')
                elif tmp=="n": 
                    self.linksel(self.num)
                elif tmp=="m": 
                    self.sammle("https://www.brainzilla.com/logic/zebra/",self.num)
                elif tmp=="M": 
                    self.sammle("https://www.ahapuzzles.com/logic/zebra/",self.num)
                elif tmp=="o": 
                    self.onego()                                          
                elif tmp=="p": 
                    self.propBox()                      
                elif tmp=="t":
                    self.holeText()
                elif tmp=="T":
                    self.schreib_tag()
                elif tmp=="q":  
                    if self.driver is not None:
                        self.driver.quit()
                    print ("See you")
                    sys.exit(0)    
                elif tmp=="s":
                    self.spanum=self.num   
                    print('Spalte',self.spanum)                    
                elif tmp=="w":
                    self.schreibe()    
                elif tmp=="x":
                    self.rulOpts=self.num                    
                    print ('rulOpts',self.rulOpts)
                elif tmp=="z":
                    self.zeinum=self.num   
                    print('Zeile',self.zeinum)                    
                else:
                    print(tmp,"? n taG,  n Diff,  stArte, holBox, seleCts, Evalbox, Propbox, holText, Info, n Spalte,  Lies, Write, Quit")
                    print("Holen: (nG  nD  Box) oder per Maus, dann O = Text, Prop, selCs, Eval, Info, Write")
                    print("Eintragen: P, C, L oder einzeln nZ nS nJ X opts")
                    print("anzlinks dann Brainzilla: m,n  Alphapuzz M,n")
                    print("Special nZeile nX rulOpts Tag lesen G schreiben ")
                    
            except Exception as inst:
                print ("main Exception "+str(inst))
                print(traceback.format_exc())
            
        

        
                      
if __name__ == "__main__":
    g=mini()
    try:
        print (sys.argv)
        if len(sys.argv)==1:
            g.starte()
            g.doit()
        elif len(sys.argv)==2: 
            g.doit()           
        else:
          g.readFile(sys.argv[1]+'.txt')
    except Exception as inst:
        print ("Haupt Exception "+str(inst))  
        print(traceback.format_exc())
    