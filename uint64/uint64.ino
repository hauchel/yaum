// Test sprintf uint64_t
uint8_t  my08 = 255;
uint16_t my16 = 65535;
uint32_t my32 = 4294967295UL;
uint64_t my64 = 18446744073709551615ULL;

typedef union {
  uint64_t za64;
  uint32_t za32[2];
  uint16_t za16[4];
  uint8_t  za08[8];
} zahl_t;
zahl_t zahl;

void zeigzahl(const char txt[]) {
  char str[100];
  Serial.println();
  sprintf(str, "%-12s  1*64:  %llu", txt, zahl.za64);
  Serial.println (str);
  sprintf(str, "%-12s  2*32:  %10lu  %10lu", txt, zahl.za32[0], zahl.za32[1]);
  Serial.println (str);
  sprintf(str, "%-12s  4*16: %5u %5u %5u %5u ", txt, zahl.za16[0], zahl.za16[1], zahl.za16[2], zahl.za16[3]);
  Serial.println (str);
  sprintf(str, "%-12s  8*8X: %2X %2X %2X %2X %2X %2X %2X %2X", txt, zahl.za08[0], zahl.za08[1], zahl.za08[2], \
          zahl.za08[3], zahl.za08[4], zahl.za08[5], zahl.za08[6], zahl.za08[7]);
  Serial.println (str);
}

void setup() {
  const char ich[] = "uint64 " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
}

void format64(char * buf, uint64_t n64) {
  //format with separating char; provide buf at least 30 else ...
  const byte len = 30;
  char str[len];
  uint64_t rem;
  byte p = len - 1;
  byte cc = 4;
  str[p--] = 0;
  while (p > 0) {
    rem = n64 % 10;
    cc--;
    if (cc == 0) {
      str[p--] = '.';
      cc = 3;
    }
    str[p--] = byte(rem) + '0';
    n64 = (n64 - rem) / 10;
    if (n64 == 0) break;
  }
  p++;
  for (byte k = 0; k < len; k++) {
    buf[k] = str[p++];
    if (buf[k] == 0) break;
  }
}

void check64(uint64_t n64) {
  // test format64
  char strI[100];
  char strO[100];
  format64(strI, n64);
  sprintf (strO, "%-26s<<", strI);
  Serial.println(strO);
  sprintf (strO, "%26s<<", strI);
  Serial.println(strO);
  sprintf (strO, "%12s<<", strI);
  Serial.println(strO);
}

void loop() {
  char str[100];
  sprintf (str, "my32 lu exp 4294967295: %10lu<<", my32);
  Serial.println(str);
  sprintf (str, "my64 llu exp 18446744073709551615: %llu<<", my64);
  Serial.println(str);
  //sprintf (str, "%"PRIu64" ", my64);
  //Serial.println(str);
  zahl.za64 = my64;
  zeigzahl("my64");
  zahl.za64 = 255;
  zeigzahl("255");
  zahl.za64 = 4711;
  zeigzahl("4711");
  zahl.za64 = UINT64_MAX;
  zeigzahl("max64");
  zahl.za64 = UINT32_MAX;
  zeigzahl("max32");

  check64(0);
  check64(123);
  check64(1000000);
  check64(1234);
  check64(9876543210);
  check64(my64);

  while (Serial.available() == 0);
  Serial.read();
}
