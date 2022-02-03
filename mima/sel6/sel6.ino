// see https://www.mikrocontroller.net/topic/531777
// Reihenfolge von Tasten, zeigt die erste in Warteschlange
//TODO Verhalten bei ulong Überlauf

const byte anz = 6;
// pin assignments (Arduino Pin Nummern)
const byte tas[anz] = {4, 5, 6, 7, 8, 9};
const byte led[anz] = {10, 11, 12, 13, A0, A1};
unsigned long zeit[anz];
bool verbo = true;
byte aktiv = 99; // falls kein LED aktiv

void msg(const char txt[], int n) {
  if (verbo) {
    Serial.print(txt);
    Serial.print(" ");
    Serial.println(n);
  }
}

void ledAn(byte lnr) {
  msg ("Led an ", lnr);
  if (lnr < anz) digitalWrite(led[lnr], HIGH);
}

void ledAus(byte lnr) {
  msg ("Led aus", lnr);
  if (lnr < anz)  digitalWrite(led[lnr], LOW);
}

bool tastDown(byte tnr) {
  // true falls Taste tnr gedrückt
  return digitalRead(tas[tnr]) == LOW;
}

void showWS() {
  char str[80];
  Serial.println(" #        Zeit");
  for (byte i = 0; i < anz; i++) {
    sprintf(str, "%2u   %9u  ", i, zeit[i]);
    Serial.println(str);
  }
}

byte findAktiv() {
  // returnt Nummer des kleinsten Zeitstempels > 0
  // 99 falls alles 0
  unsigned long zs;
  byte ak = 99;
  zs = millis();
  for (byte i = 0; i < anz; i++) {
    if  (zeit[i] > 0) {
      if (zeit[i] <= zs) {
        zs = zeit[i];
        ak = i;
      }
    }
  } //next
  return ak;
}

void doCmd(byte tmp) {
  if (tmp > 47 and tmp < anz + 48) { // Tasten 0 bis 5 Lampentest
    byte le = tmp - 48;
    ledAn(le);
    return;
  }
  switch (tmp) {
    case 'a':   //
      Serial.print ("Aktiv ");
      Serial.println (findAktiv());
      break;
    case 'c':   //
      for (byte i = 0; i < anz; i++) {
        ledAus(i);
      }
      break;
    case 's':   //
      showWS();
      break;
    case 'v':   //
      verbo = !verbo;
      if (verbo) {
        Serial.println("Verbose an");
      } else {
        Serial.println("Verbose aus");
      }
      break;
    default:
      Serial.print(tmp);
      Serial.println ("? 0..5, aktive, clear, show, verbose,");
  } //case
}

void setup() {
  const char info[] = "Sel6 " __DATE__  " " __TIME__;
  Serial.begin(38400);
  Serial.println(info);
  for (byte i = 0; i < anz; i++) {
    pinMode(tas[i], INPUT_PULLUP);
    pinMode(led[i], OUTPUT);
    zeit[i] = 0;
    ledAus(i);
  } //next
}

void loop() {
  byte zwi;

  if (Serial.available() > 0) {
    doCmd(Serial.read());
  }

  // Tasten vs Warteschlange
  for (byte i = 0; i < anz; i++) {
    if (tastDown(i)) {
      if (zeit[i] == 0) { // neu aufnehmen
        zeit[i] =  millis();
        msg("Neu", i);
      }
    } else {
      if (zeit[i] > 0) { // entfernen
        zeit[i] = 0;
        msg("Weg", i);
      }
    }
  } // next
  // der mit der kleinsten Zeit wird angezeigt 
  zwi = findAktiv();
  if (zwi != aktiv) {
    ledAus(aktiv);
    aktiv = zwi;
    ledAn(aktiv);
  }
}
