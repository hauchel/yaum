# -*- coding: utf-8 -*-
# Self referring Quiz Interface to
# https://www.logiquiz.com/p/hYpclPe5/#very-easy
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

"""
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
        self.driver=None 
        self.inp=inp()
        self.num=0
        self.texte=[]
        self.answers=[]
        self.nans=5         #Anzahl Antworten pro Frage, immer 5?
        self.verbose=0
        self.baseurl="https://www.logiquiz.com"
        self.url="https://www.logiquiz.com/p/hYpclPe5/#very-easy"
        self.diff=''
        self.diffn=''
        self.tag=''
        self.setdiff(0)
        self.settag(0)
        self.spanum=1       # s
        self.selcols={}     # für jede Spalte enthält []sels nach getsels
        self.zeinum=1        # z für 
        self.rulOpts=0      # name von optionen falls space: 0 erstes, 1 zweites
     
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
        #self.actionChains = ActionChains(self.driver)
        #self.driver.minimize_window()  
        
    def starte(self):
        tit="SelfRefQuiz"
        os.system("title "+tit)
        if self.driver is None:
            self.startDriver()
            time.sleep(1)
            console = gw.getWindowsWithTitle(tit)[0]
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
        
    def settag(self,ta):
        tagtab=['','vJan5gzI','VuZTt8Hy','hYpclPe5']
        tagtab=['','vJan5gzI','iHKszShF','0zQrzuF9','VuZTt8Hy','2MpRpQtw','bfWKokYv','WCpe23GC','Zn8dMPLA','hYpclPe5']
        if (ta==0): 
            for i in range (len(tagtab)):
                print(f"{i:>3}  {tagtab[i]} ")
        if ta<len(tagtab):
            self.tag=tagtab[ta]
        else:
            self.tag=tagtab[0]
        self.machurl()

    def info(self):
        print("\nInfo:")
        print('G:',self.tag,' D:',self.diffn,' = ',self.url)
        akt=self.driver.current_url
        aki=akt.split('/')
        print(len(aki),aki)
        if len(aki)>4:  print(aki[4])
        print('c:',self.selcols.keys())
 
        print('Anzahl t:',len(self.texte),' a:',len(self.answers))
        
        for t in self.texte:
            print(t)
        for t in self.answers:
            print(t)
    
    def schreibe(self):        
        fnam='H'     #+self.tag[:4]+self.diffn
        fnam=fnam.replace("-", "")
        print(fnam)
        with open(fnam+'.txt','w',encoding="utf-8",errors="ignore") as f:
            f.write('! from '+self.driver.current_url+'\n')
            sol="sol=['','_',"
            f.write("ans=[ '',\n")
            for t in self.answers:
                sol+="'a',"
                f.write(t+'\n')
            f.write("]\n")
            f.write(sol[:-5]+"]\n")
            for t in self.texte:
                f.write(t+'\n')
        print('Done. rufe srqeva ',fnam)
        subprocess.run(["python", "srqeva.py", fnam])
  
    def lese(self,machs=True):        
        fnam='result.txt'
        print(fnam)
        with open(fnam,'r') as f:
            line=f.readline().rstrip("\n")
            tt=line.split()  # c c e a c
            z=1
            for t in tt: 
                s=ord(t)-ord('a')+1
                print(t,z,s)
                if machs: self.eintrag(z,s)
                z+=1
        print('Done.')    
    
    def holeBox(self):
        print('Box ',self.url)
        self.driver.get(self.url)
    
    def propBox(self):
        cols = self.driver.find_elements(By.CLASS_NAME, "question")
        print('questions',len(cols))
        self.texte=[]
        self.answers=[]
        for c in cols:
            tops=c.find_element(By.CLASS_NAME, "top")
            tx=tops.text.strip()
            tx=tx.replace('\n',' ')
            tx = '# '+tx
            print (tx)    # Frage
            self.texte.append(tx)
            bo=c.find_element(By.CLASS_NAME, "bottom")
            flexs=bo.find_elements(By.CLASS_NAME, "d-flex")
            #print ("flexs",len(flexs))
            ans=" {"  #python dict
            for f in flexs:  # Antworten
                ps=f.find_elements(By.CLASS_NAME, "p-1")
                p0=ps[0].text
                p2=ps[2].text
                if p2=='None': p2 = '0'
                ausw=' '+p0+' '+p2
                #self.texte.append(ausw)
                p0="'"+p0[0].lower()+"' : "
                if not p2.isdigit():
                    p2="'"+p2.lower()+"'"
                ans+=p0+p2+", "
            ans=ans[:-2]+"},"
            self.answers.append(ans)
            
    def getsels(self):
        game = self.driver.find_element(By.ID, "game")
        cols=game.find_elements(By.CLASS_NAME, "column")
        self.selcols={}
        for spa  in range(1,len(cols)):
            sels=cols[spa].find_elements(By.TAG_NAME, "select")
            print('Spalte',spa,'sels',len(sels))
            self.selcols[spa]=sels
    
    def eintrag(self,zei,spa,doppelt=True):
        print('Zei',zei,'spa',spa)
        tx=".question:nth-child("+str(zei)+") li:nth-child("+str(spa)+") .icon"
        self.driver.find_element(By.CSS_SELECTOR, tx).click()
        if doppelt:
            self.driver.find_element(By.CSS_SELECTOR, tx).click()
            
    def evalBox(self,spa=1):
        game = self.driver.find_element(By.ID, "game")
        cols=game.find_elements(By.CLASS_NAME, "column")
        if spa < 1: spa=1
        if spa >len(cols): spa=len(cols)
        print('Spalte',spa,'cols',len(cols))
        sels=cols[spa].find_elements(By.TAG_NAME, "select")
        print('sels',len(sels))
        self.selcols[spa]=sels
      
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
                elif len(t) > 1:        # Verhalten bei space... : wenn Zeile == z verwende  self.rulOpts=0 
                    print(z,'options',t)
                    if self.zeinum==z:
                        zeil+=t[self.rulOpts] + ' '
                    else:
                        zeil+=t[0] + ' '
      
            current_text = select_obj.first_selected_option.text
            if z < len(self.props):
                zeil=self.props[z]+zeil
                z+=1
            print(zeil)
                       
    
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
                    self.settag(self.num)
                elif tmp=="d": 
                    self.setdiff(self.num)
                elif tmp=="e":
                    self.evalBox(self.spanum) 
                elif tmp=="i": 
                    self.info()
                elif tmp=="j" or tmp=="J": 
                    self.eintrag(self.zeinum,self.spanum,tmp=='J')
                elif tmp=="l" or tmp=="L": 
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
                    print(tmp,"? n taG,  n Diff,  stArte, holBox, Propbox, Info, n Spalte,  Lies, Write, Quit")
                    print("Holen: (nG  nD  Box) oder per Maus, dann O = Text, Prop, selCs, Eval, Info, Write")
                    print("Eintragen: P, C, L oder einzeln nZ nS nJ X opts")
                    print("anzlinks dann Brainzilla: m,n  Alphapuzz M,n")
                    print("Special nZeile nX rulOpts")
                    
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
    