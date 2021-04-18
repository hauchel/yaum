#define ANZTSK 11  // tasks from 1
#include <cluCom.h>
#include <cluComSud.h>

#define FILES
#ifdef VIEL
#include  <cluGamKudoko.h>
#else
#define ANZGAM  9
const static unsigned char PROGMEM games [ANZGAM][ANZFLD] = {
  {"................................................................................."}, // 0 leer
  {"..654873.1876395245.3.7196.6759241.343.18.6522183.549785.41.37.79.8532463.47.2815"}, // 1 einfach
  {"3.........9..2..5.....3.276..928.5.....3....47.3.648..172...6.5.4.....8....5.9..2"}, // 2 einfach
  {".7..1..8....6.24..............4.32...81..................58..7.2...7....3........"}, // 3 Elap 6813
  {"......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3."}, // 4 Elap 185558
  {"..654874.1876395245.3.7196.6759241.343.18.6522183.549785.41.37.79.8532463.47.2815"}, // 5 einfach err
  {"3......4..9..2..5.....3.276..928.5.....3....47.3.648..172...6.5.4.....8....5.9..2"}, // 6 einfach err
  {".7..1..8....6.24..............4.32...81..................58..7.2...7....3........"}, // 7 Elap 6813
  {"......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3."}, // 8 Elap 185558
};
#endif

byte target = 4;        //twi-adr of current slave
byte targetFS = 42;     //twi-adr of fileS
bool autoRun = false;   // true manage tasks
bool autoBegin = false;  // true generate new games
bool fileMode = true;   // use fileS or flash
uint16_t myGam = 1;
char mySrt = 'a';
uint16_t anzGam = ANZGAM;  // might change if from fileS
byte tarr = 0;          //task round robin
byte recvP = 0;
char recvBuf[32];
byte zeigClu = 0;

typedef struct {
  byte tskStat;     // Waiting Running Complete Killed
  uint16_t tskGam;
  char tskSrt;
  byte tskSlv;      // logical Slave
  char tskRunS;     // its State
  char tskRunR;     // its Result
} tskInfoT;
tskInfoT tskInfo[ANZTSK];

void getGame (byte gam) {
  byte b;
  byte f = 0;
  msgZ(5, F("Game"), gam);
  for (byte k = 0; k < 81; k++) {
    b = pgm_read_byte(&games[gam][k]);
    Serial.print(char(b));
    f++;
    if (b == '.') {
      fld[f] = 0;
    } else {
      fld[f] = b - '0';
    }
  }
  Serial.println();
}

void ledOn(byte n) {
  digitalWrite(ledPin, HIGH);
  ledCnt = n;
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void scanne() {
  byte error, address;
  msgF(F("Scanne bis "), 49);
  for (address = 1; address < 50; address++ ) {
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
    if (zeig > 2) Serial.write(c);
  }
  if (zeig > 2) {
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
  /*
    0:success
    1:data too long to fit in transmit buffer
    2:received NACK on transmit of address
    3:received NACK on transmit of data
    4:other error
    5:mein error
  */
  ledOn(1);
  return res;
}

byte query(byte myTarg) {
  byte anz = 3;
  byte res;
  byte targetS = target;
  if (myTarg != 0)  target = myTarg;
  msgZ(5, F("Query adr"), target);
  res = sende("S");
  if (res != 0) {
    recvBuf[0] = '?';
    recvBuf[1] = '?';
    target = targetS;
    return res;
  }
  Wire.requestFrom(target, anz);
  recvP = 0;
  char c;
  while (0 < Wire.available())   {   //
    c = Wire.read();
    recvBuf[recvP++] = c;
    if (zeig > 2) Serial.print(c);
  }
  if (zeig > 5) {
    msgF(F("<query anz="), recvP);
  } else {
    if (zeig > 2) Serial.println('<');
  }
  target = targetS;
  return 0;
}

uint32_t queryEla() {
  byte anz = 12; // or so
  byte res;
  uint32_t myTim = 0;
  char c;
  res = sende("T");
  if (res != 0) {
    recvBuf[0] = '?';
    recvBuf[1] = '?';
    return 0;
  }
  recvP = 0;
  Wire.requestFrom(target, anz);
  while (0 < Wire.available())   {   //
    c = Wire.read();
    recvBuf[recvP++] = c;
  }
  if (zeig > 5) {
    msgF(F("<query Ela anz="), recvP);
  }
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

byte sendFld1(char c) {
  // 1  Xa   1..30
  // 2  Xb  31..60
  // 3  Xc  61..
  byte ug, og;
  ug = getUG(c);
  og = getOG(c);
  byte res;
  Wire.beginTransmission(target); //
  Wire.write('X');
  Wire.write(c);
  for (byte k = ug; k <= og; k++) {
    Wire.write(fld[k] + '0');
  }
  res = Wire.endTransmission();
  msgZ(5, F("sendFld1 is "), res);
  ledOn(1);
  return res;
}

byte sendFld() {
  byte res;
  res = sendFld1('a');
  if (!res) {
    res = sendFld1('b');
    if (!res) {
      res = sendFld1('c');
    }
  }
  return res;
}

byte storeFld(char c) {
  // a  Xa   1..30
  // b  Xb  31..60
  // c  Xc  61..
  byte  ug, og;
  ug = getUG(c);
  og = getOG(c);
  byte f = 0;
  for (byte k = ug; k <= og; k++) {
    fld[k] = recvBuf[f++] - '0';
  }
  return 0;
}

byte holeFld() {
  byte res;
  res = sende("Fa");
  if (!res) {
    request(30);
    storeFld('a');
    res = sende("Fb");
    if (!res) {
      request(30);
      storeFld('b');
      res = sende("Fc");
      if (!res) {
        request(21);
        storeFld('c');
      }
    }
  }
  return res;
}


void info() {
  byte b;
  msgF(F("myGam"), myGam);
  msgF(F("mySrt"), mySrt);
  msgF(F("tarr"), tarr);
  msgF(F("anzGam"), anzGam);
  msgF(F("Target"), target);
  msgF(F("fileMode"), fileMode);
  msgF(F("TWBR"), TWBR);
  b = TWSR & 3;
  msgF(F("TWSR"), b);
  /*
    TWSR 0..3 = 1,4,16,64
    SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
    note: TWBR should be 10 or higher for master mode
    It is 72 for a 16mhz  with 100kHz TWI */
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
  query(0);
}

byte freeSlave() {
  for (byte k = 1; k < ANZSLA; k++) {
    byte st = slaInfo[k].slaRunS;
    if (st == 'I') {
      return k;
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
      err = query(adr);
      if (err == 0) {
        slaInfo[k].slaRunS = recvBuf[0];
        slaInfo[k].slaRunR = recvBuf[1];
      }
    } // adr
  } // for
}

void sendNummer(char c, uint16_t i) {
  char str[15];
  sprintf(str, "%c%u%c", c, i, c);
  sende(str);
}

bool checkFS() {
  // target must be FS
  //char str[20];
  byte cnt = 10;
  delay(10); // slave needs time to at least process first chars of prev request
  while (cnt > 0) {
    cnt--;
    query(0);
    //sprintf(str, "Check %2d %c %c", recvP, recvBuf[0], recvBuf[1]);
    //Serial.println(str);
    if (recvBuf[0] == 'B') {
      delay(10);
    } else {
      if (recvBuf[0] == 'E') {
        Serial.println(F("CheckFS Error"));
        autoRun = false;
      }
      return true;
    }
  } // while
  return false;
}

void openFile(uint16_t i) {
  byte targS = target;
  target = targetFS;
  sendNummer('O', i);
  anzGam = 9999; // or so
  myGam = 0;
  mySrt = 'a';
  checkFS();
  target = targS;
}

void readGame(uint16_t i) {
  byte targS = target;
  target = targetFS;
  sendNummer('G', i);
  checkFS();
  holeFld();
  target = targS;
}

void showTasks() {
  char str[50];
  Serial.println();
  Serial.println(F("    St  Slv  Gam Srt S R"));
  for (byte k = 1; k < ANZTSK; k++) {
    sprintf(str, "%2d  %c   %2d  %4d %c  %c %c", k, tskInfo[k].tskStat, tskInfo[k].tskSlv, tskInfo[k].tskGam, tskInfo[k].tskSrt, tskInfo[k].tskRunR, tskInfo[k].tskRunS);
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

byte newTask(uint16_t gam, char srt) {
  if (gam >= anzGam) {
    msgF(F("newTask game over "), gam);
    return 1;
  }
  byte t = freeTask();
  if (t > ANZTSK) {
    msgF(F("newTask no tasks"), t);
    return 2;
  }
  tskInfo[t].tskGam = gam;
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

void startTask(byte ta, byte sl) {
  char str[50];
  uint16_t gam = tskInfo[ta].tskGam;
  sprintf(str, "starte task %2d on %2d  Game %4d Srt %c", ta, sl, gam, tskInfo[ta].tskSrt);
  Serial.println(str);
  tskInfo[ta].tskStat = 'R';
  tskInfo[ta].tskRunS = ' ';
  tskInfo[ta].tskSlv = sl;
  slaInfo[sl].slaTask = ta;
  slaInfo[sl].slaRunS = 'R';
  //fetch game, transfer and start
  target = slaInfo[sl].slaAdr;
  if (fileMode) {
    readGame(gam);
  } else {
    getGame(gam);
  }
  sendFld();
  if (tskInfo[ta].tskSrt == 'a') {
    sendCmd(4);
  } else {
    sendCmd(5);
  }
  sendCmd(1);
}

void checkSlaves() {
  char str[50];
  byte targetS = target;
  uint32_t ela;
  getSlaves();
  for (byte sl = 1; sl < ANZSLA; sl++) {
    if (slaInfo[sl].slaRunS == 'C') { //book completed task
      byte ta = slaInfo[sl].slaTask;
      target = slaInfo[sl].slaAdr;
      ela = queryEla();
      holeFld();
      sprintf(str, "L %u %c sta %c ela %lu", tskInfo[ta].tskGam , tskInfo[ta].tskSrt, slaInfo[sl].slaRunR, ela); //max 32
      Serial.println(str);
      sendCmd(8); //slave idle
      tskInfo[ta].tskStat = 'C'; // other info kept
      target = targetFS;    // log data
      checkFS();
      sende(str);
      checkFS();
      sendFld();
      checkFS();
      sende("R");
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
      // scan for free slaves
      sl = freeSlave();
      if (sl > ANZSLA) {
        msgF(F("no slaves "), sl);
        return;
      }
      startTask(tarr, sl);
    } // waiting
  } // loop cnt
  msgF(F("no tasks waiting "), tarr);
}

void doBegin() {
  // adds task toggles srt and incs myGam
  byte res;
  msgZ (5, F("Begin Gam"), myGam);
  if (!autoBegin) {
    msgF(F("Autobegin false"), autoBegin);
    return;
  }
  while (true) {
    res = newTask(myGam, mySrt);
    if (res != 0) {
      if (res == 1) {
        autoBegin = false;
        msgF(F("doBegin Games over"), res);
      }
      return;
    }
    if (mySrt == 'a') {
      mySrt = 'u';
    } else {
      mySrt = 'a';
      myGam++;
    }
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
  info();
}

void doCmd( char tmp) {
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
    Serial.print(F("\b\b\b\b"));
    Serial.print(inp);
    return;
  }
  inpAkt = false;

  switch (tmp) {
    case 'a':   //
      assignTask();
      break;
    case 'A':   //
      anzGam = inp;
      msgF(F("anzGam is"), anzGam);
      break;
    case 'b':   //
      doBegin();
      showTasks();
      break;
    case 'c':   //
      sendCmd(inp);
      break;
    case 'd':   //
      scanne();
      break;
    case 'e':   //
      queryEla();
      break;
    case 'E':   //
      setFreq(inp);
      break;
    case 'f':   //
      showFld();
      break;
    case 'F':   //
      fileMode = !fileMode;
      msgF(F("fileMode is"), fileMode);
      break;
    case 'g':   //
      getGame(inp);
      break;
    case 'G':   //
      myGam = inp;
      msgF(F("myGam is"), myGam);
      break;
    case 'h':   //
      holeFld();
      break;
    case 'i':   //
      info();
      break;
    case 'j':   //
      sendFld();
      break;
    case 'k':   //
      killTask(inp);
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
      newTask(inp, mySrt);
      showTasks();
      break;
    case 'o':   //
      getSlaves();
      showSlaves();
      break;
    case 'p':   //
      slaSel = inp;
      showSlaves();
      break;
    case 'q':   //
      msgF(F("Query is"), query(0));
      break;
    case 'r':   //
      request(inp);
      break;
    case 's':   //
      sende("huhu");
      break;
    case 't':   //
      target = inp;
      msgF(F("Target"), target);
      break;
    case 'T':   //
      tick = inp;
      msgF(F("Tick is"), tick);
      break;
    case 'v':   //
      msgF(F("verify "), 0);
      checkSlaves();
      break;
    case 'V':   //
      msgF(F("TargetTWI Adr "), inp);
      sendNummer('V', inp);
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
      readGame(inp);
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
  } // case
}

void setup() {
  const char ich[] = "sudMaster " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  Wire.begin(); // join i2c bus (address optional for master)
  setFreq(1);
  pinMode(ledPin, OUTPUT);
  ledOn(1);
  getEprom();
  for (byte k = 1; k <ANZSLA; k++) {
    if (eeData.info.slaAdr[k] < 127) {
      slaInfo[k].slaAdr = eeData.info.slaAdr[k];
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
    if (autoRun) {
      checkSlaves();
      if (vt100Mode) {
        showSlaves();
        showTasks();
        vt100ClrEos();
      }
      doBegin();
      assignTask();
    } // auto
    if (ledCnt == 0) {
      ledOff();
    } else {
      ledCnt--;
    }
  } // tick
}
