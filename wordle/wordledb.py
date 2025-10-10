# -*- coding: utf-8 -*-
import MySQLdb as mdb
import time

class ldb():
    
    def __init__(self):
        self.db = mdb.connect(host='localhost',user='root', password='', database='wordle')
        self.db.set_character_set('utf8')
        self.cs= self.db.cursor()
        self.cs.execute('SET NAMES utf8;')
        self.cs.execute('SET CHARACTER SET utf8;')
        self.cs.execute('SET character_set_connection=utf8;')
        self.verbose=False
        #  enable for Buchstabenhaufigkeit  only        
        self.hauf = {'A': 65, 'B': 19, 'C':30, 'D': 50, 'E': 174, 'F':16, 'G':30,
                     'H':47,'I':75,'J': 2, 'K': 12, 'L':34,'M': 25, 'N':98,
                     'O':25,'P':8,'Q': 0, 'R': 70, 'S':72,'T': 61, 'U':43,
                     'V':7,'W':19,'X': 0, 'Y':0, 'Z':11             }
        
    def zeit(self):
        return time.strftime("'%Y-%m-%d  %H:%M:%S'", time.localtime())

    def say(self,s):
        if self.verbose:
            print (str(s))

    def logge(self,text,anzahl,prio=5):
        t="insert into log (Zeit,Prio,Text,Anzahl) values ("
        t+=self.zeit()+","+str(prio)+",'"+text+"',"+str(anzahl)+");"
        self.execc(t)
        print (">"+text)

    def fetch1(self,query):
        self.say ("Fetch1: "+query)     
        self.cs.execute(query)
        result = self.cs.fetchone()
        self.say ("answer: %s" % str(result))
        self.db.commit()
        return result

    def fetch99(self,query):
        self.say ("Fetch99: "+query)     
        self.cs.execute(query)
        result = self.cs.fetchall()
        self.say ("answer: %s" % str(result))
        self.db.commit()
        return result
    
    def execc(self,query):
        self.say ("execc: "+query) 
        try:
            tmp=self.cs.execute(query)
        except Exception as inst:
            print ("ldb exec Exception "+str(inst)   )
            print(query)
            tmp="kaputt"                         
        self.db.commit()
        return tmp

    def getManyOne(self,query):
        # returns one col as list
        tmp=self.fetch99(query)
        w=[]
        for t in tmp:
            ts=str(t[0])
            w.append(ts)
        return w

    def getDict(self,query):
        # returns dict, col 0 is key
        tmp=self.fetch99(query)
        w={}
        for t in tmp:
            w[t[0]]=t[1]
        return w
    
    def getDataBu(self):
        return self.getManyOne("select wort from worte order by prio desc, dup asc,  buhauf desc")
        
    def getDataWo(self):
        return self.getManyOne("select wort from worte order by prio desc, dup asc,  worthauf desc")

    def getAllWortHauf(self):
        return self.getDict("select wort, worthauf from worte order by worthauf desc")
        
    def numDup(self,w):
        n=0
        for i in range(5):    
            for j in range(i+1,5):
                #print(i,"  ",j)
                if w[i]==w[j]:
                    n+=1
        return n
    
    def updDup(self,n):
        erg=self.getManyOne("select wort from worte where dup=99 limit "+str(n))  # unbekannte haben 99
        for w in erg:
            if len(w)==5:
                dup=self.numDup(w)
                self.execc("update worte set dup ="+str(dup)+" where wort ='"+w+"';")
            else:
                print("updDup Len Err ",w)
                self.delWort(w)
            
    def numHauf(self,w):
        h=0
        try:
            for i in range(5):    
                h += self.hauf[w[i]]
        except KeyError:
            print("Wie kommt denn bitte",w,"hier rein?")
        return h                
                
    def updBuHauf(self,n):
        erg=self.getManyOne("select wort from worte where buhauf=0 limit "+str(n))
        for w in erg:
            dup=self.numHauf(w)
            self.execc("update worte set buhauf ="+str(dup)+" where wort ='"+w+"';")
 
    def setWortHauf(self,w,n):
        self.execc("update worte set worthauf ="+str(n)+" where wort ='"+w+"';") 
    
    def getWortHauf(self,w):
        pl=self.getManyOne("select worthauf from worte where wort ='"+w+"';")         
        return pl[0]
    
    def setWortPrio(self,w,n):
        print(w,'auf Prio',n)
        self.execc("update worte set prio ="+str(n)+" where wort ='"+w+"';")  

    def getWortPrio(self,w):
        pl=self.getManyOne("select prio from worte where wort ='"+w+"';")         
        return pl[0]
    
    def getWortInfo(self,w):
        pl=self.fetch99("select * from worte where wort ='"+w+"';")         
        return pl[0]
        
    def loadfile(self):
        self.execc("delete FROM worte;") #!!
        self.execc("LOAD DATA LOCAL INFILE 'd:greed/d.txt' INTO TABLE worte CHARACTER SET UTF8MB4;")

    def writefile(self):
        hauf=self.getAllWortHauf()
        filnam='worte.txt'
        print(len(hauf),'nach',filnam)
        with open(filnam, "w", encoding="utf-8") as f:
            for d in hauf: f.write(f"{d}  {hauf[d]}\n")
        
    def delWort(self,w):
        self.execc("delete FROM worte where wort='"+w+"';")
    
    def insWort(self,w,prio=0,woha=0):
        if len(w)==5:
            if all('A' <= b <= 'Z' for b in w):
                self.execc(f"INSERT INTO worte (wort,prio,worthauf) VALUES ('{w}',{prio},{woha});")
            else: 
                print(w,"nicht A-Z")
        else:
            print(w,"nicht 5 Zeichen")                


    def newtab(self):
        tx= """
    CREATE TABLE `worte_neu` (
	`wort` VARCHAR(5) NOT NULL DEFAULT '' COLLATE 'utf8mb4_general_ci',
	`prio` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0',
	`dup` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '99',
	`buhauf` INT(11) NULL DEFAULT '0',
	`worthauf` INT(11) NULL DEFAULT '0',
	PRIMARY KEY (`wort`) USING BTREE)
    COLLATE='utf8mb4_general_ci'
    ENGINE=InnoDB;
"""
        self.execc(tx)

# Test / Wartungsfunktionen wenn direkt aufgerufen, einfach 0 in 1 ändern 
# oder in python
#  from wordledb import ldb
#  k=ldb()
# z.B. t=k.getWortHauf()

if __name__ == "__main__":
    k=ldb()  
    k.verbose=True
    if 0:
        k.newtab()   # ggf worte_neu vorher löschen, dann in worte umbenennen
    if 0:
        k.verbose=False
        k.writefile()   # überschreibt bestehende!
    if 0:
        erg=k.getManyOne("select wort from worte where dup=99 limit 10")
        print("Bereich:",erg)
        for w in erg:
            print(w,k.numDup(w)     )          
    if 0:
        k.verbose=False
        k.updDup(2000)
    if 0:
        k.setWortHauf("COUCH",44)
    if 0:
        k.verbose=False
        k.updHauf(1000)       
  
    if 0:
       k.updDup(100)
       k.updHauf(100)          
        
