/* M3 only
   a..z to be coded
   DCBA  Walzenlage e.g. 2A selects Walze 2 for rightmost
   LKJI  Ringstellung e.g. 13K selects Ringstellung 13 for leftmost on M3
   Vab...V  SteckVerbind (=0 and e.g. VklV or VklitnpvzV, VaaV zeros a)
   ü+#äö debug
*/

const static unsigned char PROGMEM walze [9][28] = { //null terminated 0..25, 26=Ü 27=0
  {"ABCDEFGHIJKLMNOPQRSTUVWXYZ "}, // 0 leer
  {"EKMFLGDQVZNTOWYHXUSPAIBRCJQ"}, // 1
  {"AJDKSIRUXBLHWTMCQGZNPYFVOEE"}, // 2
  {"BDFHJLCPRTXVZNYEIWGAKMUSQOV"}, // 3
  {"ESOVPZJAYQUIRHXLNFTGKDCMWBJ"}, // 4
  {"VZBRGITYUPSDNHLXAWMJQOFECKZ"}, // 5
  {"EJMZALYXVBWFCRQUONTSPIKHGD "}, // 6 UKW A
  {"YRUHQSLDPXNGOKMIEBFZCWVJAT "}, // 7 UKW B
  {"FVPJIAOYEDRZXWGCTKUQSBNMHL "}, // 8 UKW C
};

const byte ledPin = 13;
unsigned long currTim;
unsigned long nexTim = 0;
unsigned long tick = 500;
uint16_t inp;
bool inpAkt;
// current coding
byte cod [9][28]; // stck,1,2,3,UKW,3,2,1,stck
byte cnt[4]; // counter,      valid only 1,2,3; 5=3,6=2,7=1
byte rng[4]; // ringstellung,      ""
char mode;  // ' ' neutral, 'V' during steck
bool verbo = true;

// print from Flash:
void prnt(PGM_P p) {
  while (1) {
    char c = pgm_read_byte(p++);
    if (c == 0) break;
    Serial.write(c);
  }
  Serial.write(" ");
}

void msgF(const __FlashStringHelper *ifsh, uint16_t n) {
  // text verbraucht nur flash
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  prnt(p);
  Serial.println(n);
}

void selectWalz(byte pos, byte num) {
  Serial.print(pos);

  for (byte k = 0; k < 27; k++) {
    cod[pos][k] = pgm_read_byte(&walze[num][k]) - 'A';
    if (verbo) {
      Serial.print(char(cod[pos][k] + 'a'));
    }
  }
  if (pos < 4) {
    cnt[pos] = 0;
  }
  msgF(F(" selectWalz "), num);
}

void reversWalz(byte pos, byte num) {
  byte b;
  Serial.print(pos);

  for (byte k = 0; k < 26; k++) {
    b = pgm_read_byte(&walze[num][k]) - 'A';
    if (verbo) {
      Serial.print(char(b + 'a'));
    }
    cod[pos][b] = k;
  }
  msgF(F(" reversWalz "), num);
}

byte walzCnt(byte num) {
  byte n; // counter to use
  switch (num) {
    case 4:   //
      n = 0;
      break;
    case 5:   //
      n = 3;
      break;
    case 6:   //
      n = 2;
      break;
    case 7:   //
      n = 1;
      break;
    case 8:   //
      n = 0;
      break;
    default:
      n = num;
  }
  return n;
}

void showCod(byte num) {
  char str[50];
  byte c = walzCnt(num);
  byte v = cnt[c]; // first value to show
  sprintf(str, "%2d %2d  ", num, v);
  Serial.print(str);
  for (byte k = 0; k < 27; k++) {
    Serial.print(char(cod[num][k] + 'a'));
    v += 1;
    if (v > 25) v = 0;
  }
  Serial.println();
}

void showAll() {
  Serial.println();
  for (byte k = 0; k < 9; k++) {
    showCod(k);
  }
}

void setupWalz() {
  selectWalz(0, 0); // selfSteck
  selectWalz(1, 1);
  selectWalz(2, 2);
  selectWalz(3, 3);
  selectWalz(4, 7); // UKW B
  reversWalz(5, 3);
  reversWalz(6, 2);
  reversWalz(7, 1);
  selectWalz(8, 0); // selfSteck
}

void incCnt() {
  bool carry;
  carry = (cnt[1] == cod[1][26]);
  cnt[1] += 1;
  if (cnt[1] > 25) {
    cnt[1] = 0;
  }
  if (!carry) return;
  carry = (cnt[2] == cod[2][26]);
  cnt[2] += 1;
  if (!carry) return;
  cnt[3] += 1;
  if (cnt[3] > 25)  cnt[3] = 0;
}

byte cod1(byte b, byte co, byte cn) {
  b = b + cnt[cn];
  if (b > 25) b -= 26;
  if (verbo) {
    Serial.print(char(b + 'a'));
  }
  b = cod[co][b];
  if (verbo) {
    Serial.print(char(b + 'a'));
    Serial.print(' ');
  }

  b = b  - cnt[cn];
  if (b > 200) b += 26; //negative
  return b;
}


byte encode(byte c) {
  // gets a..z returns a..z
  byte b = c - 'a';
  incCnt();
  b = cod[0][b];
  if (verbo) {
    Serial.print(char(b + 'a'));
    Serial.print(' ');
  }
  b = cod1(b, 1, 1);
  b = cod1(b, 2, 2);
  b = cod1(b, 3, 3);

  b = cod[4][b];  //UKW
  if (verbo) {
    Serial.print(char(b + 'a'));
    Serial.print(' ');
  }

  b = cod1(b, 5, 3);
  b = cod1(b, 6, 2);
  b = cod1(b, 7, 1);
  c = char(b + 'a');
  return c;
}

void doCmd( byte ch) {
  char d;
  //Serial.print (ch);
  if ((ch >= 'a') && (ch <= 'z')) {
    if (inpAkt) { // last entered was num
      ch = ch - 32;
      inpAkt = false; //also set below
    } else {
      d = encode(ch);
      Serial.print (d);
      return;
    }
  }
  if ( ch == ' ') {
    Serial.print (char(ch));
    return;
  }

  if ( ch == 8) { //backspace removes last digit
    inp = inp / 10;
    return;
  }

  if ((ch >= '0') && (ch <= '9')) {
    if (inpAkt) {
      inp = inp * 10 + (ch - '0');
    } else {
      inpAkt = true;
      inp = ch - '0';
    }
    return;
  }

  inpAkt = false;
  switch (ch) {
    case 'A':   //
      selectWalz(1, inp);
      reversWalz(7, inp);
      break;
    case 'B':   //
      selectWalz(2, inp);
      reversWalz(6, inp);
      break;
    case 'C':   //
      selectWalz(3, inp);
      reversWalz(5, inp);
      break;
    case 'D':   //
      selectWalz(4, inp);
      break;
    case 'H':   //
      selectWalz(0, inp);
      selectWalz(8, inp);
      break;
    case 'I':   //
      cnt[1] = inp;
      showCod(1);
      break;
    case 'J':   //
      cnt[2] = inp;
      showCod(2);
      break;
    case 'K':   //
      cnt[3] = inp;
      showCod(3);
      break;
    case '#':   //
      showCod(inp);
      break;
    case '+':   //
      verbo = not verbo;
      break;
    case 252:   // ü
      setupWalz();
      break;
    case 246:   // ö
      showAll();
      break;
    case 228:   // ä
      showAll();
      break;
    case 'T':
      showAll();
      break;
    default:
      Serial.println (byte(ch));
  } // case
}

void setup() {
  const char ich[] = "enigma " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  pinMode(ledPin, OUTPUT);
  setupWalz();
}

void loop() {
  unsigned char ch;
  if (Serial.available() > 0) {
    ch = Serial.read();
    doCmd(ch);
  } // avail
  currTim = millis();
  if (nexTim < currTim) {
    nexTim = currTim + tick;
  } // tick
}
