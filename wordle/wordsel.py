# -*- coding: utf-8 -*-
# Wordle Selenium-related

import sys
import os
import time
import psutil
import traceback

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
class wosel():
    
    def __init__(self):
        self.driverloc = r'D:/greed/chromedriver.exe'
        self.arg="user-data-dir=C:\\Users\\hh\\AppData\\Local\\Google\\Chrome\\chess"
        self.driver=None
        self.puzurl=""
        self.buref={}   # Reference element zum Auslesen eines Buchstabens aus keyboard der website
        self.buval={}   # Stand jedes Buchstabens auf keyboard
        self.verbose=False
        self.website(3,False)  #Default
                    
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
        #Spiegel return buchliste (0,el.text,spalte)
        try:
            self.driver.switch_to.frame(0)   
        except:
            #print("Switch Frame Failed")
            pass
        buli=[]
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
                    buli.append ([0,el.text,spalte])
                elif 'flipping-success' in clns:
                    if self.verbose: print('success')
                    buli.append ([8,el.text,spalte])
                elif 'flipping-almost' in clns:
                    if self.verbose: print('almost')
                    buli.append ([1,el.text,spalte])
                else:
                    print('Kein flipping:',zeile,i,el.text,clns)
                    raise ValueError('Ferdisch:')
            i+=2
            spalte+=1
            if i % 10 == 0:
                zeile+=1
                spalte=0
        return buli
        
    def getFeld2(self):
        #wdl        
        buli=[]
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
                        buli.append([0,d.text,p])
                    elif wrt=='near':
                        buli.append([1,d.text,p])
                    elif wrt=='succ':
                        buli.append([8,d.text,p])
                        succnt+=1
                        if succnt==5: raise ValueError('Ferdisch:')
                p+=1
        return buli
        
    def getFeld4(self):
        # wordledeutsch not supported
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
        #print("Hole Feld ",self.puznum)
        if self.puznum==1:
            return self.getFeld1()
        elif self.puznum==2:            
            return self.getFeld2()
        elif self.puznum==3:            
            return self.getFeld2()
        else:
            print("Hole geht nicht für ",self.puznum)
            return []

    def getKeywert(self,bu):
        # holt keyboard für bu
        cla = self.buref[bu].get_attribute("class")
        #if self.verbose: print('keywert',bu,cla)
        clas=cla.split()  #A hg-button hg-standardBtn status-3 multiple-status
        clas.append('unbek')
        if clas[2]=='status-1':
            self.buval[bu]='Netdr'
        elif clas[2]=='status-2':
            self.buval[bu]='Drinn'
        elif clas[2]=='status-3':
            if 'multiple-status' in clas:
                self.buval[bu]='Multi'
            else: 
                self.buval[bu]='Einzg'
        else:
            self.buval[bu]='unbek'
        
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
                #if self.verbose: print(bu,cla)
                if len(bu)==1:
                    self.buref[bu]=d
                    self.getKeywert(bu)
    
    def getKeybrd(self):
        #print("Hole Keyboard ",self.puznum)
        if self.puznum==1:
            self.getKeybrd1()
        else:
            print("getKeybrd nicht für ",self.puznum)

                
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
        self.driver.get(self.puzurl)
        
    def sende(self,w):
        actions = ActionChains(self.driver) 
        actions.send_keys(w)
        actions.send_keys(Keys.RETURN)
        actions.perform()

    def refresh(self):
        self.driver.get(self.driver.current_url)   
        
    def quit(self):
        if self.driver is not None: self.driver.quit()
 
# nur zum debuggen: 
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

# zum debuggen entweder
# python
# import wordsel   
# g=wordsel.wosel() 
# g.website(2)
# g.verbinde()
#
# oder:
if __name__ == "__main__":
    g=wosel()
    g.website(2)
    
    
#
#
#
#    