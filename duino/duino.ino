// slave for duinocoin via i2c
// take the sha1 and uniqueID from revox Arduino_Code
// compare to this ino if changed
//
#define VERSION 'b'
#include <cluCom.h>
#include "sha1.h"
#include "uniqueID.h"
byte backP = 0;
char backBuf[32];
byte recvP = 0;
char recvBuf[32];
volatile char slCmd = ' '; // sent to slave by Cxx to be executed in doCmd
uint16_t slNum = 0; //      "   after some cmds
uint32_t elaTim = 0; // back to master after T
volatile char runS   = 'I';  // State: Idle Busy Complete  I->B->cmd->I  I->B->hashme->C  C->I 
volatile char runR   = VERSION;  // Result 0..9, here Initial version
byte zeigClu = 0; // debug output communication

uint16_t  inp;
bool inpAkt;                  // true if last input was a digit


// compare to 
char buffer[44];
String IDstring = "??";
//Received result (308)
String lastblockhash = "17f0c5bf6a51c7e342f2904149378405ee18c929";
String newblockhash = "0b2f92fa113372c4020bb80616ffaa05947d3ca7";
unsigned int difficulty = 5;
unsigned int ducos1result = 0;
unsigned int ducos1tim = 0;
unsigned char* newblockhash1;
size_t sizeofhash = 100;

// DUCO-S1A hasher
int ducos1a(String lastblockhash, String newblockhash, int difficulty) {
  // DUCO-S1 algorithm implementation for AVR boards (DUCO-S1A)
  // Difficulty loop
  //Conversion
  const char * c = newblockhash.c_str();
  size_t len = newblockhash.length();
  size_t final_len = len / 2;
  // Clearing the newblockhash1 buffer
  memset(newblockhash1, 0, sizeofhash);
  for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
    newblockhash1[j] = (c[i] % 32 + 9) % 25 * 16 + (c[i + 1] % 32 + 9) % 25;
  for (int ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++) {
    Sha1.init();
    Sha1.print(lastblockhash + String(ducos1res));
    // Get SHA1 result
    uint8_t * hash_bytes = Sha1.result();
    if (memcmp(hash_bytes, newblockhash1, SHA1_HASH_LEN * sizeof(char)) == 0) {
      // If expected hash is equal to the found hash, return the result
      return ducos1res;
    }
  }
  return 0;
}

void hashme() {
  Serial.print(F("Hash me: "));
  unsigned long startTime = millis();
  // Call DUCO-S1A hasher
  newblockhash.toUpperCase();
  ducos1result = ducos1a(lastblockhash, newblockhash, difficulty);
  // Calculate elapsed time
  ducos1tim = uint16_t( millis() - startTime);
  runS = 'C';
  Serial.print(String(ducos1result) + "," + String(ducos1tim) + "," + String(IDstring) + "\n");
}

void info() {
  Serial.print (F("Last "));
  Serial.println (lastblockhash);
  Serial.print (F("New  "));
  Serial.println (newblockhash);
  Serial.print (F("Diffi "));
  Serial.println (difficulty);
  Serial.print (F("RecvP "));
  Serial.println (recvP);
  for (byte k = 0; k < recvP; k++) {
    Serial.print(recvBuf[k]);
  }
  Serial.println ();
}

// zeigs:
const byte zFlow = 1;
const byte zNew = 2;        //
const byte zTeach = 4;      //
const byte zFact = 8;       //
const byte zCheck = 64;
const byte zDet = 128;      // Detail

void ledOn(byte n) {
  digitalWrite(ledPin, HIGH);
  ledCnt = n;
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void provideStr(const char str[]) {
  // puts str (\0)  to backBu,
  backP = 0;
  if (zeigClu > 0) {
    Serial.print (F("proStr "));
    Serial.println (str);
  }
  for (byte k = 0; k < 30; k++) {
    if (str[k] == 0) return;
    backBuf[backP++] = str[k];
  }
  msgF(F("Err provideStr overrun"), backP);
}

void provide16(uint16_t num) {
  // puts num to backBu, num readable len5 no \0
  char str[20];
  sprintf(str, "%5u", num);
  provideStr(str);
}

void provideId() {
  char str[30];
  sprintf(str, "%-22s  ", IDstring.c_str());
  provideStr(str);
}

// char     Auftrag                         resp    Rem
// A   *  clear flags                      -       sets runS runR
// D      set difficulty      tbd                     -
// E   *  provide exec time  in ms
// H      hashme
// I   *  provide ID          tbd
// L      lastblockhash
// M
// N      newblockhash
// O
// R   *  provide Result
// S   *  provide Status                    runS runR slCmd
// T      teach in              Tb8
// V      set twi Adr nummer    Vb8         -

void receiveEvent(int howMany) {
  ledOn(1);
  if (howMany == 0) return;
  char c;
  c = Wire.read();
  if (c == 'S') {  //status return immediately
    backP = 3;
    backBuf[0] = runS;
    backBuf[1] = runR;
    backBuf[2] = slCmd;
    return;
  }
  if (c == 'R') {  //result return immediately
    provide16(ducos1result);
    return;
  }
  if (c == 'E') {  //elapsed return immediately
    provide16(ducos1tim);
    return;
  }
  runS = 'B'; // busy until cmd processed
  slCmd = c;
}

void requestEvent() {
  for (byte k = 0; k < backP; k++) {
    Wire.write(backBuf[k]);
    if (zeigClu) Serial.print(backBuf[k]);
  }
  if (zeigClu) msgF(F("<ReQEv"), backP);
  ledOn(1);
}

void help () {
  Serial.println (F("Slave commands:"));
  Serial.println (F("Debug  :  Z z: Flow 1, New 2, Teach 4, Fact 8, Check 64, Det 128 "));
  Serial.println (F("Info   :  I j J(anaflash)  0l(index)"));
  Serial.println (F("Remote :  G(ener) R(ead) F(ield)  K"));
}

void doCmd( char ch) {
  bool weg = false;
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
    case 'A':   //
      runS = 'I';
      runR  = VERSION;
      slCmd = ' ';
      break;
    case 'L':   //
      lastblockhash = "";
      for (byte k = 1; k < recvP; k++) {
        lastblockhash += recvBuf[k];
      }
      break;
    case 'M':   //
      for (byte k = 1; k < recvP; k++) {
        lastblockhash += recvBuf[k];
      }
      break;
    case 'N':   //
      newblockhash = "";
      for (byte k = 1; k < recvP; k++) {
        newblockhash += recvBuf[k];
      }
      break;
    case 'O':   //
      for (byte k = 1; k < recvP; k++) {
        newblockhash += recvBuf[k];
      }
      hashme();
      break;
    case 'd':   //
      difficulty = inp;
      info();
      break;
    case 'h':   //
    case 'H':   //
      hashme();
      break;
    case 'i':   //
      info();
      break;
    case 'I':   //
      provideId();
      break;
    case 'v':   //
    case 'V':   //
      if (ch == 'V') eeData.info.myAdr = slNum; else eeData.info.myAdr = inp;
      setEprom();
      setup();
      break;
    case 'x':   //
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
      msgF(F(" ?? "), ch);
      help();
  } // case
}

void setup() {
  const char ich[] = "duino " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  pinMode(ledPin, OUTPUT);
  // Allocating memory for the hash calculation
  newblockhash1 = (unsigned char*)malloc(sizeofhash * sizeof(unsigned char));

  // Grab Arduino chip ID
  IDstring = "DUCOID";
  char buff[4];
  for (size_t i = 0; i < 8; i++)
  {
    sprintf(buff, "%02X", (uint8_t) UniqueID8[i]);
    IDstring += buff;
  }
  ledOn(1);
  getMyAdr();
  Wire.begin(myAdr);
  Serial.println(IDstring);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  char ch;
  if (Serial.available() > 0) {
    ch = Serial.read();
    doCmd(ch);
  } // avail

  if (slCmd != ' ') { // my masters voice
    runR = '?'; // to store result
    recvP = 1;
    recvBuf[0] = slCmd;
    while (0 < Wire.available())   { //fetch
      ch = Wire.read();
      recvBuf[recvP++] = ch;
      if (zeigClu) Serial.print(ch);
    }
    doCmd(slCmd);
    slCmd = ' ';
    if (runS != 'C') {
      runS = 'I';
    }
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
