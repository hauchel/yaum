// see https://www.mikrocontroller.net/topic/531777
// Reihenfolge von Tasten, zeigt die erste in Warteschlange
// Variante mit verketteter Liste

const byte anz = 6;
//Arduino Pin Nummern für Tasten und Leds (Vorsicht, nicht verwechseln!)
const byte tas[anz] = { 4,  5,  6,  7,  8,  9};
const byte led[anz] = {10, 11, 12, 13, A0, A1};

// Liste
bool state[anz];
byte prev[anz];
byte next[anz];
byte first = 99;  // 99 = null
byte last = 99;

// debug
bool verbo = true;
bool plumi = true;

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

void showListe() {
  char str[80];
  byte i;
  Serial.println(" #  sta   prev next");
  for (i = 0; i < anz; i++) {
    sprintf(str, "%2u   %1s     %2u  %2u ", i, state[i] ? "T" : "F", prev[i], next[i]);
    Serial.println(str);
  }
  sprintf(str, "first %2u  last %2u :", first, last);
  Serial.print(str);
  i = first;
  while (i != 99) {
    Serial.print(" ");
    Serial.print(i);
    i = next[i];
  }
  Serial.println();
}

void rein(byte tnr) {
  msg("rein", tnr);
  // nur zur Sicherheit:
  if  (state[tnr]) {
    Serial.println ("War schon drin ??");
    return;
  }
  state[tnr] = true;
  if (first == 99) { // Liste leer
    first = tnr;
    prev[tnr] = 99;
    next[tnr] = 99;
  } else {
    next[last] = tnr;
    prev[tnr] = last;
  }
  next[tnr] = 99;
  last = tnr;
}

void raus(byte tnr) {
  msg("raus", tnr);
  // nur zur Sicherheit:
  if  (!state[tnr]) {
    Serial.println ("War nicht drin ??");
    return;
  }
  state[tnr] = false;
  if (prev[tnr] == 99) { // war erster
    first = next[tnr];
  } else {
    next[prev[tnr]] = next[tnr];
  }
  if (next[tnr] == 99) { // war letzter
    last = prev[tnr];
  } else {
    prev[next[tnr]] = prev[tnr];
  }
}

void doCmd(byte tmp) {
  if (tmp > 47 and tmp < anz + 48) { // Tasten 0 bis 5 rein + oder raus -
    byte le = tmp - 48;
    if (plumi) rein(le); else raus(le);
    return;
  }
  switch (tmp) {
    case 's':   //
      showListe();
      break;
    case 'v':   //
      verbo = !verbo;
      if (verbo) {
        Serial.println("Verbose an");
      } else {
        Serial.println("Verbose aus");
      }
      break;
    case '+':   //
      plumi = true;
      Serial.print("+ ");
      break;
    case '-':   //
      plumi = false;
      Serial.print("- ");
      break;
    default:
      Serial.print(tmp);
      Serial.println ("?  0..5, +, -, show, verbose");
  } //case
}

void setup() {
  const char info[] = "Sel6Lis " __DATE__  " " __TIME__;
  Serial.begin(38400);
  Serial.println(info);
  for (byte i = 0; i < anz; i++) {
    pinMode(tas[i], INPUT_PULLUP);
    pinMode(led[i], OUTPUT);
    ledAus(i);
    state[i] == false;
  } //next
}

void loop() {
  byte altf;

  if (Serial.available() > 0) {
    doCmd(Serial.read());
  }

  // Tasten vs Liste
  altf = first;
  for (byte i = 0; i < anz; i++) {
    if (tastDown(i)) {
      if (!state[i]) rein(i); 
    } else {
      if (state[i])  raus(i); 
    }
  } // next
  
  // der erste wird angezeigt
  if (first != altf) {
    ledAus(altf);
    ledAn(first);
  }

}
