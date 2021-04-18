// simple logger only
#define VERSION 'b'
/*
  FileServer   runS
  O open    File open, ready. runB can indicate filenumber, but only valid immediately after opening
  I idle    File not open, ready
  B busy    set to B immediately after receiving cmd. Processing must set it to O, I or E
  E Error   runB can indicate type of error
*/

#include <SPI.h>
#include <SD.h>

#include <cluCom.h>
#include <cluComPri.h>

char runS   = 'I';  // State Idle Running Complete fs Open
char runR   = VERSION;  // Result 0..9, here Initial version
byte backP = 0;
char backBuf[32];
byte recvP = 0;
char recvBuf[32];
char slCmd = ' '; // sent to slave by Cxx to be executed in doCmd
uint32_t slNum = 0; //      "   after some cmds
uint32_t inp;
byte zeigClu = 0; // debug output
bool inpAkt;

#define ANZFIL 6
const char fileNames [ANZFIL][12] = { // index.txt
  {"prnul.txt"}, // 0
  {"prein.txt"}, // 1
  {"przwo.txt"}, // 2
  {"prdre.txt"}, // 3
  {"prvie.txt"}, // 4
  {"prfun.txt"}, // 5
};


void  getSlNum() {
  //  transfers to slNum
  char c;
  byte k = 1;
  slNum = 0;
  while (true) {
    c = recvBuf[k++];
    if ((c >= '0') && (c <= '9')) {
      slNum = slNum * 10 + (c - '0');
    } else {
      return;
    }
  }
}

byte receiveHandle() {
  // char   Auftrag                         Master
  // Cx               slCmd
  // L     LnnnnL     logge nummer        y
  // O     OnnnnO     open file nummer    Y
  // S     provide Status runS runR       q
  // V      set twi Adr nummer            V

  char ch;
  ch = recvBuf[0];
  switch (ch) {
    case 'C':
      slCmd = recvBuf[1];
      break;
    case 'L':
    case 'O':
    case 'V':
      runS = 'B';
      slCmd = ch;
      getSlNum();
      break;
    case 'S':
      backP = 3;
      backBuf[0] = runS;
      backBuf[1] = runR;
      backBuf[2] = slCmd;
      break;
    default:
      msgF(F("recH ch?? "), ch);
      return 1;
  } // case
  return 0;
}

void receiveEvent(int howMany) {
  ledOn(1);
  recvP = 0;
  char c;
  while (0 < Wire.available())   {
    c = Wire.read();
    recvBuf[recvP++] = c;
    if (zeigClu > 0) Serial.print(c);
  }
  if (zeigClu > 0) msgF(F("<Recv"), recvP);
  if (recvP > 0) {
    receiveHandle();
  }
}

void requestEvent() {
  for (byte k = 0; k < backP; k++) {
    Wire.write(backBuf[k]);
  }
  ledOn(1);
}


File myFile;
char logNam[13];

bool noFile() {
  if (!myFile) {
    msgF(F("Err no File"), 0);
    return true;
  }
  return false;
}

byte openLogFile(byte nr) {
  runS = 'B';
  if (myFile) {
    myFile.close();
  }
  if (nr >= ANZFIL) {
    msgF(F("open num hi"), nr);
    runS = 'E';
    return false;
  }

  strcpy(logNam, fileNames[nr]);
  Serial.print(F(" Log "));
  Serial.println(logNam);
  myFile = SD.open(logNam, FILE_WRITE);
  if (myFile) {
    runS = 'O';
    runR = nr + '0';
    return true;
  } else {
    msgF(F("Error opening file"), nr);
    runS = 'E';
    return false;
  }
}

void dumpLog() {
  if (myFile) {
    msgF( F("dumpLog Close File First") , 0);
    return;
  }
  Serial.print(F("Dump Log "));
  Serial.println(logNam);
  myFile = SD.open(logNam);
  while (myFile.available()) {
    Serial.write(myFile.read());
  }
  myFile.close();
}

void delLog() {
  if (myFile) {
    msgF( F("delLog Close File First") , 0);
    return;
  }
  Serial.print(F("Del Log "));
  Serial.println(logNam);
  SD.remove(logNam);
}

void logRecvBuf() {
  char c;
  if (noFile()) return;
  msgZ(1, F("Log recvBuf") , recvP);
  for (byte k = 0; k < 32; k++) {
    c = recvBuf[k];
    if (c == 0) break;
    Serial.print(c);
    myFile.write(c);
  }
  myFile.println("<");
  Serial.println(F("<"));
}

void logSlNum() {
  if (noFile()) return;
  msgZ(1, F("Log Num") , slNum);
  myFile.println(slNum);
  runS = 'O';
}

void ledOn(byte n) {
  digitalWrite(ledPin, HIGH);
  ledCnt = n;
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void info() {
  Serial.println();
  Serial.print(F("runS "));
  Serial.print(runS);
  Serial.print(F("  runR "));
  Serial.println(runR);

  if (myFile) {
    Serial.print(logNam);
  } else {
    Serial.println(F("No File open"));
  }
}

void setup() {
  const char ich[] = "FileSerPri " __DATE__  " "  __TIME__;
  runS = 'B';
  Serial.begin(38400);
  Serial.println(ich);
  pinMode(ledPin, OUTPUT);
  ledOn(1);
  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(10)) {
    Serial.println(F("SD card initialization failed!"));
    return;
  }
  getMyAdr();
  Wire.begin(myAdr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  runS = 'I';
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
      break;
    case 'c':   //
      if (myFile) {
        myFile.close();
        msgF(F("File closed"), 0);
        runS = 'I';
      } else {
        msgF(F("Err no File"), 0);
      }
      break;
    case 'd':   //
      dumpLog();
      break;
    case 'D':   //
      delLog();
      break;
    case 'i':   //
      info();
      break;
    case 'l':
      slNum = inp;
      logSlNum();
      break;
    case 'L':   //
      logSlNum();
      break;
    case 'o':   //
      msgF(F("Open"), openLogFile(inp));
      break;
    case 'O':   //
      openLogFile(slNum);
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


void loop() {
  char tmp;
  if (Serial.available() > 0) {
    tmp = Serial.read();
    doCmd(tmp);
  } // avail

  /* we use SPI, no LED fumbling
    currTim = millis();
    if (nexTim < currTim) {
     nexTim = currTim + tick;
     if (ledCnt == 0) {
       ledOff();
     } else {
       ledCnt--;
     }
    } // tick
  */
  if (slCmd != ' ') {
    if (zeig > 5) {
      Serial.print(F("Loop slCmd "));
      Serial.print(slCmd);
      Serial.print(F(" slNum "));
      Serial.println(slNum);
    }
    doCmd(slCmd);
    slCmd = ' ';
  } //
}
