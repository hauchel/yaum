// slave for duinocoin via i2c based on revox Arduino_Code
// take the sha1 and uniqueID from revox
// compare to this ino if changed
//
#define USE_LGT_EEPROM_API
#define VERSION 'c'
#include <cluCom.h>
#include "sha1.h"
#include "uniqueID.h"
byte backP = 0;
char backBuf[32];
byte recvP = 0;
char recvBuf[32];
volatile char slCmd = ' '; // sent to slave by Cxx to be executed in doCmd
uint16_t ducos1tim = 0;
uint32_t elaTim = 0; // back to master after T
volatile char runS   = 'I';  // State: Idle Busy Complete  I->B->cmd->I  I->B->hashme->C  C->I
volatile char runR   = VERSION;  // Result 0..9, here Initial version
byte zeigClu = 0; // debug output communication

uint16_t  inp;
bool inpAkt;                  // true if last input was a digit


//check:
String lastblockhash = "";
String newblockhash = "";
String DUCOID = "";
uint16_t difficulty = 10;
uint16_t ducos1result = 0;
const uint16_t job_maxsize = 104; // 40+40+20+3 is the maximum size of a job
uint8_t job[job_maxsize];

#ifdef ARDUINO_ARCH_AVR
uint16_t ducos1a(String lastblockhash, String newblockhash, uint16_t difficulty)
#else
uint32_t ducos1a(String lastblockhash, String newblockhash, uint16_t difficulty)
#endif
{
  // DUCO-S1 algorithm implementation for AVR boards (DUCO-S1A)
  newblockhash.toUpperCase();
  const char *c = newblockhash.c_str();
  size_t final_len = newblockhash.length() >> 1;
  for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
    job[j] = ((((c[i] & 0x1F) + 9) % 25) << 4) + ((c[i + 1] & 0x1F) + 9) % 25;

  // Difficulty loop
#ifdef ARDUINO_ARCH_AVR
  // If the difficulty is too high for AVR architecture then return 0
  if (difficulty > 655)
    return 0;
  for (uint16_t ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++)
#else
  for (uint32_t ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++)
#endif
  {
    Sha1.init();
    Sha1.print(lastblockhash + String(ducos1res));
    // Get SHA1 result
    uint8_t *hash_bytes = Sha1.result();
    if (memcmp(hash_bytes, job, SHA1_HASH_LEN * sizeof(char)) == 0)
    {
      // If expected hash is equal to the found hash, return the result
      return ducos1res;
    }
  }
  return 0;
}

// Grab Arduino chip DUCOID
String get_DUCOID() {
  String ID = "DUCOID";
  char buff[4];
  for (size_t i = 0; i < 8; i++)
  {
    sprintf(buff, "%02X", (uint8_t) UniqueID8[i]);
    ID += buff;
  }
  return ID;
}

void testdata(uint8_t n) {
  if (n == 1) {
    lastblockhash = "b1d5daa0118596c898ab96a63e58e1da0ad5084a";
    newblockhash = "ded13127e18541f6dea390947807161d230e422e";
    difficulty = 6;
    return;
  }
  if (n == 2) {
    lastblockhash = "7c54338b487a53970cdd2882bee5221128417635";
    newblockhash = "403d2fb262c14bbddc980fae703739eeb55a95af";
    difficulty = 6;
    return;
  }
  if (n == 3) {
    lastblockhash = "6b41100f772d34674947145e0ace91f15667614b";
    newblockhash = "51eac4263f3014ec3a4f4644d0c5e00e1217683c";
    difficulty = 6;
    return;
  }
  msgF(F("No testdata "), n);
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
  Serial.print(String(ducos1result) + "," + String(ducos1tim) + "," + String(DUCOID) + "\n");
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
  sprintf(str, "%-22s  ", DUCOID.c_str());
  provideStr(str);
}

uint8_t slNum() {
  //returns two digits as byte from  recvBuf[1,2] e.g D06
  return (recvBuf[1]-'0')*10 + (recvBuf[2]-'0');
}

// char     Auftrag                         resp    Rem
// A      clear flags                      -       sets runS runR
// D      set difficulty      tbd                     -
// E   *  provide exec time  in ms
// H      hashme
// I      provide ID
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
  //if (howMany == 0) return;
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
    case 'D':   //
      difficulty = slNum();
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
    case 't':   //
      msgF(F("testdata"), inp);
      testdata(inp);
      break;

    case 'v':   //
    case 'V':   //
      if (ch == 'V') eeData.info.myAdr = slNum(); else eeData.info.myAdr = inp;
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
  } // case
} 

void setup() {
  const char ich[] = "duino " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  pinMode(ledPin, OUTPUT);
  //check:
  DUCOID = get_DUCOID();
  ledOn(1);
  getMyAdr();
  Wire.begin(myAdr);
  Serial.println(DUCOID);
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
