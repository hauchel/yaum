// solves wordle puzzle using known words
// Terminal setup: transmit CR for newline, local echo off, 38400
//TODO  Buchstaben nicht an bestimmter Stelle

#include "thesau94.h"

// 128 bytes are
byte ramBuffer[128];
char buchs[26]; // +,-,' '
char ist[6];
char muss[6];
char exac[6];
byte exacP;


enum instat_t {norm, plus, minus, leer, exakt};
instat_t instat;
uint16_t inp;
bool inpAkt = false;
unsigned long currMs, prevMs = 0;
bool verbo = false;
bool autom = false; // search after cr



void prnt(PGM_P p) {
  // flash to serial until \0
  while (1) {
    char c = pgm_read_byte(p++);
    if (c == 0) break;
    Serial.write(c);
  }
  Serial.write(' ');
}

void msgF(const __FlashStringHelper *ifsh, uint16_t n) {
  // text verbraucht nur flash
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  prnt(p);
  Serial.println(n);
}

void copyBeis(uint8_t  n) {
  if (n < anzBeis) {
    byte i = 0;
    byte b = !0;
    while (b != 0) {
      b = pgm_read_byte(&beis[n][i]);
      ramBuffer[i++] = b;
    }
  }
}

void initBuchs() {
  memset (buchs, ' ', 26);
  memset (exac, '*', 5);
}

void showBuchs() {
  uint8_t i;
  char str[20];
  for ( i = 0; i < 26; i++)   {
    if (i % 13 == 0)  {
      Serial.println();
    }
    sprintf(str, "%1c %1c  ", char(i + 'a'), buchs[i]);
    Serial.print(str);
  } // next i
  Serial.println();
  for ( i = 0; i < 5; i++)   {
    Serial.write(exac[i]);
  }
  Serial.println();
  prepareMuss();
  showMuss();
}

void showMuss() {
  for ( byte i = 0; i < 6; i++)   {
    if (muss[i] == 0) {
      Serial.println("<");
      return;
    }
    Serial.write(muss[i]);
    Serial.write(' ');
  }
  Serial.println("??");
}

void prepareMuss() {
  // buchs to  muss
  byte m = 0;
  for ( byte i = 0; i < 26; i++)   {
    if (buchs[i] == '+' ) {
      muss[m] = i + 'a';
      m++;
      if (m > 4) break;
    }
  } // next
  muss[m] = 0;
}

void showBuff() {
  for ( byte i = 0; i < 128; i++)   {
    if (i % 80 == 0) Serial.println();
    if (ramBuffer[i] == 0) {
      Serial.println("<<");
      return;
    }
    Serial.write(ramBuffer[i]);
    if (i % 5 == 4) Serial.write(' ');
  } //next
}

bool checkMuss() {
  // true if ist and muss
  bool found;
  for ( byte m = 0; m < 5; m++) {
    if (muss[m] == 0 ) return true;
    found = false;
    for ( byte i = 0; i < 5; i++) {
      if (muss[m] == ist[i]) {
        found = true;
        break;
      }
    } // next i
    if (!found) return false;
  } // next j
  return true;
}

bool checkExac() {
  // true if ist and exac
  for ( byte i = 0; i < 5; i++) {
    if (exac[i] != '*' ) {
      if (exac[i] != ist[i]) return false;
    }
  } // next i
  return true;
}

bool checkIst() {
  if (checkMuss()) {
    if (checkExac()) {
      if (verbo) {
        Serial.write('!');
      } else {
        for ( byte j = 0; j < 5; j++)   {
          Serial.write(ist[j]);
        }
        Serial.write(' ');
      }
      return true;
    } else {
      if (verbo) Serial.write('E');
      return false;
    }
  } else { // muss fail
    if (verbo) Serial.write('M');
    return false;
  }
}

void checkBuff() {
  byte ic = 0;
  char c;
  byte  found = 0;
  for ( byte i = 0; i < 128; i++)   {
    c = ramBuffer[i];
    if (c == 0) {
      if (found > 0) Serial.println("<<");
      return;
    }
    if (verbo) Serial.write(c);
    if (buchs[c - 'a'] == '-') { // not allowed, jump to next wort
      i += (4 - ic);
      if (verbo) Serial.write('#');
      ic = 0;
    } else {
      ist[ic] = c;
      ic++;
      if (ic > 4) { //wort in ist  ist zulässig
        if (checkIst()) found += 1;
        ic = 0;
      } // zulässig
    } // allowed
  } //next
}

void doLoop(uint16_t n) {
  unsigned long anfMs, endMs;
  prepareMuss();
  anfMs = millis();
  if (n == 0) n = anzBeis;
  if (n > anzBeis) n = anzBeis;
  for ( byte b = 0; b < n; b++) {
    copyBeis(b);
    checkBuff();
  }
  endMs = millis();
  Serial.print("Pages ");
  Serial.print(n);
  Serial.print(" took ");
  Serial.println(endMs - anfMs);
}

// syntax
// -abc  verbieten
// +abc  muss
// #abc  egal
// =a*b**     exakt 5!
// 0..9 zu inp
// b beispiel -> flash
// d suche laut drin
// e suche laut exac
// n suche laut nidri

void prompt() {
  instat = norm;
  if (autom) Serial.print("A>"); else Serial.print("N>");
}

void doCmd(byte tmp) {
  if ((tmp >= '0') && (tmp <= '9')) {
    if (inpAkt) {
      inp = inp * 10 + (tmp - '0');
    } else {
      inpAkt = true;
      inp = tmp - '0';
    }
    Serial.print("\b\b\b\b");
    Serial.print(inp);
    return;
  }
  inpAkt = false;
  Serial.print(char(tmp));


  switch (tmp) {
    case '+':   //
      instat = plus;
      return;
    case '-':   //
      instat = minus;
      return;
    case '#':   //
      instat = leer;
      return;
    case '=':   //
      instat = exakt;
      exacP = 0;
      return;
    case 13:
      if (autom) doLoop(0);
      prompt();
      return;
  } // switch control

  if (instat == exakt) {
    if (exacP > 4) {
      Serial.println("<<");
      prompt();
      return;
    }
    if ( tmp == '*') {
      exac[exacP++] = tmp;
      return;
    }
  }

  if (tmp <'a' or tmp >'z') {
    Serial.println("nicht a-z");
    prompt();
    return;
  }
  if (instat == plus) {
    buchs[tmp - 'a'] = '+';
    return;
  }
  if (instat == minus) {
    buchs[tmp - 'a'] = '-';
    return;
  }
  if (instat == leer) {
    buchs[tmp - 'a'] = ' ';
    return;
  }
  if (instat == exakt) {
    exac[exacP++] = tmp;
    return;
  }
  switch (tmp) {
    case 'a':   //
      autom = !autom;
      Serial.print("utom ");
      if (autom) Serial.println("an"); else  Serial.println("aus");
      break;
    case 'b':   //
      copyBeis(inp);
      showBuff();
      break;
    case 'c':   //
      showBuff();
      break;
    case 'i':   //
      initBuchs();
      break;
    case 'l':   //
      Serial.println("oop");
      doLoop(inp);
      break;
    case 'p':   //
      Serial.println("ruf");
      prepareMuss();
      checkBuff();
      break;
    case 's':   //
      showBuchs();
      break;
    case 'v':   //
      verbo = !verbo;
      Serial.print("erbose ");
      if (verbo) Serial.println("an"); else  Serial.println("aus");
      break;
    default:
      Serial.print(tmp);
      Serial.println ("?  0..5, +, -, show, verbose");
  } //case
  prompt();
}

void setup() {
  const char info[] = "wort5 " __DATE__  " " __TIME__;
  Serial.begin(38400);
  Serial.println(info);
  msgF(F("beisp"), sizeof(beis));
  initBuchs();
  prompt();
}


void loop() {
  if (Serial.available() > 0) {
    doCmd( Serial.read());
  } // serial

  currMs = millis();
  if (currMs - prevMs > 1000) {
    prevMs = currMs;
  } // timer
}
