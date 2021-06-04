set dude=C:\Users\hh\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino18/bin/avrdude
set conf=-CC:\Users\hh\AppData\Local\Arduino15\packages\ATTinyCore\hardware\avr\1.5.2/avrdude.conf
%dude% %conf% -v -pattiny85 -cstk500v2 -PCOM10 -t
pause
