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

    def logge(self,text,prio=5):
        t="insert into log (Zeit,Prio,Text) values ("
        t+=self.zeit()+","+str(prio)+",'"+text+"')"
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
    
    def getData(self):
        return self.getManyOne("select wort from worte order by dup asc, hauf desc")
    
    def numDup(self,w):
        n=0
        for i in range(5):    
            for j in range(i+1,5):
                #print(i,"  ",j)
                if w[i]==w[j]:
                    n+=1
        return n
    
    def updDup(self,n):
        erg=k.getManyOne("select wort from worte where dup=99 limit "+str(n))
        for w in erg:
            dup=self.numDup(w)
            self.execc("update worte set dup ="+str(dup)+" where wort ='"+w+"';")
            
    def numHauf(self,w):
        h=0
        for i in range(5):    
            h += self.hauf[w[i]]
        return h                
                
    def updHauf(self,n):
        erg=k.getManyOne("select wort from worte where hauf=0 limit "+str(n))
        for w in erg:
            dup=self.numHauf(w)
            self.execc("update worte set hauf ="+str(dup)+" where wort ='"+w+"';")
                
    def loadfile(self):
        self.execc("delete FROM worte;")
        self.execc("LOAD DATA LOCAL INFILE 'd:greed/d.txt' INTO TABLE worte CHARACTER SET UTF8MB4;")
    
    def updatehauf(self):
        tx =" UPDATE worte t1 JOIN worte_hauf t2 ON t1.wort = t2.wort  SET t1.anzahl = t2.anzahl;"
        
    def delwort(self,w):
        self.execc("delete FROM worte where wort='"+w+"';")

if __name__ == "__main__":
    k=ldb()  
    k.verbose=True

    if 0:
        erg=k.getManyOne("select wort from worte where dup=99 limit 10")
        print("Bereich:",erg)
        for w in erg:
            print(w,k.numDup(w)     )          
    if 0:
        k.verbose=False
        k.updDup(2000)
    if 0:
       print(k.numHauf("COUCH"))
    if 0:
        k.verbose=False
        k.updHauf(1000)       
  
    if 0:
       k.updDup(100)
       k.updHauf(100)          
        
