// solves wordle puzzle using known words
// Terminal setup: transmit CR for newline, local echo off, vt24
#include "optiboot.h"
// flash
#define NUMBER_OF_PAGES 102   // number of pages of 128 , one page contains 25 worte then terminated by \0xx
// allocate flash to write to, must be initialized, one more than used as 0 for EOS
const uint8_t flashSpace[SPM_PAGESIZE * (NUMBER_OF_PAGES + 1)] __attribute__ (( aligned(SPM_PAGESIZE) )) PROGMEM = {"\x0"}; //

// 128 bytes are
struct  wort_s {
  char eintr [25][5];
  uint8_t wnull;
  uint8_t wb;
  uint8_t wc;
} ;

typedef union {
  wort_s worte;
  byte ramBuffer[SPM_PAGESIZE];
} chunk_t ;
chunk_t chunk;

const byte anzBeis = 5;
char beis [anzBeis][126] = {
  "aalenabbauabendabgasabortabrufabteiabzugachimachseackeradelnadleradolfaehreaffigaffinafteragentagileahlenahmenahnenahninaktie",
  "eigeneileneiligeimereineneinereinigeiseneisigeiteleitereitleekelnekligekzemelendeliteelmaremsigendenengelengenenkelenormenzym",
  "appelaprilarchearealarenaarierarmeearminarndtarrakarschartenartigascheasiatasienasselathenatlasatmenaurumautoraxiombabelbacke",
  "badenbahrebaierbakerbambibanalbandebardebarkebaronbarrebasarbaselbasisbauchbauenbauerbayerbazarbeatebebenbeerebeetebeidebeize",
  "belagbelegbennoberndbertiberufbesenbetenbeterbetonbeugebeulebeutebezugbibelbiberbiegebienebiestbindebinombinsebirkebirnebison",
};

char buchs[26]; // +,-,' '
char muss[6];

enum stateT {norm, plus, minus, leer, gleich};
stateT state;


uint16_t inp;
bool inpAkt = false;
unsigned long currMs, prevMs = 0;
bool verbo = true;
bool plumi = true;


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

void readPage(uint16_t page) {
  msgF(F("readPage"), page);
  if ((page > 0) && (page <= NUMBER_OF_PAGES)) {  // page is translated -1 so eg 10 reads [9]
    optiboot_readPage(flashSpace, chunk.ramBuffer, page);
  } else {
    msgF(F("err: Page max "), NUMBER_OF_PAGES);
  }
}

void writePage(uint16_t page) {
  msgF(F("writePage"), page);
  if ((page > 0) && (page <= NUMBER_OF_PAGES)) {
    optiboot_writePage(flashSpace, chunk.ramBuffer, page);
  } else {
    msgF(F("err: Page max "), NUMBER_OF_PAGES);
  }
}

void copyBei(uint8_t  n) {
  if (n < anzBeis) {
    memcpy(chunk.ramBuffer, beis[n], 126);
    showChunk();
  }
}

void initBuchs() {
  memset (buchs, ' ', 26);
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
  }
  Serial.println();
  prepareMuss();
}


void prepareMuss() {
  byte m = 0;
  for ( byte i = 0; i < 26; i++)   {
    if (buchs[i] == '+' ) {
      muss[m] = i + 'a';
      m++;
      if (m > 4) break;
    }
  } // next
  muss[m] = 0;
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

void showChunk() {
  for ( byte i = 0; i < 128; i++)   {
    if (i % 80 == 0) Serial.println();
    if (chunk.ramBuffer[i] == 0) {
      Serial.println("<<");
      return;
    }
    Serial.write(chunk.ramBuffer[i]);
    if (i % 5 == 4) Serial.write(' ');
  } //next
}

void checkChunk() {
  byte ic = 4;
  char c;
  for ( byte i = 0; i < 128; i++)   {
    c = chunk.ramBuffer[i];
    if (c == 0) {
      Serial.println("<<");
      return;
    }
    if (verbo) Serial.write(c);
    if (buchs[c - 'a'] == '-') { // not allowed, jump to next wort
      i += ic;
      if (verbo) Serial.write('#');
      ic = 4;
      continue;
    }
    ic--;
    if (ic > 5) { // 255, wort zulässig, von i-4 bis i
      if (verbo) Serial.write(' ');
      ic = 4;
    }
  } //next
}


// syntax
// -abc  verbieten
// +abc  muss
// #abc  egal
// =a_b__     exakt 5!
// 0..9 zu inp
// w
// b beispiel -> flash
// r
// d suche laut drin
// e suche laut exac
// n suche laut nidri

void prompt() {
  switch (state) {
    case norm:   //
      Serial.print("N>");
      return;
    case plus:   //
      Serial.print("P>");
      return;
    case minus:   //
      Serial.print("M>");
      return;
    case leer:   //
      Serial.print("L>");
      return;
    default:   //
      Serial.print("E>");
      return;
  } // switch state
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
      state = plus;
      return;
    case '-':   //
      state = minus;
      return;
    case '#':   //
      state = leer;
      return;
    case '=':   //
      return;
    case 13:
      state = norm;
      prompt();
      return;
  } // switch control

  if (tmp <'a' or tmp >'z') {
    Serial.println("nicht a-z");
    state = norm;
    prompt();
    return;
  }
  if (state == plus) {
    buchs[tmp - 'a'] = '+';
    return;
  }
  if (state == minus) {
    buchs[tmp - 'a'] = '-';
    return;
  }
  if (state == leer) {
    buchs[tmp - 'a'] = ' ';
    return;
  }

  switch (tmp) {
    case 'b':   //
      copyBei(inp);
      break;
    case 'c':   //
      showChunk();
      break;
    case 'i':   //
      initBuchs();
      break;
    case 'p':   //
      Serial.println();
      prepareMuss();
      checkChunk();
      break;
    case 's':   //
      showBuchs();
      break;
    case 'r':   //
      readPage(inp);
      break;
    case 'w':   //
      writePage(inp);
      break;

    case 'v':   //
      verbo = !verbo;
      Serial.print("erbose ");
      if (verbo) {
        Serial.println("an");
      } else {
        Serial.println("aus");
      }
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
  msgF(F("wort_s"), sizeof(wort_s));
  msgF(F("chunk"), sizeof(chunk));
  msgF(F("flash"), sizeof(flashSpace));
  initBuchs();
  state = norm;
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
