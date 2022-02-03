
//
const byte anz = 6;
const byte inAuf[anz] = {23, 3, 25, 21, 27, 29};
const byte inZu[anz] = {22, 2, 24, 20, 26, 28};
const byte last[anz] = {LOW, LOW, LOW, LOW, LOW, LOW};
const byte outAuf[anz] = {30, 4,}; //...
const byte outZu[anz] = {31, 5,}; //...
const byte outLauf[anz] = {32, 6,}; //...
long premillis[anz];
long Laufzeit = 5000;
enum stateT {oben, runter, unten, rauf};
stateT state[anz];

void setup() {
  for (byte i = 0; i < anz; i++) {
    state[i] = oben;
    pinMode(inAuf[i], INPUT_PULLUP);
    pinMode(inZu[i], INPUT_PULLUP);
    pinMode(outAuf[i], OUTPUT);
    digitalWrite(outZu[i], HIGH);
    pinMode(outZu[i], OUTPUT);
    pinMode(outLauf[i], OUTPUT);
  }
}

void loop() {
  for (byte i = 0; i < anz; i++) {
    switch (state[i]) {
      case oben:
        if (digitalRead(inZu[i]) == HIGH) {
          state[i] = runter;
          digitalWrite(outLauf[i], HIGH);
          digitalWrite(outAuf[i], LOW);
          digitalWrite(outZu[i], LOW);
          premillis[i] = millis();
        }
        break;
      case runter:
        if (millis() - premillis[i] >= Laufzeit) {
          state[i] = unten;
          // LED schalten?
          digitalWrite(outLauf[i], HIGH);
        }
        break;
      case unten:
        if (digitalRead(inAuf[i]) == HIGH) {
          state[i] = rauf;
          digitalWrite(outLauf[i], LOW);
          digitalWrite(outAuf[i], HIGH);
          digitalWrite(outZu[i], LOW);
        }
        break;
      case rauf:
        if (millis() - premillis[i] >= Laufzeit) {
          state[i] = oben;
          // LED schalten?
          digitalWrite(outLauf[i], HIGH);
        }
        break;
    }
  }
}
