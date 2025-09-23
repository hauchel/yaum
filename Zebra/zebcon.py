# -*- coding: utf-8 -*-
# Stufe 2 spaltet h.txt in 1 Datei pro Rätsel
# Aus
# Opt
# End
# dann
alis=[]
olis=[]
txlis=[]
txt=''
txa=''
mod=0
fnam='h'
cnt=0
with open(fnam+'.txt') as f:
    for line in f:
        line = line.strip()
        #print(mod)
        if mod==0:
            if line[:3] == 'Aus':
                t=line[4:].split(' ',1)[0]
                alis.append(t.capitalize())
            elif line[:3] == 'Opt':
                if line[4:] != '' : 
                    t=line[4:].split(' ',1)[0]
                    if t.isdigit(): t='Z'+t
                    if t[0]=='$':   t='D'+t[1:].split(',',1)[0]
                    olis.append(t.capitalize())
            elif line[:3] == 'End':
                txt=alis[0]+' = ['
                for o in olis:
                    txt += o+', '
                txt=txt[:-2]+'],'
                print (txt)
                txa+=txt+'\n'
                olis=[]
                del alis[0]
                if len(alis)==0:
                    txlis.append(txa)
                    txa=''
            else:
                mod=1
        else:
            if line[:3]=='===':  
                if cnt>0:
                    f2.close()
                cnt+=1
                f2=open(fnam+str(cnt)+'.txt','w')
                f2.write(txlis[0])
                f2.write('=\n')
                del txlis[0]
            else:
                f2.write(line+"\n")
    f2.close()   
