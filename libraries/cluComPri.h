// common stuff required primes and 64-bit. 
typedef union {
  uint64_t za64;
  uint32_t za32[2];
  uint16_t za16[4];
  uint8_t  za08[8];
} zahl_t;

typedef struct {      //128 byte chunk 2*64 (== uint8_t ramBuffer[SPM_PAGESIZE])
  uint16_t za16[64];
} chu16_t;

typedef struct {      // 32*4
  uint32_t za32[32];
} chu32_t;

typedef struct {      // 16*8
  uint64_t za64[16];
} chu64_t;

typedef union {
  chu16_t chu16;
  chu32_t chu32;
  chu64_t chu64;
  uint8_t ramBuffer[SPM_PAGESIZE];
} chunk_t ;
chunk_t chunk;

void format64(char * buf, uint64_t n64) {
  //format with separating char; provide buf at least [30] else ...
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

void msg64(const __FlashStringHelper *ifsh, uint64_t num) {
  // llu's
  char str[30];
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  prnt(p);
  format64(str, num);
  Serial.println(str);;
}

void zeigZahl64(uint64_t num) {
  char str[100];
  const char txt[] = {"  "};
  zahl_t zahl;
  zahl.za64 = num;
  Serial.println();
  Serial.print(F("    64  : "));
  format64(str, zahl.za64);
  Serial.println(str);
  sprintf(str, "%s  2*32:  %10lu  %10lu", txt, zahl.za32[0], zahl.za32[1]);
  Serial.println (str);
  sprintf(str, "%s  4*16: %5u %5u %5u %5u ", txt, zahl.za16[0], zahl.za16[1], zahl.za16[2], zahl.za16[3]);
  Serial.println (str);
  sprintf(str, "%s  8*8X: %2X %2X %2X %2X %2X %2X %2X %2X", txt, zahl.za08[0], zahl.za08[1], zahl.za08[2], \
          zahl.za08[3], zahl.za08[4], zahl.za08[5], zahl.za08[6], zahl.za08[7]);
  Serial.println (str);
}


void showChu16() {
  // show chunk for 2 byte values
  char str[50];
  for (byte k = 0; k < 64; k++) {
    sprintf(str, "%6u", chunk.chu16.za16[k]);
    if ((k % 8) == 0) Serial.println();
    Serial.print (str);
    if (chunk.chu16.za16[k] == 0) break;
  }
  Serial.println();
}

void showChu64() {
  // show chunk for 8 byte values
  char str[50], str1[50];
  for (byte k = 0; k < 16; k++) {
    format64(str1, chunk.chu64.za64[k]);
    sprintf(str, "  %20s", str1 );
    if ((k % 4) == 0) Serial.println();
    Serial.print (str);
    if (chunk.chu64.za64[k] == 0) break;
  }
  Serial.println();
}
