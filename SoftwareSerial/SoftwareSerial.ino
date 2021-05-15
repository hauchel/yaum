/*
  
 Verbindung mit ESP via SoftSer
 7  Reset       über Spannungsteiler an ESP Reset (activ low)
 8  RX   weiss an ESP TX
 9  TX   gelb  über Spannungsteiler an ESP RX 

 etwas tricky, weil sich die sch Software immer mit 115200 meldet, daher hoffen dass es beim ersten Mal funzt
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9); // RX, TX

void setup() {
  const char ich[] = "SoftwareSerial " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  mySerial.begin(115200);
  mySerial.println("AT+CIOBAUD=38400");
  mySerial.begin(38400);
}

void loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
