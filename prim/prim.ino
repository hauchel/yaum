// must use optiboot loader as it writes to own flash
#include "optiboot.h"
#define NUMBER_OF_PAGES 55   // number of pages to use (limited by flash size)
#define VERSION 'c'
#include <cluCom.h>
#include <cluComPri.h>
byte backP = 0;
char backBuf[32];
byte recvP = 0;
char recvBuf[32];
char slCmd = ' '; // sent to slave by Cxx to be executed in doCmd
uint64_t slNum = 0; //      "   after some cmds
uint32_t elaTim = 0; // back to master after T
uint64_t topPrim = 0; // highest generated prime
uint16_t topPage = 1; // next page to write
char runS   = 'I';  // State Idle Running Complete fs Open
char runR   = VERSION;  // Result 0..9, here Initial version
byte zeigClu = 0; // debug output communication

#define ANZPR8 55
const static unsigned char PROGMEM prim8 [ANZPR8] = // one byte primes
{ 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
  73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
  179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 0
};

const uint64_t primTst64[10] = {0, 30781L * 30803L, 33359L * 33359L, 56003, 61967, 57667, 56003, 61967, 57667, 56003}; // recall using p
/*
  uint8_t  my08 = 255;
  uint16_t my16 = 65535;
  uint32_t my32 = 4294967295UL;
  uint64_t my64 = 18446744073709551615ULL;
*/
uint64_t  inp;
bool inpAkt;                  // true if last input was a digit
const byte inpSM = 10;        // Stack for inps size
uint64_t inpStck[inpSM];      //
byte  inpSP = 0;              //
const byte memoM = 10;        // Memory
uint64_t memo[memoM];         //

// zeigs:
const byte zFlow = 1;
const byte zNew = 2;    //
const byte zTeach = 4;    //
const byte zFact = 8;    //
const byte zCheck = 64;
const byte zDet = 128;    //Detail

// allocate flash to write to, must be initialized, one more than used as 0 for EOS
const uint8_t flashSpace[SPM_PAGESIZE * (NUMBER_OF_PAGES + 1)] __attribute__ (( aligned(SPM_PAGESIZE) )) PROGMEM = {"\x0"}; //

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

byte checkP8_64(uint64_t z) {
  // checks z for divisions by primes <255
  // returns 0 if prime, else divisor
  byte b = !0;  // current divisor
  byte pp = 0;   // pointer to prime
  uint64_t rem;
  while (b != 0) {
    b = pgm_read_byte(&prim8[pp++]);
    //    Serial.print (b);
    //    Serial.print ("  ");
    rem = z % b;
    if (rem == 0) break;
    //    Serial.println (uint32_t(rem));
  }
  return b;
}

byte checkP8_16(uint16_t z) {
  byte b = !0;
  byte pp = 0;
  uint16_t rem;
  while (b != 0) {
    b = pgm_read_byte(&prim8[pp++]);
    if (zeig & zCheck ) {
      Serial.print (b);
      Serial.print ("  ");
    }
    rem = z % b;
    if (rem == 0) break;
  }
  return b;
}

uint16_t checkP16_64(uint64_t z) {
  // checks z for divisions by 2 byte primes from flash, returns divisor or 0 if prim
  uint16_t b = !0;
  //    uint16_t og = sqr(z) <-
  uint16_t pp = 0;
  uint64_t rem;
  while (b != 0) {
    b = pgm_read_word(&flashSpace[pp]);
    if (zeig & zCheck ) {
      Serial.print (b);
      Serial.print ("  ");
    }
    if (b == 0) return 0; //end of flash
    if (b == z) return 0; // tbd more than sqrt
    rem = z % b;
    if (rem == 0) return b; //divisor b found
    pp += 2;
  }
  return b;  // gcc wants it
}


uint16_t checkDivP16_64(uint64_t z) {
  // checks z for divisions by 2 byte primes from flash, returns divisor, 0 is prim
  // breaks if sqrt, but much slower than before
  uint16_t b = !0;
  uint16_t pp = 0;
  uint64_t rem;
  uint64_t diso;

  while (b != 0) {
    b = pgm_read_word(&flashSpace[pp]);
    if (zeig & zCheck ) msgF(F("Teste "), b);
    if (b == 0) break;
    diso = z / b;
    rem = z % b;
    if (zeig & zCheck) msg64(F("Diso "), diso);
    if (diso < b) {
      return 0;
    }
    if (rem == 0) break; // fact found
    pp += 2;
  }
  return b;
}

uint16_t firstZero16() {
  // returns position of first word zero from flash
  uint16_t b = !0;
  uint16_t pp = 0;
  while (b != 0) {
    b = pgm_read_word(&flashSpace[pp]);
    if (b == 0) break;
    pp += 2;
  }
  return pp;
}

void doIndex(byte pag) {
  char str[30];
  if (pag > NUMBER_OF_PAGES) {
    msgF(F("Err topPage:"), topPage);
    runR = '7';
    return;
  }
  Serial.println();
  uint16_t b = !0;
  uint16_t pp = pag * SPM_PAGESIZE;
  pag = 1; // pages start at 1
  while (b != 0) {
    b = pgm_read_word(&flashSpace[pp]);
    if (b == 0) break;
    sprintf(str, "%2u at %4u is %6u ", pag, pp, b);
    Serial.println (str);
    pag++;
    pp += SPM_PAGESIZE;
  }
}

uint16_t new16(uint16_t n) {
  // creates new 16 chunk starting from n, sets topPrime, returns last prime+2
  byte b = !0;
  byte chu16P;
  msgL(zNew, F("new 16 Start"), n);
  // find valid first:
  while (b != 0) {
    msgL(zNew, F("new 16 first"), n);
    b = checkP8_16(n);
    if (b != 0) {
      if (b == 2) {
        n = n + 1;
      } else {
        n = n + 2;
      }
    }
  } //while !0
  chu16P = 0;
  chunk.chu16.za16[chu16P++] = n;
  while (chu16P < 64) {
    n = n + 2;
    msgL(zNew, F("new 16 build"), n);
    b = checkP8_16(n);
    if (b == 0) {
      chunk.chu16.za16[chu16P++] = n;
      msgL(zNew, F("new Prime"), chu16P);
    }
  }  // 64
  topPrim = n;
  return n + 2;
}

void teachIn16(byte anz) {
  // generates and stores first anz chunks from topPrim
  uint16_t n = topPrim;
  if (n < 256) n = 257;
  char str[50];
  if (anz > NUMBER_OF_PAGES) {
    msgF(F(" teach in max "), NUMBER_OF_PAGES);
    return;
  }
  for (byte k = 1; k <= anz; k++) {
    if (zeig & zTeach) {
      sprintf(str, "teach #%2u start with %6u ", k, n);
      Serial.print(str);
    }
    n = new16(n);
    if (zeig & zTeach) showChu16();
    if (Serial.available() > 0) {
      msgF(F(" teach in aborted"), 0);
      return;
    }
    writePage(k);
  }
}

byte checkRange(uint64_t num) {
  // checks range 2*anz starting from num (should be odd) for primes
  // returns number of primes found, primes in chunk
  byte anz = 10;
  byte pp = 0;
  byte found = 0;
  uint16_t b;
  for (byte k = 1; k <= anz; k++) {
    if (zeig & zFact) {
      msg64(F("checkRange "), num);
    }
    b = checkP8_64(num);
    if (b == 0) {
      b = checkP16_64(num);
    }
    if (zeig & zFact) {
      msg64(F("checkRange b "), b);
    }
    if (b == 0)  {
      chunk.chu64.za64[pp++] = num;
      found++;
      if (pp > 15) {
        msgF(F("checkRange err: "), pp);
        return found;
      }
    } // prim found
    num += 2;
  } // k
  chunk.chu64.za64[pp] = 0;
  return found;
}

void generate(uint64_t num) {
  // generates and writes next chunk
  if (topPage > NUMBER_OF_PAGES) {
    msgF(F("Err topPage:"), topPage);
    runR = '8';
    return;
  }
  startTim = millis();
  new16(uint16_t(num));
  writePage(topPage++);
  endTim = millis();
  showTim();
  msgF(F("Generated, top Page"), topPage);
  runR = '0';
}



void ledOn(byte n) {
  digitalWrite(ledPin, HIGH);
  ledCnt = n;
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void provide32(char c, uint32_t num) {
  // puts c nu c to backBu, num readable
  backP = 0;
  char str[20];
  sprintf(str, "%c%lu%c", c, num, c);
  if (zeigClu > 0) {
    Serial.print (F("pro32 "));
    Serial.println (str);
  }
  for (byte k = 0; k < 20; k++) {
    backBuf[backP++] = str[k];
    if (str[k] == 0) return;
  }
  msgF(F("Err provide32 overrun"), backP);
}

void provide64(char c, uint64_t num) {
  // puts c num c to backBu, num binary
  backP = 0;
  backBuf[backP++] = c;
  zahl_t myz;
  myz.za64 = num;
  for (byte k = 0; k < 8; k++)  {
    backBuf[backP++] = myz.za08[k];
  }
  backBuf[backP++] = c;
}

void provideFld(char c) {
  byte  ug, og;
  switch (c) {
    case 'a':
      ug = 0;
      break;
    case 'b':
      ug = 32;
      break;
    case 'c':
      ug = 64;
      break;
    case 'd':
      ug = 96;
      break;
      break;
    default:
      ug = 0;
      msgF(F("ProvideFld fatal"), c);
  }
  og = ug + 32;
  backP = 0;
  for (byte k = ug; k < og; k++) {
    backBuf[backP++] = chunk.ramBuffer[k];
  }
}


void nummer() {
  //  transfers to slNum, assumes 1+8 bytes, no further checks
  byte p = 1;
  zahl_t num ;
  num.za64 = 0;
  for (byte k = 0; k < 8; k++) {
    num.za08[k] = recvBuf[p++];
  }
  slNum = num.za64;
  if (zeig & zDet) {
    zeigZahl64(num.za64);
  }
}

byte receiveHandle() {
  // char     Auftrag                         resp    Rem
  // A   *  clear flags           A           -       sets runS runR
  // Cx     slCmd                 Cx          -
  // E   *  provide exec time     E           Ec..E
  // Fx     provide field abcd                b8b8b8b8 a 0..3, b4..7 ...
  // G      generate from nummer  Gb8         -       writes to flash, inc topPage
  // I      isprim?               Ib8         runS C, runR 0 or 1
  // K      prim in range?        Kb8         b8b8b8...(0)  max 4*b8 runR is number of primes found
  // N      new from  nummer      Nb8         -       topPrim valid
  // P   *  provide top Prim      P           Pb8P
  // R      read page num         Rb8         -
  // S   *  provide Status                    runS runR slCmd
  // T      teach in              Tb8
  // Y      init to top prim      Yb8
  // V      set twi Adr nummer    Vb8         -
  char ch;
  ch = recvBuf[0];

  switch (ch) {
    case 'A':
      runS = 'I';
      runR  = VERSION;
      slCmd = ' ';
      break;
    case 'C':
      slCmd = recvBuf[1];
      break;
    case 'E':
      provide32(ch, elaTim);
      break;
    case 'F':
      provideFld(recvBuf[1]);
      break;
    case 'S':
      backP = 3;
      backBuf[0] = runS;
      backBuf[1] = runR;
      backBuf[2] = slCmd;
      break;

    case 'P':
      provide64(ch, topPrim);
      break;
    default:
      runS = 'B';
      slCmd = ch;
      nummer();
      break;
  } // case
  return 0;
}

void receiveEvent(int howMany) {
  ledOn(1);
  recvP = 0;
  if (howMany == 0) return;
  char c;
  while (0 < Wire.available())   {
    c = Wire.read();
    recvBuf[recvP++] = c;
    if (zeigClu) Serial.print(c);
  }
  if (zeigClu) msgF(F("<RecEv"), recvP);
  if (recvP > 0) {
    receiveHandle();
  }
}

void requestEvent() {
  for (byte k = 0; k < backP; k++) {
    Wire.write(backBuf[k]);
    if (zeigClu) Serial.print(backBuf[k]);
  }
  if (zeigClu) msgF(F("<ReQEv"), backP);
  ledOn(1);
}

void factorize(uint64_t num) {
  // put result in 64*2 byte chunk
  byte pp = 0;
  uint16_t res = !0;
  if (zeig & zFact) msg64(F("Fact "), num);
  // check 8 bit
  while (res != 0) {
    res = checkP8_64(num);
    if (res != 0) {
      if (zeig & zFact) msgF(F("Fac 8 = "), res);
      chunk.chu16.za16[pp++] = res;
      num = num / res;
    }
  }
  res = !0;
  while (res != 0) {
    res = checkP8_64(num);
    if (res != 0) {
      if (zeig & zFact) msgF(F("Fac 16 = "), res);
      chunk.chu16.za16[pp++] = res;
      num = num / res;
    }
  }
  if (zeig & zFact) msgF(F("Fac End = "), res);
  chunk.chu16.za16[pp++ ] = num;
  chunk.chu16.za16[pp++ ] = 0;
}

void showTim() {
  elaTim = endTim - startTim;
  Serial.print(F("Elapsed: "));
  Serial.println(elaTim);
  Serial.println();
}

void showInp() {
  zeigZahl64(inp);
}

void showJ() {
  char str[100],  strNum[25];
  Serial.println();
  sprintf(str, " runS %c, runR %c, slCmd %c.  topPage %u ", runS, runR, slCmd, topPage);
  Serial.print(str);
  format64(strNum, topPrim);
  sprintf(str, " topPrim %s", strNum);
  Serial.println(str);
  format64(strNum, slNum);
  sprintf(str, " slNum   %s", strNum);
  Serial.print(str);
  format64(strNum, inp);
  sprintf(str, "   inp     %s", strNum);
  Serial.println(str);
}

void showInfo() {
  char str[100];
  sprintf(str, "prim8: = %p\n", prim8);
  Serial.print(str);
  sprintf(str, "flashSpace[0]  = %p\n", &flashSpace[0]);
  Serial.print(str);
  sprintf(str, "flashSpace[max] = %p\n", &flashSpace[SPM_PAGESIZE * NUMBER_OF_PAGES - 1]);
  Serial.print(str);
  msgF(F("Chunks:"),  NUMBER_OF_PAGES);
  msgF(F("Flash space use:"), sizeof(flashSpace));
}

void anaFlash() {
  // analyzes flash, sets top when zeros found
  uint16_t adr;
  adr = firstZero16();
  msgF(F("First Zero at "), adr);
  if (adr == 0) {
    topPrim = 0;
    topPage = 1;
  } else {
    topPrim = pgm_read_word(&flashSpace[adr - 2]);
    topPage = adr / 128 + 1;
  }
  showJ();
}

void doInit(uint64_t top) {
  for (byte k = 0; k < SPM_PAGESIZE; k++) {
    chunk.ramBuffer[k] = 0;
  }
  writePage(1);
  anaFlash();
  if (top != 0) {
    topPrim = top;
    msg64(F("Set topPrim"), topPrim);
  }
}

void inpPush() {
  msg64(F("Push"), inp);
  if (inpSP >= inpSM) {
    msgF(F("inp Overflow"), inpSP);
  } else {
    inpStck[inpSP] = inp; // is inc'd on pop
    inpSP++;
  }
}

uint64_t inpPop() {
  if (inpSP > 0) {
    inpSP --;
    return inpStck[inpSP];
  } else {
    msgF(F("inpstack Underflow"), 0);
    return 0;
  }
}

void inp2Memo() {
  msgF(F("inp2Memo to"), inp);
  if (inp > memoM) {
    msgF(F("memo"), inp);
  } else {
    memo[inp] = inpPop();
    inp = memo[inp];
  }
}

void memo2Inp() {
  msgF(F("memo2Inp"), inp);
  if (inp > memoM) {
    msgF(F("memo "), inp);
  } else {
    inp = memo[inp];
  }
}

bool doCmdCalc(char ch) {
  uint64_t zwi;
  bool show = true;
  switch (ch) {
    case ',':   //
      inpPush();
      show = false;
      break;
    case '>':   //
      inp2Memo();
      break;
    case '~':   //
      zwi = inp;
      inp = slNum;
      slNum = zwi;
      showJ();
      msgF(F("Swap with slNum"), inp);
      break;
    case '<':   //
      memo2Inp();
      break;
    case '+':   //
      inp =  inpPop() + inp;
      break;
    case '-':   //
      inp = inpPop() - inp;
      break;
    case '*':   //
      inp = inpPop() * inp;
      break;
    case '/':   //
      inp = inpPop() / inp;
      break;
    case 13:   // CR to show
      break;
    default:
      return false;
      help();
  } // case
  if (show) showInp();
  return true;
}


void help () {
  Serial.println (F("Slave commands:"));
  Serial.println (F("Calcula:  ,(push) + - * /   ,m>(store)  m<(get)  ~(swap)  p(rim)"));
  Serial.println (F("PrimChk:  a 8,  b 16,  c 24,  d 32"));
  Serial.println (F("Chunk  :  r(ead),  w(rite),  s(how),  g(enerate),  t(each),  y(nit) "));
  Serial.println (F("Debug  :  Z z: Flow 1, New 2, Teach 4, Fact 8, Check 64, Det 128 "));
  Serial.println (F("Info   :  i I j J(anaflash)  l(index)"));
  Serial.println (F("Remote :  G(ener) R(ead) F(ield)  "));
}

void doCmd( char ch) {
  bool weg = false;
  uint8_t res8 = 0;
  uint16_t res16 = 0;
  Serial.print (ch);
  if ( ch == 8) { //backspace removes last digit
    weg = true;
    inp = inp / 10;
  }
  if ((ch >= '0') && (ch <= '9')) {
    weg = true;
    if (inpAkt) {
      inp = inp * 10 + (ch - '0');
    } else {
      inpAkt = true;
      inp = ch - '0';
    }
  }
  if (weg) {
    return;
  }
  inpAkt = false;
  Serial.print ("\b");
  switch (ch) {
    case 'a':   //
      startTim = millis();
      res8 = checkP8_64(inp);
      endTim = millis();
      msgF(F(" checkP8_64:"), res8);
      showTim();
      break;
    case 'b':   //
      startTim = millis();
      res16 = checkP16_64(inp);
      endTim = millis();
      msgF(F(" checkP16_64:"), res16);
      showTim();
      break;
    case 'c':   //  P24_
      startTim = millis();
      res16 = checkDivP16_64(inp);
      endTim = millis();
      msgF(F(" checkDIVP16_64:"), res16);
      showTim();
      break;
    case 'd':   //  P32_
      startTim = millis();
      res8 = checkP8_16(uint16_t(inp));
      endTim = millis();
      msgF(F(" checkP8_16:"), res8);
      showTim();
      break;
    case 'f':   //
      startTim = millis();
      factorize(inp);
      endTim = millis();
      showTim();
      showChu16();
      break;
    case 'g':   //
      generate(inp);
      break;
    case 'G':   //
      generate(slNum);
      break;
    case 'i':   //

      break;
    case 'I':   //
      showInfo();
      break;
    case 'j':   //
      anaFlash();
      break;
    case 'J':   //
      showJ();
      break;
    case 'k':
      startTim = millis();
      res8 = checkRange(inp);
      endTim = millis();
      msgF(F("Primes found"), res8);
      showTim();
      break;
    case 'K':   //
      msg64(F("Check Range"), slNum);
      startTim = millis();
      res8 = checkRange(slNum);
      endTim = millis();
      runR = res8 + '0'; // number of primes found
      msgF(F("Primes found"), res8);
      // transfer to buff
      showTim();
      break;
    case 'l':   //
      doIndex(inp);
      break;
    case 'n':   //
      break;
    case 'N':   //
      break;
    case 'p':   //
      if (inp < 10)  inp = primTst64[inp];
      zeigZahl64(inp);
      break;
    case 'r':   //
      readPage(inp);
      showChu16();
      break;
    case 'R':   //
      readPage(slNum);
      break;
    case 'w':   //
      writePage(inp);
      break;
    case 's':   //
      showChu16();
      break;
    case 'S':   //
      showChu64();
      break;
    case 'T':
    case 't':   //
      startTim = millis();
      if (ch == 'T') teachIn16(slNum); else teachIn16(inp);
      endTim = millis();
      showTim();
      break;
    case 'v':   //
    case 'V':   //
      if (ch == 'V') eeData.info.myAdr = slNum; else eeData.info.myAdr = inp;
      setEprom();
      setup();
      break;
    case 'x':   //
      break;
    case 'y':   //
      doInit(inp);
      break;
    case 'Y':   //
      doInit(slNum);
      break;
    case 'z':   //
      zeig = inp;
      msgF(F("Zeig is"), zeig);
      break;
    case 'Z':
      zeigClu = !zeigClu;
      msgF(F("ZeigClu"), zeigClu);
      break;
    default:
      if (!doCmdCalc(ch)) {
        msgF(F(" ?? "), ch);
        help();
      }
  } // case
}

void setup() {
  const char ich[] = "prim " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  pinMode(ledPin, OUTPUT);
  ledOn(1);
  getMyAdr();
  Wire.begin(myAdr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  anaFlash();
}

void loop() {
  char ch;
  if (Serial.available() > 0) {
    ch = Serial.read();
    doCmd(ch);
  } // avail

  if (slCmd != ' ') { // masters voice
    runR = '?'; // to store result
    msgF(F("slCmd ist "), slCmd);
    doCmd(slCmd);
    slCmd = ' ';
    runS = 'C';
  }

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
