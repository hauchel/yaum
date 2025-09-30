# -*- coding: utf-8 -*-
import msvcrt
import time

class inp():      
    
    def kbhit(self):
        return msvcrt.kbhit()
    
    def getch(self):
        return msvcrt.getwch()
    
    def sleepOrKey(self,sec):
        print ("\b\b#",)
        for cnt in range(0,sec):
            if self.kbhit():
                tmp=self.getch()
                if tmp=='#':
                    raise ValueError('you pressed #')
                return tmp
            time.sleep(1)
        return ""

    def readnumkey(self):
        print ("Choose: ",)
        tmp= self.getwch()      
        print (tmp,)
        if tmp>="0" and tmp <="9":
            return int(tmp)
        print ("??")
        return 0
 
    def auswahl(self,lis,auto=True):
        le=len(lis)
        print ("Auswahl aus "+str(le),)
        if le==0:
            print ("Nix da")
            return le
        if le==1:
            if auto:
                print ("Automatisch *****")
                return le
        print (":")
        for n in range(0,le):
            print ( n+1,lis[n])
        a=self.readnumkey()
        if a>0:
            print ("->",lis[a-1])
        else:
            print ("nix")
        return a
            
    
if __name__ == "__main__":
    import sys
    i=inp()
    while 1:
            if i.kbhit():
                tmp= str(i.getch()      )
                print (tmp,)
                try:
                    if tmp=="a":
                        print("A")
                    elif tmp=="q":
                        print ("See you")
                        sys.exit(0)
                    else:
                        print("?")
                except Exception as inst:
                    print ("main Exception "+str(inst))
                print 
            
