// Majestro for Prime Concert
// Todo:
// RTC
// LED-Display
// remember last prime checked
// ...
#define ANZTSK 15  // tasks from 1
#include <cluCom.h>
#include <cluComPri.h>
byte target = 4;        //twi-adr of current slave
byte targetFS = 42;     //twi-adr of fileS
bool autoRun = false;   // true manage tasks
bool autoBegin = false; // true generate new games
bool fileMode = true;   // use fileS or flash
char mySrt = '2';       // not used
byte tarr = 0;          //task round robin
byte recvP = 0;
char recvBuf[32];
byte zeigClu = 0;
long primCnt;           // primes found
uint16_t loopCnt;       // loops
uint16_t loopMsg = 60;  // print status after ticks
// zeigs:
const byte zFlow = 1;   // general flow info
const byte zTab = 2;    // slaves and tasks
const byte zTeach = 4;  //
const byte zFact = 8;   //
const byte zCheck = 64;
const byte zDet = 128;    //Detail

uint64_t inp;
uint64_t topPrim = 0;   // not used (highest prim of all slaves)
uint64_t myPrim = 0;    // next range to send
bool inpAkt;
typedef struct { // tasks
  byte tskStat;     // Waiting Running Complete Killed
  uint64_t tskZahl;
  char tskSrt;      // 2,3,4..corresponds to slv
  byte tskSlv;      // logical Slave
  char tskRunS;     // its State
  char tskRunR;     // its Result
} tskInfoT;
tskInfoT tskInfo[ANZTSK];

void scanne() {
  byte error, address;
  const byte bis = 42;
  msgF(F("Scanne bis "), bis);
  for (address = 1; address <= bis; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16)  Serial.print("0");
      Serial.println(address, HEX);
    }
    else if (error == 4) {
      Serial.print(F("Unknown error at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
}

void request(byte anz) {
  msgZ(5, F("Request from "), target);
  Wire.requestFrom(target, anz);
  recvP = 0;
  char c;
  while (0 < Wire.available())   {
    c = Wire.read();
    recvBuf[recvP++] = c;
    if (zeigClu) Serial.write(c);
  }
  if (zeigClu) {
    Serial.print(F("<Recv "));
    Serial.println(recvP);
  }
}

byte sende(const char txt[]) {
  byte res;
  Wire.beginTransmission(target); //
  Wire.write(txt);
  res = Wire.endTransmission();  //
  if (res == 0) {
    msgZ(5, F("Sende is"), res);
  } else {
    msgZ(1, F("Err: Sende is"), res);
  }
  /*  0:success,  1:data too long to fit , 2:received NACK on transmit of address
    3:received NACK on transmit of data,  4:other error   5:mein error  */
  return res;
}

byte sendeC(char c) {
  byte res;
  Wire.beginTransmission(target); //
  Wire.write(c);
  res = Wire.endTransmission();  //
  if (res == 0) {
    msgZ(5, F("SendeC is"), res);
  } else {
    msgZ(1, F("Err: SendeC is"), res);
  }
  return res;
}

byte sendeZahl(byte c, uint64_t num) {
  byte res;
  zahl_t myz;
  myz.za64 = num;
  Wire.beginTransmission(target); //
  Wire.write(c);
  for (byte k = 0; k < 8; k++)  {
    Wire.write(myz.za08[k]);
  }
  res = Wire.endTransmission();  //
  if (res == 0) {
    msgZ(5, F("SendeZahl is"), res);
  } else {
    msgZ(1, F("Err: SendeZahl is"), res);
  }
  return res;
}

byte prepQuery(char c) {
  byte res;
  msgZ(5, F("Query adr"), target);
  res = sendeC(c);
  if (res != 0) {
    recvBuf[0] = '?';
    recvBuf[1] = '?';
  }
  return res;
}

void holQuery(byte anz) {
  Wire.requestFrom(target, anz);
  recvP = 0;
  char c;
  if (zeigClu) Serial.print('>');
  while (0 < Wire.available())   {   //
    c = Wire.read();
    recvBuf[recvP++] = c;
    if (zeigClu) Serial.print(c);
  }
  if (zeigClu) {
    msgF(F("<qu Resp anz="), recvP);
  }
}

byte queryStat(byte myTarg) {
  byte anz = 3;
  byte res;
  byte targetS = target;
  if (myTarg != 0)  target = myTarg;
  res = prepQuery('S');
  if (res == 0) {
    holQuery(anz);
  }
  target = targetS;
  return res;
}

uint64_t queryTopPrim() {
  byte anz = 10; //  Pb8P
  byte res;
  res = prepQuery('P');
  if (res != 0) {
    return 0;
  }
  holQuery(anz);
  byte p = 1;
  zahl_t num ;
  num.za64 = 0;
  for (byte k = 0; k < 8; k++) {
    num.za08[k] = recvBuf[p++];
  }
  if (zeigClu) {
    msg64(F("Top ="), num.za64);
  }
  return num.za64;
}

uint32_t queryEla() {
  byte anz = 12; // or so
  byte res;
  uint32_t myTim = 0;
  char c;
  res = prepQuery('E');
  if (res != 0) {
    return 0;
  }
  holQuery(anz);
  byte k = 1;
  while (true) {
    c = recvBuf[k++];
    if ((c >= '0') && (c <= '9')) {
      myTim = myTim * 10L + (c - '0');
    } else {
      return myTim;
    }
  }
  return 0;
}


bool checkFS() {
  // wait until target not busy, returns true if ready
  char str[20];
  byte cnt = 10;
  while (cnt > 0) {
    cnt--;
    queryStat(0);
    if (recvBuf[0] == 'B') {
      if (cnt < 9) {
        sprintf(str, "Check %2d %c %c", target, recvBuf[0], recvBuf[1]);
        Serial.println(str);
      }
      delay(3);
    } else {
      if (recvBuf[0] == 'E') {
        sprintf(str, "Check Error autorun stop %2d %c %c", target, recvBuf[0], recvBuf[1]);
        Serial.println(str);
        autoRun = false;
      }
      return true;
    }
  } // while
  return false;
}

void addprime() {
  // extract from chunk and send to fileS
  uint64_t num;
  byte targetS = target;
  target = targetFS;
  for (byte k = 0; k < 3; k++) {
    num = chunk.chu64.za64[k];
    if (num == 0) break;
    checkFS();
    sendeZahl('L', num);
    primCnt++;
    if (zeig & zFlow) msg64(F("addPrime"), num);
  }
  target = targetS;
}

byte holeFld(char c) {
  // transfer primes to chunk 64 to 32 byte
  char str[50];
  byte res;
  str[0] = 'F';
  str[1] = c;
  str[2] = 0;
  res = sende(str);
  if (res != 0) return res;
  request(32);
  for (byte k = 0; k < 32; k++) {
    chunk.ramBuffer[k] = recvBuf[k];
  }
  addprime();
  return 0;
}


void uhrTim() {
  //converts ms to hhmmss 60s/m 60m/h
  uint32_t tim;
  uint16_t hr, mi, se;
  char str[100];
  tim = millis() / 1000L;
  hr = tim / 3600L;
  tim = tim - hr * 3600;
  mi = tim / 60;
  se = tim - mi * 60;
  sprintf(str, "%02u:%02u:%02u Primes %lu ", hr, mi, se, primCnt);
  Serial.println(str);
}

void info() {
  msg64(F("topPrim: "), topPrim);
  msg64(F("myPrim : "), myPrim);
  msgF(F("Target:"), target);
  msgF(F("tick (ms):"), tick);
  msgF(F("loopCnt:"), loopCnt);
  msgF(F("loopMsg:"), loopMsg);
  msgF(F("fileMode:"), fileMode);
}


void sendCmd(byte b) {
  char c = ' ';
  char txt[5];
  switch (b) {
    case 0:   //
      c = 'X';
      break;
    case 1:   //  execute
      c = 'x';
      break;
    case 2:   // show field
      c = 'f';
      break;
    case 3:   // show lev
      c = 'l';
      break;
    case 4:   // sort
      c = 'a';
      break;
    case 5:   // sort
      c = 'u';
      break;
    case 6:   // log game tbd
      c = 'R';
      break;
    case 8:   // set idle
      c = 'I';
      break;
    case 9:   // abort
      c = 'X';
      break;
    default:
      msgF(F("Err sendCmd Unknown ?? "), b);
      return;
  }
  txt[0] = 'C';
  txt[1] = c;
  txt[2] = 0;
  sende(txt);
}

byte freeSlave() {
  // returns idle slave
  for (byte k = 1; k < ANZSLA; k++) {
    if (slaInfo[k].slaAdr != 0) {
      byte st = slaInfo[k].slaRunS;
      if (st == 'I') {
        return k;
      }
    }
  }
  return ANZSLA + 1;
}


void getSlaves() {
  byte adr;
  byte err;
  for (byte k = 1; k < ANZSLA; k++) {
    adr = slaInfo[k].slaAdr;
    slaInfo[k].slaRunS = ' ';
    slaInfo[k].slaRunR = ' ';
    if ((adr > 0) && (adr < 128) ) {
      slaInfo[k].slaRunS = '?';
      err = queryStat(adr);
      if (err == 0) {
        slaInfo[k].slaRunS = recvBuf[0];
        slaInfo[k].slaRunR = recvBuf[1];
      }
    } // adr
  } // for
}

// only use for fs, should use sendeZahl as this is slooow
void sendeNummer(char c, uint32_t i) {
  char str[15];
  sprintf(str, "%c%lu%c", c, i, c);
  sende(str);
}

void openFile(uint16_t i) {
  byte targS = target;
  target = targetFS;
  sendeNummer('O', i);
  mySrt = '2';
  checkFS();
  target = targS;
}

void showTasks() {
  char str[100], strZ[50];
  Serial.println();
  Serial.println(F("    St  Slv Srt   Num"));
  for (byte k = 1; k < ANZTSK; k++) {
    format64(strZ, tskInfo[k].tskZahl);
    sprintf(str, "%2d  %c   %2d  %c  %c %c %s", k, tskInfo[k].tskStat, tskInfo[k].tskSlv, tskInfo[k].tskSrt, tskInfo[k].tskRunR, tskInfo[k].tskRunS, strZ);
    Serial.println(str);
  }
}

byte freeTask() {
  for (byte k = 1; k < ANZTSK; k++) {
    byte st = tskInfo[k].tskStat;
    if ((st != 'W') && (st != 'R')) return k;
  }
  return ANZTSK + 1;
}

byte newTask(uint64_t num, char srt) {
  // returns 0 if sux or 2 of no tasks avail
  byte t = freeTask();
  if (t > ANZTSK) {
    return 2;
  }
  if (num == 0) num = topPrim;
  tskInfo[t].tskZahl = num;
  tskInfo[t].tskSrt = srt;
  tskInfo[t].tskStat = 'W';
  tskInfo[t].tskSlv = 0;
  tskInfo[t].tskRunR = ' ';
  tskInfo[t].tskRunS = ' ';
  return 0;
}

void killTask(byte t) {
  tskInfo[t].tskStat = 'K';
}

void killAllTasks() {
  for (byte k = 1; k < ANZTSK; k++) {
    tskInfo[k].tskStat = 'K';
  }
}

void getInfo16() {
  uint64_t top;
  topPrim = 0;
  // for all typ 2 sample
  for (byte sl = 1; sl < ANZSLA; sl++) {
    if (slaInfo[sl].slaTyp == 2) {
      target = slaInfo[sl].slaAdr;
      if (target != 0) {
        Serial.print(F("Target "));
        Serial.print(target);
        top = queryTopPrim();
        msg64(F("  Top prim"), top);
        if (top > topPrim) topPrim = top;
      } // targ
    } //2
  } //sl
  msg64(F("topPrim after"), topPrim);
}

/*
  void teachIn16(uint64_t anz) {
  msg64(F("Teach in 16 top ="), topPrim);
  for (byte i = 0; i < anz; i++) {
    // list of active slaves state 2
    for (byte sl = 1; sl < ANZSLA; sl++) {
      if (slaInfo[sl].slaTyp == 2) {
        if (slaInfo[sl].slaAdr != 0) {
          target = slaInfo[sl].slaAdr;
          msgF(F("Target "), target);
          sendeZahl('G', topPrim + 2);
          if (checkFS()) {   //wait for Completion
            topPrim = queryTopPrim();
          } else {
            msgF(F("teachIn Error Fatal"), 0);
            return;
          }
          msgF(F("topPrim after"), topPrim);
        } // adr
      } //2
    } //sl
  } //i
  }
*/
void startTask(byte ta, byte sl) {
  char str[50], strZ[50];
  if (zeig & zFlow) {
    format64(strZ, tskInfo[ta].tskZahl);
    sprintf(str, "starte task %2d on %2d  Srt %c  %s", ta, sl, tskInfo[ta].tskSrt, strZ);
    Serial.println(str);
  }
  tskInfo[ta].tskStat = 'R';
  tskInfo[ta].tskRunS = ' ';
  tskInfo[ta].tskSlv = sl;
  slaInfo[sl].slaTask = ta;
  slaInfo[sl].slaRunS = 'R';
  // start
  byte targetS = target;
  target = slaInfo[sl].slaAdr;
  sendeZahl('K', tskInfo[ta].tskZahl);
  /*checkFS(); fire & forget
    sendeZahl('T', 30);  // teach 30
    if (tskInfo[ta].tskSrt == 'a') {
      sendCmd(4);
    } else {
      sendCmd(5);
    }
  */
  target = targetS;
}

void checkSlaves() {
  // for completed tasks
  char str[50];
  byte targetS = target;
  uint32_t ela;
  getSlaves();
  for (byte sl = 1; sl < ANZSLA; sl++) {
    if (slaInfo[sl].slaRunS == 'C') { //book completed task
      byte ta = slaInfo[sl].slaTask;
      target = slaInfo[sl].slaAdr;
      //ela = queryEla();
      ela = 0;
      if ((slaInfo[sl].slaRunR > '0') && (slaInfo[sl].slaRunR < '5')) { // have to fetch and book primes
        if (zeig & zFlow)  Serial.println(F("HoleFld"));
        holeFld('a');
      }
      if (zeig & zFlow) {
        sprintf(str, "checkSlave  %2u sta %c %c ela %lu", target, slaInfo[sl].slaRunS, slaInfo[sl].slaRunR, ela); //max 32
        Serial.println(str);
      }
      slaInfo[sl].slaRunS = 'I';
      sendeC('A'); //merci vielmals
      tskInfo[ta].tskStat = 'C'; // other info kept
    }
  } // loop sl
  target = targetS;
}

void assignTask() {
  byte sl;
  byte cnt = ANZTSK;
  while (cnt > 0) {
    cnt--;
    tarr++;
    if (tarr >= ANZTSK) tarr = 1;
    if (tskInfo[tarr].tskStat == 'W') {
      // find free slave suited
      sl = freeSlave();
      if (sl > ANZSLA) {
        if (zeig & zFlow) msgF(F("no slaves "), sl);
        return;
      }
      startTask(tarr, sl);
    } // waiting
  } // loop cnt
  if (zeig & zFlow) msgF(F("no tasks waiting "), tarr);
}

void doBegin(uint64_t num) {
  // adds task and incs myPrim. continues if num 0
  byte res;
  if (num != 0) { //called first time
    myPrim = num;
    primCnt = 0;
  }
  if (zeig & zFlow) msgZ (5, F("doBegin"), myPrim);
  if (!autoBegin) {
    msgF(F("Autobegin false"), autoBegin);
    return;
  }
  while (true) {
    res = newTask(myPrim, mySrt);
    if (res != 0) {
      if (res == 1) { //error
        autoBegin = false;
        msgF(F("doBegin Games over"), res);
      }
      return;
    }
    myPrim += 20;
  }
}

void setFreq(byte b) {
  unsigned long freq;
  switch (b) {
    case 0:   //
      freq = 100000L;
      break;
    case 1:   //
      freq = 400000L;
      break;
    case 2:   //
      freq = 600000L;
      break;
    case 3:   //
      freq = 33333L;
      break;
    case 9:   //
      freq = 1000000L; // geht nicht
      break;
    default:
      msgF(F("setFreq Unknown ?? "), b);
      return;
  }
  msgF(F("Freq to "), b);
  Wire.setClock(freq);
  msgF(F("TWBR:"), TWBR);
  msgF(F("TWSR:"), TWSR & 3);
  /*
    TWSR 0..3 = 1,4,16,64
    SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
    note: TWBR should be 10 or higher for master mode
    It is 72 for a 16mhz  with 100kHz TWI */
}

void help () {
  Serial.println (F("Master commands:"));
  Serial.println (F("Tasks  :  a(ssign), b(egin), g(o teachIn), k(ill), l(ist),  n(ew), N(ew), U(nter)"));
  Serial.println (F("Queries:  q(uery), e(elapsed), G(topPrim) "));
  Serial.println (F("Sends  :  c(ommand), h(new), f(ield) r(ead) s "));
  Serial.println (F("Slaves :  d(etect), A(nf), E(set freq), m M p P, O,o(verview), t(arget),, V(set twi adr)"));
  Serial.println (F("Debug  :  Z, z: zFlow 1, zNew 2, zTeach 4, zCheck 64, zDet 128 "));
  Serial.println (F("Info   :  i I j s u(tick)"));
  Serial.println (F("Remote :  G(ener) R(ead) F(ield)  T(each) W(init) Y(openfile)  y(lognumber)"));
}

void doCmd( char tmp) {
  char str[50];
  bool weg = false;
  Serial.print (tmp);
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
    return;
  }
  inpAkt = false;
  Serial.print("\b");
  switch (tmp) {
    case 'a':   //
      msgF(F("assign"), 0);
      assignTask();
      break;
    case 'A':   // clear flags
      sendeC('A');
      break;
    case 'b':   //
      autoBegin = true;
      doBegin(inp);
      showTasks();
      break;
    case 'c':   //
      sendCmd(inp);
      break;
    case 'd':   //
      scanne();
      break;
    case 'e':   //
      sprintf(str, "Ela %lu", queryEla());
      Serial.println(str);
      break;
    case 'E':   //
      setFreq(inp);
      break;
    case 'f':   //
      holeFld('a');
      showChu64();
      break;
    case 'F':   //
      fileMode = !fileMode;
      msgF(F("fileMode is"), fileMode);
      break;
    case 'g':   //
      sendeZahl('F', inp);
      break;
    case 'G':   //
      queryTopPrim();
      break;
    case 'h':   //
      sendeZahl('G', inp);
      break;
    case 'i':   //
      info();
      break;
    case 'j':   //
      getInfo16();
      break;
    case 'k':   //
      killTask(inp);
      showTasks();
      break;
    case 'K':   //
      killAllTasks();
      showTasks();
      break;
    case 'l':   //
      showTasks();
      break;
    case 'm':   //
      slaInfo[slaSel].slaAdr = inp;
      showSlaves();
      break;
    case 'M':   //
      slaInfo[slaSel].slaAdr = inp;
      eeData.info.slaAdr[slaSel] = inp;
      setEprom();
      showSlaves();
      break;
    case 'n':   //
      msgF(F("New Task is"), newTask(inp, '2'));
      showTasks();
      break;
    case 'N':   //
      newTask(255ULL, '2');
      newTask(17125ULL, '2');
      newTask(30133ULL, '2');
      showTasks();
      break;
    case 'o':   //
      getSlaves();
      showSlaves();
      break;
    case 'O':   //
      slaAkt = !slaAkt;
      showSlaves();
      break;
    case 'p':   //
      slaSel = inp;
      showSlaves();
      if (slaInfo[slaSel].slaAdr != 0) {
        target = slaInfo[slaSel].slaAdr;
        msgF(F("Target "), target);
      }
      break;
    case 'P':   //
      slaInfo[slaSel].slaTyp = inp;
      eeData.info.slaTyp[slaSel] = inp;
      setEprom();
      showSlaves();
      break;
    case 'q':   //
      msgF(F("Query is"), queryStat(0));
      break;
    case 'r':   // read
      sendeZahl('R', inp);
      break;
    case 's':   //
      showChu16();
      break;
    case 't':   //
      target = inp;
      msgF(F("Target"), target);
      break;
    case 'T':   //
      msg64(F("Teach-in"), inp);
      sendeZahl('T', inp);
      break;
    case 'u':   //
      tick = inp;
      msgF(F("Tick is"), tick);
      break;
    case 'U':   //
      loopMsg = inp;
      msgF(F("LoopMsg after"), loopMsg);
      break;
    case 'v':   //
      msgF(F("verify "), 0);
      checkSlaves();
      break;
    case 'V':   //
      msgF(F("TargetTWI Adr "), inp);
      sendeZahl('V', inp);
      break;
    case 'W':   //
      msg64(F("Init Slave"), inp);
      sendeZahl('Y', inp);
      break;
    case 'X':
    case 'x':   //
      autoRun = !autoRun;
      autoBegin = true;
      msgF(F("autoRun is"), autoRun);
      vt100Mode = (tmp == 'X');
      if (vt100Mode) {
        vt100Home();
        vt100ClrEos();
      }
      break;
    case 'y':   //
      target = targetFS;
      checkFS();
      sendeNummer('L', inp);
      checkFS();
      break;
    case 'Y':   //
      openFile(inp);
      break;
    case 'z':   //
      zeig = inp;
      msgF(F("Zeig"), zeig);
      break;
    case 'Z':
      zeigClu = !zeigClu;
      msgF(F("ZeigClu"), zeigClu);
      break;
    default:
      msgF(F("?? "), tmp);
      help();
  } // case
}


void ledOn(byte n) {
  digitalWrite(ledPin, HIGH);
  ledCnt = n;
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void setup() {
  const char ich[] = "priMaster " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  Wire.begin(); // (address optional for master)
  setFreq(1);
  pinMode(ledPin, OUTPUT);
  ledOn(1);
  getEprom();
  for (byte k = 1; k < ANZSLA; k++) {
    if (eeData.info.slaAdr[k] < 127) {
      slaInfo[k].slaAdr = eeData.info.slaAdr[k];
      if ( eeData.info.slaTyp[k] > 127) {
        eeData.info.slaTyp[k] = 0;
      }
      slaInfo[k].slaTyp = eeData.info.slaTyp[k];
    }
  }
}

void loop() {
  char tmp;
  if (Serial.available() > 0) {
    tmp = Serial.read();
    doCmd(tmp);
  } // avail

  currTim = millis();
  if (nexTim < currTim) {
    nexTim = currTim + tick;
    loopCnt++;
    if (loopCnt => loopMsg) {
      loopCnt = 0;
      uhrTim();
    }
    if (autoRun) {
      checkSlaves();
      if (vt100Mode) {
        showSlaves();
        showTasks();
        vt100ClrEos();
      }
      doBegin(0);
      assignTask();
    } // auto
    if (ledCnt == 0) {
      ledOff();
    } else {
      ledCnt--;
    }
  } // tick
}
