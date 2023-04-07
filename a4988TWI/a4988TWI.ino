#define NOTWI
/*
  A4988 Stepper using Timer2
  Driving 4 * A4988,
  Using TWI mcp23008 8 bit for Enable and Dir:
  32103210
  Ena Dir   white   green
  Connect Ground  and 5V!
    blue VMOT 16,
    blk GND 9,
    red VDD 10
  Outputs  4988:
    14  2B    B-  orng
    13  2A    B   blue
    12  1A    A   red
    11  1B    A-  yello

  If notwi use ports below to set max 2 Steppers
*/

const byte nPts = 20; // number of Points
byte volatile tim2CompB = 0;     // debug shows timcnt after int processing
byte volatile tim2Busy = 0;     // after int 1: done 2..5: # of steppers-1
bool wait = false;              //
byte tim2cs = 6;                    // value of TCCR2B i.e. clock select, 6 = /256
byte steSel = 0;  // selected stepper
byte inpPtr = 222;  // to next to fetch, >edilen=none
byte thisPrg = 0;   // prg last read

#ifdef NOTWI
const byte nStp = 2; // number of Steppers
const byte steDir[nStp] = {4, 7}; //green
const byte steStp[nStp] = {5, 8}; //lo>hi clocks, yell
const byte steEna[nStp] = {6, 9}; //hi disabled, weiss
uint16_t volatile tim2Count[nStp] = {0, 0}; // steps remaining
uint16_t volatile stePos[nStp] = {0, 0};    // position 0..
int16_t steRicht[nStp] = {0, 0}; ;           // -1 dec 1 inc
bool steDirPlus[nStp] = {true, false};      // set steDir for increasing position
#else
const byte nStp = 4; // number of Steppers
const byte steStp[nStp] = {6, 7, 8, 9}; //Pin, lo>hi clocks, yell  to 7
uint16_t volatile tim2Count[nStp] = {0, 0, 0, 0}; // steps remaining
uint16_t volatile stePos[nStp] = {0, 0, 0, 0};    // position 0..
int16_t steRicht[nStp] = {0, 0, 0, 0}; ;           // -1 dec 1 inc
bool steDirPlus[nStp] = {true, true, false, true};         // set steDir for increasing position
#endif

int16_t pts[nPts][nStp];    // waypoints
const byte nStk = 5; //stacksize
byte stkPrg[nStk];   // prog
byte stkPos[nStk];   //
byte stkPtr = 0;

#include <EEPROM.h>
#include "helper.h"
#include "vt100Edi.h"
#include <Wire.h>
#include "timer2.h"


unsigned long zwiMs = 10;  // check each ms for button
uint16_t movs = 10;       //  Button Delta (set by m)

bool togg = false;    // for manual toggle of step
bool verbo = false;


const byte mcpDev = 0x20; // TWI-ADDR
byte mcpVal = 255; // Enable and Dir High

byte setMcp(byte reg, byte val) {
#ifdef NOTWI
  return 0;
#endif
  Wire.beginTransmission(mcpDev);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission();     // stop transmitting
}

byte getMcp(byte reg) {
#ifdef NOTWI
  return 0;
#endif
  Wire.beginTransmission(mcpDev);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)mcpDev, (uint8_t)1);
  return Wire.read();
}

void setLi() {
  bitSet(mcpVal, steSel);
#ifdef NOTWI
  digitalWrite(steDir[steSel], HIGH);
#else
  setMcp(9, mcpVal);
#endif
}

void setRe() {
  bitClear(mcpVal, steSel);
#ifdef NOTWI
  digitalWrite(steDir[steSel], LOW);
#else
  setMcp(9, mcpVal);
#endif
}

void setAllDisa() {
  mcpVal = mcpVal | 0xF0;
#ifdef NOTWI
  for (byte i = 0; i < nStp; i++) {
    digitalWrite(steEna[steSel], HIGH);
  }
#else
  setMcp(0, 0); // in case init failed...;
  setMcp(9, mcpVal);
#endif
}

void setDisa() {
  msgF(F(":Disabled"), steSel);
  bitSet(mcpVal, steSel + 4);
#ifdef NOTWI
  digitalWrite(steEna[steSel], HIGH);
#else
  setMcp(9, mcpVal);
#endif
}

void setEna() {
  msgF(F(":Enabled"), steSel);
  bitClear(mcpVal, steSel + 4);
#ifdef NOTWI
  digitalWrite(steEna[steSel], LOW);
#else
  setMcp(9, mcpVal);
#endif
}

void stePosition(uint16_t neu) {
  if (neu == 0) return;
  if (neu == stePos[steSel]) return;
  if (neu > stePos[steSel]) {
    steRicht[steSel] = 1;
    if (steDirPlus[steSel]) {
      setLi();
    } else {
      setRe();
    }
    tim2Count[steSel] = neu - stePos[steSel];
  } else {
    steRicht[steSel] = -1;
    if (steDirPlus[steSel]) {
      setRe();
    } else {
      setLi();
    }
    tim2Count[steSel] = stePos[steSel] - neu;
  }
}

void putPoints() {
  EEPROM.put(600, pts);
}

void getPoints() {
  EEPROM.get(600, pts);
}

bool putEdi(byte n) {
  int adr = n * ediLen;
  if (n > 9) {
    msgF(F("putEdi?"), n);
    return false;
  }
  msgF(F("put Adr"), adr);
  EEPROM.put(adr, ediStr);
  return true;
}

bool getEdi(byte n) {
  int adr = n * ediLen;
  if (n > 9) {
    msgF(F("getEdi?"), n);
    return false;
  }
  msgF(F(":Get Adr"), adr);
  EEPROM.get(adr, ediStr);
  thisPrg = n;
  ediStr[ediLen - 1] = 0; // just in case
  return true;
}

void showPoints() {
  char str[50];
  Serial.println();
  for (byte p = 0; p < nPts; p++) {
    sprintf(str, "%2u %4u %4u %4u %4u ", p, pts[p][0],  pts[p][1], pts[p][2], pts[p][3]); // lazy
    Serial.println(str);
  }
}

void setPoint(byte p) {
  if (p >= nPts) {
    msgF(F("Invalid setPoint"), p);
    return;
  }
  for (byte i = 0; i < nStp; i++) {
    pts[p][i] = stePos[i];
  }
  showPoints();
}

void posPoint(byte p) {
  if (p >= nPts) {
    msgF(F("Invalid posPoint"), p);
    return;
  }
  byte steSelS = steSel;
  for (steSel = 0; steSel < nStp; steSel++) {
    stePosition(pts[p][steSel]);
  }
  steSel = steSelS;
}

bool pushStk() {
  msgF(F("pushStk"), stkPtr);
  msgF(F("inpPtr"), inpPtr);
  if (stkPtr >= nStk) return false;
  stkPrg[stkPtr] = thisPrg;
  stkPos[stkPtr] = inpPtr;
  stkPtr++;
  return true;
}

bool popStk() {
  msgF(F("popStk"), stkPtr);
  if (stkPtr <= 1) {
    inpPtr = ediLen + 1;
    return false;
  }
  stkPtr--;
  inpPtr = stkPos[stkPtr];
  inpPtr++;
  msgF(F("inpPtr "), inpPtr);
  return getEdi(stkPrg[stkPtr]);
}

void info() {
  char str[50];
  msgF(F("SteSel"), steSel);
  Serial.println("     Pos    Cnt  Ri  Dir");
  for (byte i = 0; i < nStp; i++) {
    sprintf(str, "%2u %5u  %5u  %2d   %2d", i, stePos[i], tim2Count[i], steRicht[i], steDirPlus[i]);
    Serial.println(str);
  }
  msgF(F("OCR2A"), OCR2A);
  msgF(F("mcpVal"), mcpVal);
}

void posInfo() {
  char str[50];
  sprintf(str, "%4u %4u %4u %4u", stePos[0], stePos[1], stePos[2], stePos[3]);
  Serial.println(str);
}

void afterEdi(byte c) {
  Serial.println();
  if (c == 27) return;
  msgF(F("Writeback"), thisPrg);
  putEdi(thisPrg);
  if (c == 193) { //up
    inpPtr = 0;
  }
  if (c == 194) { //down
    inpPtr = 0;
  }
}

void doCmd(char x) {
  Serial.print(char(x));
  if (doNum(x)) {
    return;
  }
  switch (x) {
    case 13:
      vt100Clrscr();
      break;
    case ' ':
      setAllDisa();
      inpPtr = ediLen + 1; // to stop
      stkPtr = 0;
      prnF(F("Stopped"));
      break;
    case '"':
      afterEdi(doEdi());
      break;
    case '!':
      inpPtr = 0;
      break;

    case 'a':
      OCR2A = inp;
      msgF(F(":OCR2A"), OCR2A);
      break;
    case 'b':
      OCR2B = inp;
      msgF(F("OCR2B"), OCR2B);
      break;
    case 'c':
      timer2Init();
      msgF(F("TCCR2B"), TCCR2B);
      break;
    case 'd':
      msgF(F(":Delay"), inp);
      delay(inp * 10);
      break;
    case 'D':
      setDisa();
      break;
    case 'E':
      setEna();
      break;
    case 'f':
      steDirPlus[steSel] = (inp != 0);
      info();
      break;
    case 'g':
      if (getEdi(inp)) {
        ediY = inp + 2;
        afterEdi(doEdi());
      }
      break;
    case 'h':
      if (putEdi(inp)) msgF(F("edi put"), inp);
      break;
    case 'i':
      info();
      break;
    case 'j':
      getPoints();
      break;
    case 'J':
      putPoints();
      prnF(F("Saved"));
      break;
    case 'l':
      msgF(F("Links High"), steSel);
      setLi();
      break;
    case 'm':
      movs = inp;
      msgF(F("movs"), movs);
      break;
    case 'n':
      zwiMs = inp;
      msgF(F("zwiMs"), inp);
      break;
    case 'o':
      msgF(F(":pOs"), inp);
      stePos[steSel] = inp;
      break;
    case 'p':
      posPoint(inp);
      break;
    case 'P':
      setPoint(inp);
      break;
    case 'q':
      break;
    case 'Q':
      break;
    case 'r':
      msgF(F("Rechts Low"), steSel);
      setRe();
      break;
    case 's':
      msgF(F(":Stepto"), inp);
      stePosition(inp);
      break;
    case 't':
      msgF(F("Toggle"), togg);
      if (togg) {
        digitalWrite(steStp[steSel], HIGH);
      } else {
        digitalWrite(steStp[steSel], LOW);
      }
      togg = !togg;
      break;
    case 'u':
      showPoints();
      break;
    case 'v':
      verbo = !verbo;
      msgF(F("Verbo"), verbo);
      break;
    case 'w':
      wait = true;
      tim2Busy = 0;
      break;
    case 'W':
      if (wait) Serial.print("Wai "); else Serial.print("NoW ");
      Serial.println(tim2Busy);
      break;
    case 'x':
      if (!pushStk()) {
        msgF(F("exec push err"), stkPtr);
        inpPtr = ediLen + 1;
        break;
      }
      if (getEdi(inp)) {
        if (inpPtr < ediLen) { //running, will be inced below
          msgF(F("exec run from"), inpPtr);
          inpPtr = 255;
        } else {
          msgF(F("exec new"), inpPtr);
          inpPtr = 0;
        }
      } else {
        msgF(F("getEdu fail "), inp);
        inpPtr = ediLen + 1;
      }
      break;
    case 'y':
      steSel = inp;
      if (steSel >= nStp) {
        steSel = 0;
      }
      setEna();
      msgF(F("SteSel"), steSel);
      break;
    default:
      Serial.print('?');
      Serial.println(int(x));
      inpPtr = 222;
  } // case
}

bool do1Butt(byte po, byte pro) {
  // returns true if action taken
  if (digitalRead(po) == LOW) {
    if (inpPtr > ediLen) {
      msgF(F("Prog running But"), pro);
      return true;
    }
    getEdi(pro) ;
    inpPtr = 0;
    return true;
  }
  return false;
}

void doButts() {
  if (digitalRead(A0) == LOW) {
    getEdi(0) ;
    inpPtr = 0;
    stkPtr = 0;
    return;
  }
  if (do1Butt(A1, 1)) return;
  if (do1Butt(A2, 2)) return;
  if (do1Butt(A3, 3)) return;
}

void setup() {
#ifdef NOTWI
  const char info[] = "a4988 NOTWI " __DATE__  " "  __TIME__;
#else
  const char info[] = "a4988TWI " __DATE__  " "  __TIME__;
#endif
  Serial.begin(38400);
  Serial.println(info);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
#ifdef NOTWI
  for (byte i = 0; i < nStp; i++) {
    pinMode(steEna[i], OUTPUT);
    digitalWrite(steEna[i], HIGH);
    pinMode(steStp[i], OUTPUT);
    digitalWrite(steStp[i], LOW);
    pinMode(steDir[i], OUTPUT);
  }
#else
  Wire.begin();
  setMcp(0, 0); // all as output;
  setMcp(9, 0xFF); //disable;
  setMcp(9, mcpVal);
  for (byte i = 0; i < nStp; i++) {
    pinMode(steStp[i], OUTPUT);
    digitalWrite(steStp[i], LOW);
  }
#endif
  steSetup();
  getPoints();

}

void loop() {
  if (Serial.available() > 0) {
    doCmd( Serial.read());
  }
  if (wait) {
    if (tim2Busy == 1) {
      wait = false;
    }
  } else {
    if (inpPtr < ediLen) {  // points to next to be executed
      if (ediStr[inpPtr] == 0) {
        popStk();
      } else {
        doCmd(ediStr[inpPtr]);
        inpPtr++;
        if ((inpPtr >= ediLen) || (ediStr[inpPtr] == 0)) {
          popStk();
        }
      }
    } //inp
  } // wait
  if (tim2CompB != 0) {
    if (verbo)  Serial.println(tim2CompB);
    tim2CompB = 0;
  }
  currMs = millis();
  if ((currMs - prevMs >= zwiMs) ) {
    doButts();
    prevMs = currMs;
  }
}
