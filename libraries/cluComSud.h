// common routines for sod-cluster

uint16_t inp;
bool inpAkt;      // true if last input was a digit

#define ANZFLD 82 // 81 fields starting at 1
unsigned char fld[ANZFLD]; // starts at 1
unsigned char removeme[10]; // can be removed if all same anzfld


byte getUG(char c) {
  // a  Xa   1..30
  // b  Xb  31..60
  // c  Xc  61..
  switch (c) {
    case 'a':   //
      return 1;
    case 'b':   //
      return 31;
    case 'c':   //
      return 61;
    default:
      msgF(F("getUG c?? "), c);
  }
  return 0;
}

byte getOG(char c) {
  switch (c) {
    case 'a':   //
      return 30;
    case 'b':   //
      return 60;
    case 'c':   //
      return ANZFLD-1;
    default:
      msgF(F("getOG c?? "), c);
  }
  return 0;
}

void showFldOne(byte f) {
  char c;
  c = fld[f] + '0';
  if (c == '0') {
    c = '.';
  }
  if (c > '9') {
    c = '*';
  }
  Serial.print(c);
}

void showFld() {
  byte f = 1;
  Serial.println();
  for (byte r = 0; r < 9; r++) {
    Serial.print(' ');
    for (byte t = 0; t < 3; t++) {
      for (byte e = 0; e < 3; e++) {
        showFldOne(f);
        f++;
      } //einzel
      Serial.print("  ");
    } //triple
    Serial.println();
    if ((r == 2) || (r == 5)) {
      Serial.println();
    }
  } //row
}
