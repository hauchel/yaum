// same code used in Bahn and rfid, use BIG to switch
//
const char txAttach[] PROGMEM = "Attach";
const char txDetach[] PROGMEM = "Detach";
const char txDetall[] PROGMEM = "Det All";
const char txDelay[] PROGMEM = "Delay";
const char txEndprog[] PROGMEM = "Endprog";
const char txInvalid[] PROGMEM = "Invalid Ex";
const char txMovOn[] PROGMEM = "SloMov on";
const char txReserved[] PROGMEM = "Reserved";
const char txReturn[] PROGMEM = "Return";
const char txSetVar1[] PROGMEM = "SetVar1 ";
const char txSetVar2[] PROGMEM = "SetVar2 ";
const char txSkipF[] PROGMEM = "Skip if False";
const char txSkipT[] PROGMEM = "Skip if True";
const char txStepDone[] PROGMEM = "Wait Stepper";
const char txStepZero[] PROGMEM = "Stepper Zero";
const char txStepos[] PROGMEM = "Step Pos";
const char txTracOff[] PROGMEM = "Trace off";
const char txTracOn[] PROGMEM = "Trace on";
const char txVar1[] PROGMEM = "SetVar1";
const char txVar2[] PROGMEM = "SetVar2";
const char txVerboOff[] PROGMEM = "Verbose off";
const char txVerboOn[] PROGMEM = "Verbose on";
const char txWaitF[] PROGMEM = "Wait False";
const char txWaitT[] PROGMEM = "Wait True";

byte prog[progLen];
byte progp = 0;
byte beflen; // sick, set by decodeprog
byte prognum;
bool mess = false;
bool dirty = false;   // true: current prog changed
bool teach = false;   // true: teach-in
bool trace = false;   // true: show ececuted command
byte traceLin;        // avoid many trace outputs during wait

#ifdef BIG

unsigned long stepTim[progLen]; // set on entry
const byte anzStack = 16;
byte var1[anzStack];
byte var2[anzStack];
byte actIn5[anzStack];
byte actIn6[anzStack];
byte progpS[anzStack];
byte prognumS[anzStack];
byte stackp;

#endif

void decodProg(char tx[40], byte p) {
  // annoying to save RAM
  char str[20];
  char nam[20];
  byte lo, hi;
  lo = prog[p] & 0x0F;
  hi = prog[p] >> 4;
  beflen = 1;
  switch (hi) {
    case 0:   //
      strcpy_P(tx, txReserved);
      return;
    case 1:   //
      strcpy_P(nam, (char *)pgm_read_word(&(servNam[lo])));
      sprintf(tx, "%s %2u %s", "Sel Serv", lo, nam);
      return;
    case 2:   //
      if (lo < 10) {
        sprintf(tx, "%s %2u ", "Set Pos", lo);
        return;
      }
      switch (lo) {
        case 0xA:
          strcpy_P(tx, txMovOn);
          break;
        case 0xB:
          strcpy_P(tx, txStepZero);
          break;
        case 0xD:
          strcpy_P(tx, txStepDone);
          break;
        case 0xE:
          strcpy_P(tx, txAttach);
          break;
        case 0xF:
          strcpy_P(tx, txDetach);
          break;
        default:
          sprintf(tx, "%s %2u ", "Set Pos invalid", lo);
      }
      return;
    case 3:   //
      strcpy_P(str, txWaitT);
      sprintf(tx, "%s %2u ", str, lo);
      return;
    case 4:   //
      strcpy_P(str, txWaitF);
      sprintf(tx, "%s %2u ", str, lo);
      return;
    case 5:   //
      strcpy_P(str, txSkipT);
      sprintf(tx, "%s %2u ", str, lo);
      return;
    case 6:   //
      strcpy_P(str, txSkipF);
      sprintf(tx, "%s %2u ", str, lo);
      return;
    case 7:   //
      if (lo < 8)    sprintf(tx, "%s %2u ", "DJNZ 1 to",  lo);
      else sprintf(tx, "%s %2u ", "DJNZ 2 to",  lo - 8);
      return;
    case 8:   //
      if (lo < 8)    sprintf(tx, "%s %2u ", "ActIn5 on",  lo);
      else sprintf(tx, "%s %2u ", "ActIn6 on",  lo - 8);
      return;
    case 0xA:   //
      if (lo < 8)    sprintf(tx, "%s %2u ", "Label",  lo);
      else sprintf(tx, "%s %2u ", "Jump to",  lo - 8);
      return;
    case 0xB:   //
      sprintf(tx, "%s %2u ", "Call Prog", lo);
      return;
    case 0xC:   //
      sprintf(tx, "%s %2u ", "Pos Stepper", lo);
      return;
    case 0xE:   //
      beflen = 2;
      switch (lo) {
        case 0:
          strcpy_P(str, txDelay);
          sprintf(tx, "%s %3u ", str, 10 * prog[p + 1]);
          return;
        case 1:
          strcpy_P(str, txVar1);
          break;
        case 2:
          strcpy_P(str, txVar2);
          break;
        case 5:
          strcpy_P(str, txStepos);
          break;
        default:
          strcpy_P(str, txInvalid);
          sprintf(tx, "%s %2u ", str,  lo);
          return;
      }
      sprintf(tx, "%s %3u ", str, prog[p + 1]);
      return;
    case 0xF:   //
      switch (lo) {
        case 0:
          strcpy_P(tx, txReturn);
          return;
        case 2:
          strcpy_P(tx, txDetall);
          return;
        case 3:
          strcpy_P(tx, txTracOff);
          return;
        case 0xF:
          strcpy_P(tx, txEndprog);
          return;
        default:
          sprintf(tx, "%s %2u ", "F Special", lo);
      }
      return;
    default:
      sprintf(tx, "%s %2u ", "Define Hi", hi);
      return;
  }
}

void insProg(uint16_t was) {
  if (progp >= progLen) {
    msgF(F("ProgP"), progp);
    return;
  }
  dirty = true;
  for (byte i = progLen - 1; i > progp; i--) {
    prog[i] = prog[i - 1];
  }
  prog[progp] = byte(was);
  progp++;
}

void delatProg() {
  for (byte i = progp; i < progLen; i++) {
    prog[i] = prog[i + 1];
  }
  prog[progLen - 1] = 255;
  dirty = true;
}

void showProg() {
  char str[60];
  char txt[40];
  char ind[3];

  Serial.println();
  if (progp >= progLen) progp = progLen - 1; //limit
#ifdef BIG
  unsigned long mess0;
  uint16_t mw, dau;
  mess0 = stepTim[0];
#endif
  for (byte i = 0; i < progLen; i++) {
    decodProg(txt, i);
    if (i == progp) {
      strcpy(ind, "->");
    } else {
      strcpy(ind, "  ");
    }
#ifdef BIG
    if (mess) {
      dau = stepTim[i + 1] - stepTim[i];
      if (stepTim[i] == 0) {
        mw = 0;
      } else {
        mw = stepTim[i] - mess0;
      }
      sprintf(str, "%2u  %02X %3u %3s  %-20s %6u  %6u", i, prog[i], prog[i], ind, txt, dau, mw);
    }  else {
      sprintf(str, "%2u  %02X %3u %3s  %-20s", i, prog[i], prog[i], ind, txt);
    }
#else
    sprintf(str, "%2u  %02X %3u %3s  %-20s", i, prog[i], prog[i], ind, txt);
#endif
    Serial.println(str);
    if (prog[i] == 255 ) return;
    if (beflen == 2) i++;
  }
}

void showProgX() {
  char str[10];
  for (byte i = 0; i < progLen; i++) {
    sprintf(str, "%u,", prog[i]);
    Serial.print(str);
    if (prog[i] == 255 ) break;
  }
  Serial.println();
}

bool readProg(uint16_t  p, bool cleanonly) {
  if (dirty and cleanonly) {
    msgF(F("\7 **Prog dirty, use w or R"), p);
    return false;
  }
  prognum = p;
  progp = 0;
  p = p * progLen;
  EEPROM.get(p, prog);
  dirty = false;
  return true;
}

void writeProg(uint16_t  p) {
  p = p * progLen;
  if ((p + progLen) < epromAdr) {
    EEPROM.put(p, prog);
    msgF(F(" Prog write Adr="), p);
    dirty = false;
  } else {
    msgF(F("epromAdr! No Prog write "), p);
  }
}

void showAllProgX() {
  Serial.println();
  for (byte i = 0; i < 16; i++) {
    readProg(i, false);
    Serial.print(i);
    Serial.print("R");
    showProgX();
  }
}
/*  conversion proglen
void Prog4842() {
  // read 48 write 42
  uint16_t  p;
  for (byte i = 0; i < 16; i++) {
    Serial.print(i);
    p = i * 48;
    Serial.print(" read ");
    Serial.print(p);
    EEPROM.get(p, prog);
    p = i * 42;
    Serial.print(" write ");
    Serial.println(p);
    EEPROM.put(p, prog);
  }
}
*/
void redraw() {
  char str[50];
  char nam[20];
  strcpy_P(nam, (char *)pgm_read_word(&(progNam[prognum])));
  sprintf(str, "Program %2u %s", prognum, nam);
  vt100Clrscr();
  Serial.println(str);
  showProg();
}

void progge(byte b) {
  if (teach) insProg(b);
}

void showPos() {
  char str[50];
  char nam[20];
  strcpy_P(nam, (char *)pgm_read_word(&(servNam[sersel])));
  sprintf(str, "Servo %2u %s at %1u ", sersel, nam, posp[sersel]);
  Serial.print(str);
  strcpy_P(nam, (char *)pgm_read_word(&(posNam[sersel][posp[sersel]])));
  sprintf(str, "%s (%u)", nam, serpos[sersel]);
  Serial.println(str);
  for (byte i = 0; i < 10; i++) {
    strcpy_P(nam, (char *)pgm_read_word(&(posNam[sersel][i])));
    sprintf(str, "%2u   %5u  %s", i, mypos.pos[sersel][i], nam);
    Serial.println(str);
  }
}
