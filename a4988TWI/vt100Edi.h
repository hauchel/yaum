const byte ediLen = 40;
char ediStr[ediLen] = "1234567890Hello";
byte ediX = 1; // col 1..n
byte ediY = 1; // row 1..n
byte ediMsgY = 1;  // row for messages
bool ediIns = false; // ins/overwrite
bool ediDirty = false; // changed

void ediMsg(const char txt[]) {
  vt100Goyx(ediMsgY, 1);
  Serial.print(strlen(ediStr));
  Serial.print(txt);
  vt100ClrEol();
  vt100Goyx(ediY, ediX);
}

void ediDraw() {
  vt100Goyx(ediY, 1);
  Serial.print(ediStr);
  vt100ClrEol();
  vt100Goyx(ediY, ediX);
}

void ediDelChar() {
  // remove char at ediX
  byte i = ediX - 1;
  while (ediStr[i] != 0) {
    ediStr[i] = ediStr[i + 1];
    i++;
  }
  ediDirty = true;
}

void ediInsChar() {
  // ins char at ediX
  byte j = strlen(ediStr); // points to 0
  if (j >= ediLen) return;
  ediStr[j + 1] = 0;
  for (byte i = j ; i >= ediX; i--) {
    ediStr[i] = ediStr[i - 1];
  }
  ediDirty = true;
}

byte doEdi() {
  byte c;
  vt100 = 0;
  ediDirty = false;
  ediDraw();
  while (true) {
    if (Serial.available() > 0) {
      c = doVT100(Serial.read());
      switch (c) {
        case 0: // processing ESC
        case 126: // terminator
          break;
        case 13:  //
        case 27:  //
        case 193: //
        case 194: //
          return c;
          break;
        case 127: // Entf
          ediMsg("Entf");
          ediDelChar();
          ediDraw();
          break;
        case 8: // BS
          ediMsg("BS");
          ediX -= 1;
          if (ediX == 0) ediX = 1;
          ediDelChar();
          ediDraw();
          break;
        case 178: // 2
          ediIns = !ediIns;
          if (ediIns)  ediMsg("Einf");
          else         ediMsg("Uber");
          break;
        case 177: // 1 Pos1
          ediX=1;
           vt100Goyx(ediY, ediX);
          break;
        case 180: // 4 Ende
                 ediX=strlen(ediStr)+1;
           vt100Goyx(ediY, ediX);
          break;
        case 181: // 5 Pg Up
          ediMsg("E181");
          break;
        case 182: // 6 Pg Dwn
          ediMsg("E182");
          break;
        case 195: // right
          ediX += 1;
          if (ediX > ediLen) ediX = ediLen;
          vt100Goyx(ediY, ediX);
          break;
        case 196: //left
          ediX -= 1;
          if (ediX == 0) ediX = 1;
          vt100Goyx(ediY, ediX);
          break;
        default:
          if (ediIns) {
            ediInsChar();
          }
          Serial.print(char(c));
          if (ediStr[ediX - 1] == 0) ediStr[ediX] = 0;
          ediStr[ediX - 1] = c;
          ediX += 1;
          if (ediX > ediLen) ediX = ediLen;
          ediDraw();
          ediDirty = true;
      } // case
    } //avail
  } //while
  return 0;
}
