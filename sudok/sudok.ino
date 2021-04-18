// yet another sudoko solver for atmega328P
#define VERSION 'c'   // shown in runR on startup
#define ANZDEP 20     // checks per field (0..19)
#define ANZCHK 82     // entries in check table (starting at 1)
#define ANZLEV 64     // # levels for tracking (17 clues min!)
#define ANZCHA 100    // # list of fields for extanded checks
#include "depend.h"
#include <cluGamTst.h>
#include <cluCom.h>
#include <cluComSud.h>
#include <cluSlave.h>

byte single = 0;    // single step mode
byte levTrig = 99;  // sets single to 1 if levP > this
bool srtUp = true;  // check fields asc else desc

byte  block[10];   // each digit
byte  check[10];   // each digit
unsigned char  chack[ANZCHA];   // check-values for each field for

byte  chkMinFld, chkMinDof;
byte  chkMin[10];   // save each digit

typedef struct {
  byte levFldP;
  byte levChkP;
  byte levChkDof;
  byte levChkMin[4];
} levInfoT;

levInfoT levInfo[ANZLEV];
byte levP;

// zeigs:
const byte zFlow = 1;
const byte zInter = 2;
const byte zExec = 4;
const byte zDet = 128; //Detail

//10*100 stores fld
bool readEFld(byte nu) {
  Serial.println();
  if ((nu == 0) || (nu > 10)) return false;
  int adr = nu * 100;
  Serial.print(F("{\""));
  for (byte k = 1; k < ANZFLD; k++) {
    fld[k] = EEPROM.read(adr);
    showFldOne(k);
    adr++;
  }
  Serial.println(F("\"},"));
  return true;
}

bool writeEFld(byte nu) {
  if ((nu == 0) || (nu > 10)) return false;
  int adr = nu * 100;
  for (byte k = 1; k < ANZFLD; k++) {
    EEPROM.update(adr, fld[k]);
    adr++;
  }
  return true;
}

void getGame (byte gam) {
  byte b;
  byte f = 0;
  msgF(F("Game"), gam);
  for (byte k = 0; k < 81; k++) {
    b = pgm_read_byte(&games[gam][k]);
    Serial.print(char(b));
    f++;
    if (b == '.') {
      fld[f] = 0;
    } else {
      fld[f] = b - 48;
    }
  }
  Serial.println();
}

void showDeps (byte f) {
  byte b;
  msgF(F("Deps"), f);
  getGame(0);
  for (byte k = 0; k < ANZDEP; k++) {
    b = pgm_read_byte(&depend[f][k]);
    Serial.print(b);
    Serial.print(' ');
    fld[b] = 10;
  }
  showFld();
}

void showBlock() {
  Serial.println("Block:");
  for (byte r = 0; r < 10; r++) {
    Serial.print(char(block[r] + '0'));
  }
  Serial.println();

  for (byte cp = 0; cp < 10; cp++) {
    if (check[cp] == 0) {
      Serial.println();
      return;
    }
    Serial.print(char(check[cp] + '0'));
    Serial.print(' ');
  }
}

byte calcDofOne (byte f) {
  // returns number of possible for this field
  byte cp, b, v;
  for (byte k = 0; k < 10; k++) { //clear
    block[k] = 0;
  }
  for (byte k = 0; k < ANZDEP; k++) {
    b = pgm_read_byte(&depend[f][k]); // field to check
    v = fld[b];             //value
    block[v] = v;
    /* if (zeig > 3) {
      sprintf(str, "%2d  Fld %2d   Val %2d ", k, b, v);
      Serial.println(str);
      } */
  }
  cp = 0;
  if (srtUp) {
    for (byte k = 1; k < 10; k++) { //transfer check asc
      if (block[k] == 0) {
        check[cp] = k;
        cp++;
      }
    }
  } else {
    for (byte k = 9; k > 0; k--) { //transfer check desc
      if (block[k] == 0) {
        check[cp] = k;
        cp++;
      }
    }
  }
  check[cp] = 0;
  return (cp);
}


byte oneBlo (byte og) {
  // returns result  and   chkMinDof  chkMinFld
  //  0  solved                 11      0
  //  1  only one                1      field
  //  2  multiple                n      field
  //  3  unsolvable              0      field
  byte res, sel;
  char str[80];
  if (og == 0) og = ANZCHK;
  chkMinFld = 0;
  chkMinDof = 11;
  for (byte k = 1; k < og; k++) {
    if (fld[k] == 0) {
      res = calcDofOne(k);
      if (res < chkMinDof) { //new minimum found
        chkMinFld = k;
        chkMinDof = res;
        for (byte i = 0; i < 8; i++) {
          chkMin[i] = check[i];
        }
        if (zeig > 2) {
          sprintf(str, "oneBlo Fld %2d  Dof %2d  MinDof %2d  MinFld %2d ", k, res, chkMinDof, chkMinFld);
          Serial.println(str);
        }
        if (res == 0) { // this unsolvable
          return 3;
        } else {
          if (res == 1) { // only one possible, can exit 
            sel = check[0];
          } else {
            sel = 0;
          }
        }
        if (sel > 0) {
          chkMinFld = k; // ist schon so
          return 1;
        }
      } //mindo
    } // fld 0
  } // loop
  if (chkMinDof == 11) { // no fields to 
    return 0;
  } else {
    return 2;
  }
}

byte evalOnes() {
  // find forced moves, only call with levP==0, as it does not backtrack
  byte res = 1;
  char str[80];
  while (res == 1) {
    res = oneBlo(0);
    if (res == 1) { // only one
      fld[chkMinFld] = check[0];
      if (zeig > 0) {
        sprintf(str, "evalOnes set Fld %2d to %2d", chkMinFld, check[0]);
        Serial.println(str);
      }
    } else {
      if (zeig > 0) {
        sprintf(str, "evalOnes res % 2d  Minim %2d  Fld %2d", res, chkMinDof, chkMinFld);
        Serial.println(str);
      }
      return res;
    }
  }
  return 99; //avoid warning
}

byte findChack(byte n) {
  // sets field which can be set to n
  byte f;
  for (byte i = 0; i < ANZCHA; i++) {
    if (chack[i] >= 'a') {
      f = chack[i] - 'a';
    } else {
      if (chack[i] == n) {
        msgZ(1, F("findchack Field "), f);
        return f;
      }
    }
  }
  return 0;
}

byte checkInters(byte was) {
  // find intersections moves, only call with levP==0, as it does not backtrack
  // for affected fields generate possibles and chack-list like a34c789
  // returns 0 if fld is changed, 1 unchg, 2 dof 1,  >10 err
  byte res = 1;
  char str[80];
  char c;
  byte poss[10]; //count possibles
  byte chaP;
  byte f;
  byte cib;
  byte cas;
  Serial.print (F("Check Inters "));
  Serial.print(was);
  Serial.print("  ");

  if (was < 1) return 11;
  if (was < 10) { //  1..9 Block at 1,,4,7,28, , ,55
    cas = 1;
    f = was * 3 - 2;
    if (was > 6) {
      f = f + 36;
    } else if (was > 3) {
      f = f + 18;
    }
    cib = 0;
  } else if (was < 19) { //  10..18  Col
    cas = 2;
    f = was  - 9;
  } else if (was < 28) {   // 19..27 Row
    cas = 3;
    f = (was - 19) * 9 + 1;
  } else return 11;
  chaP = 0;
  for (byte i = 0; i < 10; i++) {
    poss[i] = 0;
  }
  for (byte i = 0; i < 9; i++) {
    if (fld[f] == 0) {
      res = calcDofOne (f);
      if (zeig > 8) {
        sprintf(str, "Fld %2d  Dof %1d  = ", f, res);
        Serial.print(str);
      }
      switch (res) {
        case 0:   // unsolvable
          msgF(F("checkInters bad calcDofOne"), res);
          return 12;
        case 1:   // only one, if not caught before
          msgF(F(" dof one for"), f);
          msgF(F("   value is"), check[0]);
          fld[f] = check[0];
          return 0;
        case 2:   //
          break;
        case 3:   //
          break;
      }
      c = f + 'a';  // Field number
      chack[chaP++] = c;
      for (byte cp = 0; cp < 10; cp++) {
        if (check[cp] == 0) { // end of
          if (zeig > 8)  Serial.println();
          break;
        }
        poss[check[cp]]++;    //
        chack[chaP++] = check[cp];
        if (zeig > 8) {
          c = check[cp] + '0';
          Serial.print(c);
        }
      }
    }  else { //Field !=0
      if (zeig > 8) {
        sprintf(str, "Fld %2d  is %1d", f, fld[f]);
        Serial.println(str);
      }
    }
    switch (cas) { //next field number
      case 1:   //
        f++;
        cib++;
        if (cib > 2) {
          cib = 0;
          f = f + 6;
        }
        break;
      case 2:   //
        f = f + 9;
        break;
      case 3:   //
        f++;
        break;
      default:
        msgF(F("Error Fatale cas"), cas);
        return 9;
    }
  } // next helper

  for (byte i = 1; i < 10; i++) {
    if (poss[i] == 1) {
      msgF(F(" Can set "), i);
      f = findChack(i);
      fld[f] = i;
      return 0;
    }
    Serial.print (poss[i]);
  }
  Serial.println();
  return 1;
}

void loopInters(byte bis ) {
  byte res;
  if (bis == 0) bis = 27; // check all
  byte i = bis;
  while (i > 0) {
    res = checkInters(i);
    if (res != 1)  msgF(F("Check rets "), res);
    if (res == 0) {  // field changed have to restart;
      i = bis;
    } else {
      i--;
    }
  }
}

void showLev() {
  char str[80];
  msgF(F("ShowLev "), levP);
  if (levP == 0) return;
  for (byte k = 0; k < levP; k++) {
    sprintf(str, "levP %2d  fldP %2d  chkP %2d  anz %2d = ", k, levInfo[k].levFldP, levInfo[k].levChkP, levInfo[k].levChkDof);
    Serial.print(str);
    for (byte i = 0; i < 4; i++) {
      Serial.print (levInfo[k].levChkMin[i]);
      Serial.print(' ');
    }
    Serial.println();
  }
}

bool saveLev(byte f, byte c, byte a) {
  if (levP >= ANZLEV) return true;
  levInfo[levP].levFldP = f;
  levInfo[levP].levChkP = c;
  levInfo[levP].levChkDof = a;
  for (byte i = 0; i < 4; i++) {
    levInfo[levP].levChkMin[i] = chkMin[i];
  }
  levP++;
  return false;
}

void doSetz(byte f, byte v) {
  char str[80];
  if (zeig > 0) {
    sprintf(str, "*** Setze fld %2d  auf % 2d ", f, v);
    Serial.println(str);
  }
  fld[f] = v;
  if (single) showFld();
}

bool backtrack() {
  byte neu;
  byte f;
  char str[80];
  while (true) {
    if (levP == 0) return false;
    levP--;
    levInfo[levP].levChkP++;
    neu = levInfo[levP].levChkMin[levInfo[levP].levChkP];
    f = levInfo[levP].levFldP;
    if (zeig > 0) {
      sprintf(str, "Back  levP %2d  fldP %2d  chkP %2d  anz %2d   neu %2d ", levP, f, levInfo[levP].levChkP, levInfo[levP].levChkDof, neu);
      Serial.println(str);
    }
    if (neu > 0) { //more within same lev
      doSetz(f, neu);
      if (zeig > 0) {
        sprintf(str, "Back Same levP %2d  fldP %2d  chkP %2d  anz %2d   neu %2d ", levP, levInfo[levP].levFldP, levInfo[levP].levChkP, levInfo[levP].levChkDof, neu);
        Serial.println(str);
      }
      levP++;
      return true;
    }
    // set field to 0 and back one more
    doSetz(f, 0);
  }
  return false; // no warnings
}

byte exec(bool ini) {
  /*    Return
        0     solved
        3     backtrack err

        6     abort by master
        7     abort by serial
        8,9   impossible, call expert
  */
  byte res;
  bool noch = true;
  char tmp;
  char str[80];
  if (ini ) {
    levP = 0;
  }
  while (noch) {
    res = oneBlo(0);
    if (zeig > 0) {
      sprintf(str, "exec levP %2d  res % 2d  Dof %2d  Fld %2d", levP, res, chkMinDof, chkMinFld);
      Serial.println(str);
    }
    switch (res) {
      case 0:   // done
        Serial.print("Ergeb:");
        showFld();
        return 0;
      case 1:
      case 2:   // down
        if (saveLev(chkMinFld, 0, chkMinDof)) {
          msgF(F("Level overrun"), levP);
          return 7;
        }
        doSetz (chkMinFld, chkMin[0]);
        if (levP > levTrig) single = 1;
        break;
      case 3:   // up
        if (!backtrack()) return 3;
        break;
      default:
        msgF(F("Error Fatale"), res);
        return 9;

    } // case
    if (slCmd == 'X') {
      msgF (F("abort by Master"), 0);
      return (6);
    }
    if (!single) {
      if (Serial.available() > 0) single = true;
    }
    if (single) {
      tmp = '?';
      while (tmp != ' ') {
        msgF (F("X to abort, space to cont"), 0);
        while (Serial.available() == 0);
        tmp = Serial.read();
        if (tmp == 'X') return 7;
        doCmd(tmp);
      }
    } //single
  } // while
  return 8;
}

void showTim() {
  elaTim = endTim - startTim;
  Serial.print(F("Elapsed: "));
  Serial.println(elaTim);
  Serial.println();
}

void doExec(bool neu) {
  char c;
  startTim = millis();
  runS = 'R';
  c = exec(neu) + '0';
  runS = 'C';
  runR = c;
  endTim = millis();
  showTim();
}

void doCmd(char tmp) {
  bool weg = false;
  if ( tmp == 8) { //backspace removes last digit
    weg = true;
    inp = inp / 10;
  }
  if ((tmp >= '0') && (tmp <= '9')) {
    weg = true;
    if (inpAkt) {
      inp = inp * 10 + (tmp - '0');
    } else {
      inpAkt = true;
      inp = tmp - '0';
    }
  }
  if (weg) {
    Serial.print(inp);
    return;
  }

  inpAkt = false;
  switch (tmp) {
    case '+':   //
      levP++;
      msgF(F("LevP"), levP);
      break;
    case '-':   //
      levP--;
      msgF(F("LevP"), levP);
      break;
    case ' ':   //
      break;
    case 'a':   //
      srtUp = false;
      msgF(F("Sort "), srtUp);
      break;
    case 'b':   //
      showBlock();
      break;
    case 'c':   //
      doExec(false);
      break;
    case 'd':   //
      showDeps(inp);
      break;
    case 'e':   //
      msgF(F("Calc Dof"), calcDofOne(inp));
      showBlock();
      break;
    case 'f':   //
      showFld();
      break;
    case 'g':   //
      getGame(inp);
      break;
    case 'h':   //
      checkInters(inp);
      break;
    case 'i':   //
      loopInters(inp);
      break;
    case 'I':   // reqd by Master
      runS = 'I';
      break;
    case 'l':   //
      showLev();
      break;
    case 'k':   //
      fld[inp] = 0;
      showFld();
      break;
    case 'n':   //
      evalOnes();
      break;
    case 'o':   //
      oneBlo(inp);
      break;
    case 'r':   //
      msgF(F("Read "), readEFld(inp));
      showFld();
      break;
    case 's':   //
      single = !single;
      msgF(F("Single"), single);
      break;
    case 't':   //
      levTrig = inp;
      msgF(F("Trigger"), levTrig);
      break;
    case 'u':   //
      srtUp = true;
      msgF(F("Sort "), srtUp);
      break;
    case 'v':   //
      eeData.info.myAdr = inp;
      setEprom();
      setup();
      break;
    case 'V':   //
      eeData.info.myAdr = slNum;
      setEprom();
      setup();
      break;
    case 'w':   //
      msgF(F("Write "), writeEFld(inp));
      break;
    case 'x':   //
      doExec(true);
      break;
    case 'z':   //
      zeig = inp;
      msgF(F("Zeiglev"), zeig);
      break;
    case 'Z':
      zeigClu = !zeigClu;
      msgF(F("ZeigClu"), zeigClu);
      break;
    default:
      msgF(F(" ?? "), tmp);
  } // case
}

void setup() {
  Serial.begin(38400);
  const char ich[] = "sudok " __DATE__  " "  __TIME__;
  Serial.println(ich);
  pinMode(ledPin, OUTPUT);
  ledOn(1);
  getMyAdr();
  Wire.begin(myAdr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  char tmp;
  if (Serial.available() > 0) {
    tmp = Serial.read();
    doCmd(tmp);
  } // avail

  if (slCmd != ' ') { // via TWI
    //msgF(F("slCmd ist "), slCmd);
    doCmd(slCmd);
    slCmd = ' ';
  } //

  currTim = millis();
  if (nexTim < currTim) {
    nexTim = currTim + tick;
    if (ledCnt == 0) {
      ledOff();
    } else {
      ledCnt--;
    }
  } // tick
}
