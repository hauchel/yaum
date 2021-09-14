// ATtiny85 slave for duinocoin via i2c based on revox Arduino_Code
// modified to avoid strings
// take the sha1 from revox, modify config.h to save RAM
//
#define VERSION 'a'
#include "sha1.h"
#include <Wire.h>
#include <EEPROM.h>

char backBuf[16];                // I2C size is 16
byte backP = 0;
char recvBuf[16];
byte recvP = 0;
char lastblock[42];               //40+\0
byte lastblockP = 0;
char newblock[42];
byte newblockP = 0;
volatile char slCmd = ' ';        // sent to slave by Cxx to be executed in doCmd
volatile char runS   = 'I';       // State: Idle Busy Complete  I->B->cmd->I  I->B->hashme->C  C->I
volatile char runR   = VERSION;   // Result 0..9, here Initial version
const byte ledPin = 3;            // LED vs GND
const byte dbgPin = 4;            // for LA
byte myAdr;                       //

unsigned long currTim;
unsigned long nexTim = 0;
unsigned long tick = 100;

byte ledCnt = 0;
//check:

uint16_t ducos1result = 0;
uint16_t ducos1tim = 0;           // in ms
uint8_t difficulty = 10;


uint16_t calcsha1(const char *lastblock, const char *newblock, uint16_t difficulty) {
  uint8_t jj[SHA1_HASH_LEN];
  for (size_t i = 0, j = 0; j < SHA1_HASH_LEN; i += 2, j++) {
    jj[j] = ((((newblock[i] & 0x1F) + 9) % 25) << 4) + ((newblock[i + 1] & 0x1F) + 9) % 25;
  }
  for (uint16_t ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++)  {
    Sha1.init();
    Sha1.print(lastblock);
    Sha1.print(ducos1res);
    uint8_t * hash_bytes = Sha1.result();
    if (memcmp(hash_bytes, jj, SHA1_HASH_LEN) == 0)   {
      return ducos1res;
    }
  }
  return 0;
}

uint8_t getEprom85() {
  return EEPROM.read(0);
}

void setEprom85(byte val) {
  EEPROM.update(0, val);
}

void hashme() {
  unsigned long startTime = millis();
  ducos1result = calcsha1(lastblock, newblock, difficulty);
  ducos1tim = uint16_t( millis() - startTime);
  runS = 'C';
}

void ledOn(byte n) {
  digitalWrite(ledPin, HIGH);
  ledCnt = n;
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void dbgOn() {
  digitalWrite(dbgPin, HIGH);
}

void dbgOff() {
  digitalWrite(dbgPin, LOW);
}

void provideStr(const char str[]) {
  // puts str (\0)  to backBu,
  backP = 0;
  for (byte k = 0; k < 16; k++) {
    if (str[k] == 0) return;
    backBuf[backP++] = str[k];
  }
}

void copyBuf() {
  // puts recv to back, test only
  backP = 16;
  for (byte k = 0; k < 16; k++) {
    backBuf[k] =  recvBuf[k];
  }
}

void provide16(uint16_t num) {
  // puts num to backBu, num readable len5 no \0
  char str[20];
  sprintf(str, "%5u", num);
  provideStr(str);
}

uint8_t slNum() {
  //returns two digits as byte from  recvBuf[1,2] e.g D06
  return (recvBuf[1] - '0') * 10 + (recvBuf[2] - '0');
}

// char     Auftrag                         resp    Rem
// A      clear flags                      -       sets runS runR
// D      set difficulty                        -
// E   *  provide exec time  in ms
// H      hashme
// I      provide ID
// L      lastblockhash
// M
// N
// O      newblockhash
// P
// Q
// R   *  provide Result
// S   *  provide Status                    runS runR slCmd
// V      set twi Adr nummer    Vb8         -

void receiveEvent(int howMany) {
  ledOn(1);
  if (howMany == 0) return;
  dbgOn();
  char c;
  c = Wire.read();
  if (c == 'S') {  //status return immediately
    backP = 3;
    backBuf[0] = runS;
    backBuf[1] = runR;
    backBuf[2] = slCmd;
    dbgOff();
    return;
  }
  if (c == 'R') {  //result return immediately
    provide16(ducos1result);
    dbgOff();
    return;
  }
  if (c == 'E') {  //elapsed return immediately
    provide16(ducos1tim);
    dbgOff();
    return;
  }
  runS = 'B'; // busy until cmd processed
  slCmd = c;
  recvP = 1;
  recvBuf[0] = c;
  while (Wire.available())   { //fetch, has to be done here for 85!!
    c = Wire.read();
    recvBuf[recvP++] = c;
  }
  dbgOff();
}

void requestEvent() {
  dbgOn();
  ledOn(1);
  for (byte k = 0; k < backP; k++) {
    Wire.write(backBuf[k]);
  }
  dbgOff();
}

void doCmd( char ch) {
  switch (ch) {
    case 'A':   //
      runS = 'I';
      runR  = VERSION;
      slCmd = ' ';
      break;
    case 'C':   //
      copyBuf();
      break;

    case 'D':   //
      difficulty = slNum();
      EEPROM.update(1, byte(difficulty)); //debug
      break;
    case 'L':   //
      lastblockP = 0;
      for (byte k = 1; k < recvP; k++) {
        lastblock[lastblockP++] = recvBuf[k];
      }
      break;
    case 'M':
      for (byte k = 1; k < recvP; k++) {
        lastblock[lastblockP++] = recvBuf[k];
      }
      break;
    case 'N':
      for (byte k = 1; k < recvP; k++) {
        lastblock[lastblockP++] = recvBuf[k];
      }
      lastblock[lastblockP] = 0;
      break;
    case 'O':   //
      newblockP = 0;
      for (byte k = 1; k < recvP; k++) {
        newblock[newblockP++] = recvBuf[k];
      }
      break;
    case 'P':   //
      for (byte k = 1; k < recvP; k++) {
        newblock[newblockP++] = recvBuf[k];
      }
      break;
    case 'Q':   //
      for (byte k = 1; k < recvP; k++) {
        newblock[newblockP++] = recvBuf[k];
      }
      newblock[newblockP] = 0;
      hashme();
      break;
    case 'H':   //
      hashme();
      break;
    case 'V':   //
      setEprom85(byte(slNum()));
      setup();
      break;
  } // case
}

void setup() {
  pinMode(ledPin, OUTPUT);
  ledOn(1);
  pinMode(dbgPin, OUTPUT);
  myAdr = getEprom85();
  if ((myAdr < 10) || (myAdr > 100)) { // scan only from 0x08 to 0x77
    myAdr = 50;
  }
  Wire.begin(myAdr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  char ch;
  if (slCmd != ' ') { // my masters voice
    dbgOn();
    runR = '?'; // to store result
    doCmd(slCmd);
    slCmd = ' ';
    if (runS != 'C') {
      runS = 'I';
    }
    dbgOff();
  }  // slcommand

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
