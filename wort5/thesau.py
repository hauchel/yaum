# -*- coding: utf-8 -*-
import MySQLdb as mdb

class ldb():
    
    def __init__(self):
        self.db = mdb.connect(host='localhost',user='root', password='', database='thesau')
        self.cs= self.db.cursor()
        self.verbose=False

    def say(self,s):
        if self.verbose:
            print (str(s))

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
            tmp="kaputt"                         
        self.db.commit()
        return tmp

    def aspell_create_table(self):
        self.execc('DROP TABLE if EXISTS aspell;');
        tmp= "CREATE TABLE `aspell` (`wort` VARCHAR(10) NOT NULL "
        tmp+="COLLATE 'utf8_general_ci') "
        tmp+="COLLATE='utf8_general_ci';"
        self.execc(tmp);
        tmp= "LOAD DATA INFILE 'c:/lenovo/aspell.txt' IGNORE "
        tmp+="INTO table aspell fields TERMINATED BY '/';"
        self.execc(tmp);

    def wort5_create_table(self):
        self.execc('DROP TABLE if EXISTS wort5;');
        tmp= "CREATE TABLE `wort5` (`wort` VARCHAR(10) NOT NULL "
        tmp+="COLLATE 'utf8_general_ci') "
        tmp+="COLLATE='utf8_general_ci';"
        self.execc(tmp);
        
    def wort5_convert_from_aspell(self):
        tmp= "INSERT ignore INTO wort5(wort) "
        tmp+="SELECT wort FROM aspell WHERE CHAR_LENGTH( wort ) <6;"
        self.execc(tmp);
        
    def wort5_convert_from_openthesau(self):
        tmp= "INSERT ignore INTO wort5(wort) "
        tmp+="SELECT baseform FROM word_mapping WHERE CHAR_LENGTH( baseform ) <6;"
        self.execc(tmp);
        
    def wort5_index(self):  
        tmp= "ALTER IGNORE TABLE wort5 "
        tmp+="ADD UNIQUE INDEX idx_name (wort);"
        self.execc(tmp);
        
    def wort5_umlaute(self):        
        tmp="UPDATE wort5 SET wort = lower(wort); "
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'ä', 'ae'); "
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'ö', 'oe'); "
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'ü', 'ue'); "
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'ß', 'ss'); "
        self.execc(tmp);
        tmp="delete from wort5 WHERE CHAR_LENGTH(wort) != 5; "
        self.execc(tmp);
        # more strange chars
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'à', 'a');" #xE0
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'é', 'e');" #xE9
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'ê', 'e');" #xE9
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'é', 'e');" #xE9
        self.execc(tmp);
        tmp="UPDATE wort5 SET wort = REPLACE(wort, 'ñ', 'n');" #xF1
        self.execc(tmp);
        
        
    def wort5_toc_Beis(self):
        # group in 25 each
        res=self.fetch99("select wort from wort5 order by wort")
        lin=0
        wrt=0
        zei=""
        print("const byte anzBeis = xx;")
        print("const static char PROGMEM beis [anzBeis][126] = {")
        for r in res:
            zei=zei+r[0]
            wrt+=1
            if wrt>24:
                print ('  "'+zei+'",')
                zei=""
                wrt=0
                lin+=1
        print('  "'+zei+'" // anz='+str(lin+1))
        print('};')
        
            
            

    def readFile(self,myName,myNum):
        fil = open(myName, 'r')
        count=0
        while True:
            count += 1
            line = fil.readline().strip()
            if not line:
                break
            print("Line{}: {}".format(count, line))
            self.insertGame(count,myNum,'a',line)
        fil.close()


if __name__ == "__main__":
    k=ldb()  
    k.verbose=True
    if 1:
        k.aspell_create_table()
    if 1:
        k.wort5_create_table()        
    if 1:
        k.wort5_convert_from_aspell()        
    if 1:
        k.wort5_convert_from_openthesau()
    if 1:
        k.wort5_umlaute()
    if 1:
        k.wort5_index()        
    if 1:
        k.wort5_toc_Beis()
    if 0:
        k.fetch99("select wort from wort5")

       
