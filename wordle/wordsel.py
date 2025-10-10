# -*- coding: utf-8 -*-
# Wordle Selenium-related

import sys
import os
import time
import psutil
import pyautogui
import traceback

from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys
from selenium.common.exceptions import JavascriptException, NoSuchElementException
""" Gedächtnis wie ein Sieb:
find_element(By.ID, ‘id’)
find_element(By.NAME, ‘name’)  <name letters="si" length="5"></game-row>
find_element(By.XPATH, ‘xpath’) root: /html/body/...  relative //html/body/... kaputt html/body/...
All descendants: './/*',  Only direct children: './*',  All <div> descendants: './/div',  Direct divs: './div'
find_element(By.LINK_TEXT, ‘link_text’)
find_element(By.PARTIAL_LINK_TEXT, ‘partial_link_text’)
find_element(By.TAG_NAME, ‘tag_name’)
find_element(By.CLASS_NAME, ‘class_name’)
find_element(By.CSS_SELECTOR, ‘css_selector’)
    by ID	        #myid	        find_element(By.CSS_SELECTOR, "#myid")
    by class	    .btn	        find_element(By.CSS_SELECTOR, ".btn")
    by attribute	[name='email']	find_element(By.CSS_SELECTOR, "[name='email']")
    by hierarchy	div > span	    find_element(By.CSS_SELECTOR, "div > span")
    everything	*	                find_elements(By.CSS_SELECTOR, "*")

x.get_attribute("id"))
x.tag_name
x.text
x.click()  if  x.is_displayed() and x.is_enabled()
x.getText()
ShadowRoot .find_element(), .find_elements() only
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
                    raise ValueError('Ferdisch')
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
                        if succnt==5: raise ValueError('Ferdisch')
                p+=1
        return buli
        
    def getFeld4(self):
        # wordledeutsch nervt mit shadow-root /html/body/game-app//game-theme-manager/div/game-modal
        # game-app shadow-root game-theme-manager  shadow-root div id=game
        # 
        buli=[]
        e = self.find_dom("game-app", "game-theme-manager")
        board = e.find_element(By.ID,"board")
        rows = board.find_elements(By.TAG_NAME,"game-row")
        #print('Anzahl game-row',len(rows))
        for row in rows:
            lett=row.get_attribute("letters")
            if lett!='':
                if self.verbose: print("game-row",lett)
                if lett is None:
                    return buli
                shar = self.driver.execute_script("return arguments[0].shadowRoot", row)
                tiles = shar.find_elements(By.CSS_SELECTOR, "game-tile")	
                #print('Anzahl tiles',len(tiles))
                p=0
                succnt=0
                for tile in tiles:
                    bu=tile.get_attribute("letter").upper()
                    #print(bu,tile.get_attribute("evaluation") )
                    shat = self.driver.execute_script("return arguments[0].shadowRoot", tile)
                    divs = shat.find_elements(By.CSS_SELECTOR, "div")	
                    #print('Anzahl divs',len(divs))     
                    wrt=divs[0].get_attribute("data-state")  # Gott seis getrommelt und gepfiffen...
                    if wrt=='absent':
                        buli.append([0,bu,p])
                    elif wrt=='present':
                        buli.append([1,bu,p])
                    elif wrt=='correct':
                        buli.append([8,bu,p])
                        succnt+=1
                        if succnt==5: raise ValueError('Ferdisch')
                    else:
                        print("getFeld4 ?? wrt",wrt) 
                        raise ValueError('Unbekannt?')
                    p+=1
        return  buli     

    def nachFeld4(self):
        # stellt fest ob pop-up da, dann klick button Nochmal, etwas nifty
        e = self.find_dom("game-app", "game-theme-manager")
        gam=e.find_element(By.ID,"game")
        dir=gam.find_elements(By.XPATH, "./*")      #3 is game-modal open
        x=self.driver.execute_script("return arguments[0].shadowRoot", dir[3])  #returns shadow_root
        el=x.find_elements(By.CSS_SELECTOR, '*')    # 4 is close-icon
        if el[4].is_displayed():
            print('Displayed')
        else:
            print('Not Displayed')
            return []
        mo=dir[3].find_elements(By.XPATH,'./*')     # mo[0] game-stats
        x=self.driver.execute_script("return arguments[0].shadowRoot", mo[0])  #returns shadow_root
        el=x.find_elements(By.CSS_SELECTOR, '#refresh-button')     
        el[0].click()
    
    def find_dom(self, *selectors):
        """
        Traverses nested shadow roots using CSS selectors.
        Example:
            element = self.find_in_shadow_dom("game-app", "game-theme-manager", "game-row")
        """
        script = "let el = document.querySelector(arguments[0]);"
        # Chain each shadow root
        for i in range(1, len(selectors)):
            script += f"if (el) el = el.shadowRoot && el.shadowRoot.querySelector(arguments[{i}]);"
        script += "return el;"
        try:
            element = self.driver.execute_script(script, *selectors)
            if not element:
                raise NoSuchElementException(f"Element not found for chain: {' > '.join(selectors)}")
            return element
        except JavascriptException as e:
            raise RuntimeError(f"JavaScript error while traversing shadow DOM: {e}")
            
    def getFeld(self):
        print("Hole Feld")
        if self.puznum==1:
            return self.getFeld1()
        elif self.puznum==2:            
            return self.getFeld2()
        elif self.puznum==3:            
            return self.getFeld2()
        elif self.puznum==4:            
            return self.getFeld4()            
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
            time.sleep(0.1)
            # will Fokus zurück, geht nicht mit
            # console = gw.getWindowsWithTitle("Zebra")[0]
            # console.activate()
            pyautogui.hotkey("alt", "tab") 
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
        body_html = self.driver.execute_script("return document.body.innerHTML;")
        print("Entry,body_html")
        
        for i in range(len(iframes)):
            self.driver.switch_to.default_content()
            print("iframe",i)
            try:
                self.driver.switch_to.frame(i)
                self.pfadler()
                body_html = self.driver.execute_script("return document.body.innerHTML;")
                print('iframe',body_html)
                elements = self.driver.find_elements(By.CSS_SELECTOR, "body > *")
                print(f"Found {len(elements)} direct elements inside frame")
                self.analy(elements)
            except Exception as inst:
                print ("Frame",i,"Exception "+str(inst)[:60])
        self.driver.switch_to.default_content()         
        self.pfadler()
        
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
"""
import wordsel   
from selenium.webdriver.common.by import By
g=wordsel.wosel() 
g.website(4)
g.verbinde()

"""
# oder:
if __name__ == "__main__":
    g=wosel()
    g.website(2)
    
    
#
#
#
#    