// common routines for cluster msgs, twi, vt100, slaves

//Eprom for twi-adresses and types
#define ANZSLA 20  // slaves max-1 
#define ANZEPR ANZSLA * 2 + 3

#include <Wire.h>
#include <EEPROM.h>
const byte ledPin = 13;
byte ledCnt = 2;

unsigned long currTim, startTim, endTim;
unsigned long nexTim = 0;
unsigned long tick = 1000;     //
byte myAdr = 4;
bool slaAkt = false;  // if true show only active slaves
byte zeig = 0; // debug
bool vt100Mode = false; // true use vt100 features

void prnt(PGM_P p) {
  while (1) {
    char c = pgm_read_byte(p++);
    if (c == 0) break;
    Serial.write(c);
  }
  Serial.write(" ");
}

void msgF(const __FlashStringHelper *ifsh, uint16_t n) {
  // text verbraucht nur flash
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  prnt(p);
  Serial.println(n);
}

void msgL(byte lvl, const __FlashStringHelper *ifsh, uint16_t n) {
  // only if bool set in zeig
  if (zeig & lvl) {
    PGM_P p = reinterpret_cast<PGM_P>(ifsh);
    prnt(p);
    Serial.println(n);
  }
}

void msgZ(byte lvl, const __FlashStringHelper *ifsh, uint16_t n) {
  // zeigt abh lvl
  if (zeig <= lvl) return;
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  prnt(p);
  Serial.println(n);
}

/*   as it disturbes SPI (!)
  void ledOn(byte n) {
  // digitalWrite(ledPin, HIGH); might cause problems with units using SPI, must be handled there
  ledCnt = n;
  }

  void ledOff() {
  //digitalWrite(ledPin, LOW);
  }

*/

void vt100Esc(byte n, char c) {
  char str[15];
  sprintf(str, "\x1B[%d%c", n, c);
  Serial.print(str);
}

void vt100Home() {
  Serial.print("\x1B[H");
}

void vt100ClrEol() {
  Serial.print("\x1B[K");
}
void vt100ClrEos() {
  Serial.print("\x1B[J");
}

typedef struct {
  byte myAdr;
  byte myTyp;
  byte myFFE;
  byte slaAdr[ANZSLA];
  byte slaTyp[ANZSLA];
} eeInfoT;

typedef union {
  eeInfoT info;
  byte byt[ANZEPR];
} eeDataT;

eeDataT eeData;

void getEprom() {
  byte adr = 0;
  for (byte k = 0; k < ANZEPR; k++) {
    eeData.byt[k] = EEPROM.read(adr++);
  }
}

void setEprom() {
  byte adr = 0;
  for (byte k = 0; k < ANZEPR; k++) {
    EEPROM.update(adr++, eeData.byt[k]);
  }
}

void getMyAdr() {
  getEprom();
  myAdr = eeData.info.myAdr;
  msgF(F("TWI from EProm"), myAdr);
  if (myAdr > 127) {
    myAdr = 4;
    msgF(F("Addr default"), myAdr);
  }
}

typedef struct { // slaves
  byte slaAdr;    // physical twi = target, 0 not in use
  byte slaTyp;    // whoever needs it e.g. for prim 2 3 4
  char slaRunS;   // state I R C E
  char slaRunR;   // result of exec if C
  byte slaTask;   // called by this task
} slaInfoT;

slaInfoT slaInfo[ANZSLA];
byte slaSel = 1;        // selector slavelist

/* Example:
       epr  adr typ  S R     Tsk
   1    11   11   2  I a     0
 > 2    12    0   2          0
   3    13    0   2          0
   shown by (o)nline, (O) toggles to show only adr!=0
   >  : selected (p)
   epr: address stored in Eprom, (>127 sets adr to 0)  (M)
   adr: current address, 0 = not active (m)
   typ: depends , set by (P)
   S,R: depends
   Tsk: current/last task using this slave
*/
void showSlaves() {
  char str[50];
  if (vt100Mode) {
    vt100Home();
  } else {
    Serial.println();
  }
  Serial.println(F("       epr  adr typ   S R  Tsk"));
  for (byte k = 1; k < ANZSLA; k++) {
    if (slaAkt && ( slaInfo[k].slaAdr == 0)) {
    } else {
      if (k == slaSel) {
        Serial.print (" >");
      } else {
        Serial.print ("  ");
      }
      sprintf(str, "%2d    %2d   %2d  %2d   %c %c   %2d", k, eeData.info.slaAdr[k], slaInfo[k].slaAdr, slaInfo[k].slaTyp, slaInfo[k].slaRunS, slaInfo[k].slaRunR, slaInfo[k].slaTask);
      Serial.println(str);
    } // pri
  } // loop
}
