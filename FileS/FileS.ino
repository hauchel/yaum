#define VERSION 'b'
/*
  FileServer   runS
  O open
  I idle
  B busy
*/

#include <SPI.h>
#include <SD.h>

#include <cluCom.h>
#include <cluComSud.h>
#include <cluSlave.h>
#define ANZFIL 5
const char fileNames [ANZFIL][12] = { // index.txt
  {"gamtst.txt"}, // 0
  {"kudok.txt"},  // 1
  {"many.txt"},   // 2
  {"kurz.txt"},  // 3
  {"top95.txt"},  // 4
};

File myFile;
char filNam[13];
char logNam[13];
unsigned long posi;

bool noFile() {
  if (!myFile) {
    msgF(F("Err no File"), 0);
    return true;
  }
  return false;
}

void switch2Log() {
  if (myFile) {
    runS = 'B';
    posi = myFile.position();
    myFile.close();
    myFile = SD.open(logNam, FILE_WRITE);
  }
}

void switch2Fil() {
  myFile.close();  // log!
  myFile = SD.open(filNam);
  myFile.seek(posi);
  msgZ(5, F("Reopen Pos= "), posi);
  runS = 'O';
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
  msgZ(5, F("Log recvBuf") , recvP);
  switch2Log();
  for (byte k = 0; k < 32; k++) {
    c = recvBuf[k];
    if (c == 0) break;
    Serial.print(c);
    myFile.write(c);
  }
  myFile.println("<");
  Serial.println(F("<"));
  switch2Fil();
}

void logFld() {
  if (noFile()) return;
  msgZ(5, F("Log Fld") , recvP);
  switch2Log();
  for (byte k = 1; k < ANZFLD; k++) {
    myFile.write(fld[k] + '0');
  }
  myFile.println("<");
  switch2Fil();
}

byte searchGame(uint16_t gam) {
  if (noFile()) return false;
  unsigned long pos = 82L * long(gam);
  return myFile.seek(pos);
}

byte openGameFile(byte nr) {
  runS = 'B';
  if (myFile) {
    myFile.close();
  }
  if (nr >= ANZFIL) {
    msgF(F("open num hi"), nr);
    runS = 'E';
    return false;
  }
  sprintf(logNam, "log%d.TXT", nr);
  strcpy(filNam, fileNames[nr]);
  Serial.print(filNam);
  Serial.print(F(" Log "));
  Serial.println(logNam);
  myFile = SD.open(filNam);
  if (myFile) {
    runS = 'O';
    runR = nr + '0';
    return true;
  } else {
    msgF(F("Error opening file"), nr);
    runS = 'I';
    return false;
  }
}

byte readGame () {
  if (noFile()) return false;
  runS = 'B';
  byte b;
  byte f = 0;
  if (zeig > 5) msgF(F("Game at "), myFile.position());
  for (byte k = 0; k < 81; k++) {
    b = myFile.read();
    Serial.print(char(b));
    f++;
    if (b == '.') {
      fld[f] = 0;
    } else {
      fld[f] = b - '0';
    }
  }
  b = myFile.read(); //CR
  if (b == 10) {
    Serial.println (" LF");
    runS = 'O';
  } else {
    msgF(F(" Invalid EOL"), b);
    runS = 'E';
  }
  if (zeig > 5) msgF(F("Game at "), myFile.position());
  return true;
}

// predefined variables
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int getFreeMemory() {
  int free_memory;
  if ((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
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
  Serial.print(F("Free Memory = "));
  Serial.println(getFreeMemory());


  if (myFile) {
    Serial.print(filNam);
    Serial.print(F("  Posi "));
    Serial.println(myFile.position());
  } else {
    Serial.println(F("No File open"));
  }
}

void setup() {
  const char ich[] = "FileS " __DATE__  " "  __TIME__;
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
  byte res;
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
    case 'f':   //
      showFld();
      break;
    case 'i':   //
      info();
      break;
    case 'L':   //
      logRecvBuf();
      break;
    case 'p':   //
      if (myFile) {
        msgF(F("Position"), myFile.position());
      } else {
        msgF(F("Err no File"), 0);
      }
      break;
    case 'o':   //
      msgF(F("Open"), openGameFile(inp));
      break;
    case 'r':   //
      res = readGame();
      msgF(F("Read is "), res);
      break;
    case 'R':   //
      logFld();
      break;
    case 's':   //
      res = searchGame(inp);
      msgF(F("Search is "), res);
      break;
    case 'S':   //
      myFile.seek(inp);
      msgF(F("Position"), myFile.position());
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
    switch (slCmd) {
      case 'O':   //
        openGameFile(slNum);
        break;
      case 'G':   //
        searchGame(slNum);
        readGame();
        break;
      case 'L':   //
      case 'R':   //
        doCmd(slCmd);
        break;
      default:
        msgF(F("slCmd ?? "), slCmd);
    } //case
    slCmd = ' ';
  } //
}
