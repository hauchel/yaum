// slave functionality
#include <Wire.h>
byte backP = 0;
char backBuf[32];
byte recvP = 0;
char recvBuf[32];
char slCmd = ' '; // sent to slave by Cxx to be executed in doCmd
uint32_t slNum = 0; //      "   after some cmds
uint32_t elaTim = 0; // back to master after T
char runS   = 'I';  // State Idle Running Complete fs Open
char runR   = VERSION;  // Result 0..9,  Initial version
byte zeigClu = 0; // debug output

void storeFld(char c) {
  // a  Xa   1..30
  // b  Xb  31..60
  // c  Xc  61..
  byte  ug, og;
  ug = getUG(c);
  og = getOG(c);
  byte f = 2;
  for (byte k = ug; k <= og; k++) {
    fld[k] = recvBuf[f++] - '0';
  }
}

void provideFld(char c) {
  // a  Xa   1..30
  // b  Xb  31..60
  // c  Xc  61..
  byte  ug, og;
  ug = getUG(c);
  og = getOG(c);
  backP = 0;
  for (byte k = ug; k <= og; k++) {
    backBuf[backP++] = fld[k] + '0';
  }
}

void provide32(char c, uint32_t nu) {
  // puts c nu to backBu
  backP = 0;
  char str[20];
  sprintf(str, "%c%lu", c, nu);
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

void nummer() {
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
  // char   Auftrag           handled
  // Cx       slCmd           slave S F
  // Fa Fb Fc   provide fld         here
  // G      read game nummer      slave F
  // L          logge           slave F
  // O          open file nummer      slave F
  // T          provide execution time    here
  // S          provide Status runS runR  here
  // R          store Result        slave F
  // V      set twi Adr nummer      slave S F
  // Xa Xb Xc   store fld         here
  char ch;
  ch = recvBuf[0];
  switch (ch) {
    case 'C':
      slCmd = recvBuf[1];
      break;
    case 'F':
      provideFld(recvBuf[1]);
      break;
    case 'G':
    case 'O':
    case 'V':
	  runS='B';
      slCmd = ch;
      nummer();
      break;
    case 'R':
    case 'L':
      slCmd = ch;
	  runS='B';
      break;
    case 'S':
      backP = 3;
      backBuf[0] = runS;
      backBuf[1] = runR;
	  backBuf[2] = slCmd;
      break;
    case 'T':
      provide32(ch, elaTim);
      break;
    case 'X':
      storeFld(recvBuf[1]);
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
  if ((recvP>5) && (recvP<32)) recvBuf[recvP++] = '\x00';   // >5  to avoid problems with 'S' query which immediately fills [0]
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
