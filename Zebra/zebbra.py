# -*- coding: utf-8 -*-
# hole ein zebra riddle in Datei 
import sys
from inp import inp
import time
import random
import traceback

from bs4 import BeautifulSoup

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
        self.links=[]
        self.verbose=0
        self.baseurl="https://www.brainzilla.com"
        self.url=self.baseurl+"/logic/zebra/who-owns-the-crocodile/"
        
     
    def startDriver(self):
        service = Service(executable_path=r'D:/greed/chromedriver.exe')
        options = webdriver.ChromeOptions()
        options.add_experimental_option('excludeSwitches', ['enable-logging'])
        options.add_argument(r"--user-data-dir=C:\temp") 
        options.add_argument('--ignore-certificate-errors')
        options.add_argument('--ignore-ssl-errors')
        options.add_argument("--window-size=1300x1080")
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

    def info(self):
        print(self.props)
        for t in self.zeilen:
            print(t)
        print('=')
        for t in self.texte:
            print(t)
    
    def schreibe(self):        
        t=self.url.split('/')
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
        
            
    def sammle(self,url,anz=20):
        if anz == 0:
            i=0
            for l in self.links:
                print(i,l)
                i+=1
            return
            
        self.links=[]
        print ('sammle',anz,' von ',url)
        self.driver.get(url)
        time.sleep(1)
        soup = BeautifulSoup(self.driver.page_source, "lxml")
        anchors = soup.find_all("a", href=True)
        found=0
        for a in anchors:
             href = a["href"].strip()
             if href.startswith("/logic/zebra/"): 
                 print(found,href)
                 self.links.append(href)
                 found+=1
                 if found>anz: return self.links
        return self.links
        
    def linksel(self,num):
        if len(self.links) < num:
            print('zu viele',num)
            return
        self.url=self.baseurl+self.links[num]
        print('url is ',self.url)
        
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
            t=p.text
            print('t vor',t)
            t=t.replace(' ','')
            print('t nach',t) 
            self.props.append(t)
       
    def evalBox(self,spa=1):
        game = self.driver.find_element(By.ID, "game")
        cols=game.find_elements(By.CLASS_NAME, "column")
        print('cols',len(cols))
        sels=cols[spa].find_elements(By.TAG_NAME, "select")
        print('sels',len(sels))
        self.zeilen=[]
        i=0
        for s in sels:
#            for opt in s.find_elements(By.TAG_NAME, "option"):
#                val = opt.get_attribute("value")
#                txt = opt.text.strip()
#                print(val, txt)    
            select_obj = Select(s)
            zeil=' '
            for opt in select_obj.options:
                print(f"{opt.get_attribute('value')} | {opt.text}")
                tx=opt.text
                print('tx=',tx)
                t=tx.split()
                print(t)
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
            print("Aktueller sichtbarer Text:", current_text)
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
                elif tmp=="e":
                    self.evalBox() 
                elif tmp=="i": 
                    self.info() 
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
                    self.schreibe()                    
                elif tmp=="w":  
                    self.linksel(self.num)
                else:
                    print(tmp,"? Links(anz) Wahl(num), stArte, holBox, Evalbox, Propbox, holText, Info, Schreib, Quit")
                    print("Ablauf: nn Links, nn Wahl, Text, Prop, Eval, Info, Schreib")
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
    