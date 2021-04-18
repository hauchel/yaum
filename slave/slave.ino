#include <Wire.h>
#include <cluCom.h>
#include <cluSlave.h>

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
    Serial.print(inp);
    return;
  }
  inpAkt = false;
  switch (tmp) {
    case 'a':   //
      eeData.info.myAdr = inp;
      setEprom();
      setup();
      break;
    case 'b':   //
      backP = inp;
      msgF(F("backP is"), backP);
      break;
    case 'c':   //
      ledOff();
      break;
    case 'f':   //
      showFld();
      break;
    case 'r':   //
      break;
    case 'x':   //
      msgF(F("docmd x"), 0);
      break;
    case 'y':   //
      msgF(F("docmd y"), 0);
      break;
    case 'z':   //
      msgF(F("docmd z"), 0);
      break;
    default:
      msgF(F("?? "), tmp);
  } // case
}

void setup() {
  const char ich[] = "slave " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  pinMode(ledPin, OUTPUT);
  ledOn(1);
  getMyAdr();
  Wire.begin(myAdr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  char tmp;
  if (Serial.available() > 0) {
    tmp = Serial.read();
    doCmd(tmp);
  } // avail

  if (slCmd != ' ') {
    msgF(F("slCmd ist "), slCmd);
    doCmd(slCmd);
    slCmd = ' ';
  } // 
  
  currTim = millis();
  if (nexTim < currTim) {
    nexTim = currTim + tick;
    Serial.print("S");
    if (ledCnt == 0) {
      ledOff();
    } else {
      ledCnt--;
    }
  } // tick
}


