# -*- coding: utf-8 -*-
# 
import sys
from inp import inp
import time
import random
import traceback

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
        self.props=[]
        self.zeilen=[]
        self.verbose=0
        self.baseurl="https://www.zebrapuzzles.com"
        self.url=self.baseurl
        self.diff=''
        self.diffn=''
        self.tag=''
        self.setdiff(0)
        self.settag(0)
        self.spalte=1
        self.selcols=[]   # für jede Spalte enthält []
     
    def startDriver(self):
        service = Service(executable_path=r'D:/greed/chromedriver.exe')
        options = webdriver.ChromeOptions()
        options.add_experimental_option('excludeSwitches', ['enable-logging'])
        options.add_argument(r"--user-data-dir=C:\temp") 
        options.add_argument('--ignore-certificate-errors')
        options.add_argument('--ignore-ssl-errors')
        options.add_argument("--window-size=1400x1080")
        options.add_experimental_option('excludeSwitches', ['enable-logging'])
        arg="user-data-dir=C:\\Users\\hh\\AppData\\Local\\Google\\Chrome\\chess"
        options.add_argument(arg)        
        self.driver = webdriver.Chrome(service=service,options=options)
        self.driver.implicitly_wait(1)
        self.driver.set_window_size(700,1080)
        self.actionChains = ActionChains(self.driver)
        #self.driver.minimize_window()  
        
    def starte(self):
        if self.driver is None:
            self.startDriver()
        print("started")
        
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
        if dif<len(diftab): dif=0
        self.diff=diftab[dif]
        self.diffn=str(dif)
        self.machurl()
        
    def settag(self,ta):
        tagtab=['','2MpRpQtw','bfWKokYv','WCpe23GC','Zn8dMPLA','hYpclPe5']
        if ta<len(tagtab):
            self.tag=tagtab[ta]
        else:
            self.tag=tagtab[0]
        self.machurl()

    def info(self):
        print(self.url)
        print(self.props)
        for t in self.zeilen:
            print(t)
        print('=')
        for t in self.texte:
            print(t)
    
  
    def schreibe(self):        
        fnam='H'+self.tag[:4]+self.diffn
        fnam=t[-2]
        fnam=fnam.replace("-", "")
        fnam=fnam[:5]+'.txt'
        print(fnam)
        with open(fnam,'w') as f:
            for t in self.zeilen:
                f.write(t+'\n')
            f.write('=\n')
            for t in self.texte:
                f.write(t+'\n')
        print('Done.')
  
    def lese(self):        
        fnam='result.txt'
        print(fnam)
        with open(fnam,'r') as f:
            for line in f:
                print(line)
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
            print(p.text) 
            self.props.append(p.text)
       
    def evalBox(self,spa=1):
        game = self.driver.find_element(By.ID, "game")
        cols=game.find_elements(By.CLASS_NAME, "column")
        if spa < 1: spa=1
        if spa >len(cols): spa=len(cols)
        print('Spalte',spa,'cols',len(cols))
        sels=cols[spa].find_elements(By.TAG_NAME, "select")
        print('sels',len(sels))
        self.zeilen=[]
        i=0
        for s in sels:
            select_obj = Select(s)
            zeil=' '
            for opt in select_obj.options:
                #print(f"{opt.get_attribute('value')} | {opt.text}")
                tx=opt.text
                t=tx.split()
                try:
                    zeil+=t[0] + ' '
                except Exception as inst:
                    pass
                
            current_text = select_obj.first_selected_option.text
            if i< len(self.props):
                zeil=self.props[i]+zeil
                i+=1
            print(zeil)
            self.zeilen.append(zeil)
            print("Text:", current_text)
            #select_obj.select_by_value("2")
            
    def holeText(self):
        self.driver.get(self.url)
        game = self.driver.find_element(By.ID, "game")
        elems = game.find_elements(By.TAG_NAME, "li")
        self.texte=[]
        for e in elems:
            t = e.text.strip()
            if not t:
                continue
            if (len(t) > 20) and (t.endswith(".")):
                print(t)
                self.texte.append(t)
    
    
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
                elif tmp=="g": 
                    self.settag(self.num)
                elif tmp=="d": 
                    self.setdiff(self.num)
                elif tmp=="e":
                    self.evalBox(self.spalte) 
                elif tmp=="i": 
                    self.info()
                elif tmp=="l": 
                    self.lese()
                elif tmp=="p": 
                    self.propBox()                      
                elif tmp=="t":
                    self.holeText()
                elif tmp=="q":  
                    if self.driver is not None:
                        self.driver.quit()
                    print ("See you")
                    sys.exit(0)    
                elif tmp=="l":  
                    links=self.sammle("https://www.brainzilla.com/logic/zebra/",self.num)
                    print(links)
                elif tmp=="s":
                    self.spalte=self.num                    
                elif tmp=="w":
                    self.schreibe()                    
                else:
                    print(tmp,"? n taG,  n Diff,  stArte, holBox, Evalbox, Propbox, holText, Info, n Spalte,  Lies, Write, Quit")
                    print("Ablauf: nG  nD Text, Prop, Eval, Info, Write")
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
    