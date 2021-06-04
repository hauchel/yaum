// test sha1 to get rid of strings
// https://daknuett.github.io/cryptosuite2/usage_arduino.html#id2
#include <cluCom.h>
#include "sha1.h"

uint16_t  inp;
bool inpAkt;                  // true if last input was a digit

String lastblockhash = "";
String newblockhash = "";

char glastblock[45];
//byte lastblockP = 0;
char gnewblock[45];
//byte newblockP = 0;

uint16_t difficulty = 10;
uint16_t ducos1result = 0;
uint16_t ducos1tim = 0;
uint16_t testres = 0;

const uint16_t job_maxsize = 104; // 40+40+20+3 is the maximum size of a job
uint8_t job[job_maxsize];

uint16_t ducos1a(String lastblockhash, String newblockhash, uint16_t difficulty) {
  // DUCO-S1 algorithm implementation for AVR boards (DUCO-S1A)
  //newblockhash.toUpperCase();
  const char *c = newblockhash.c_str();
  size_t final_len = newblockhash.length() >> 1;
  for (size_t i = 0, j = 0; j < final_len; i += 2, j++) {
    job[j] = ((((c[i] & 0x1F) + 9) % 25) << 4) + ((c[i + 1] & 0x1F) + 9) % 25;
  }
  for (uint16_t ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++)  {
    Sha1.init();
    Sha1.print(lastblockhash + String(ducos1res));
    // Get SHA1 result
    uint8_t *hash_bytes = Sha1.result();
    if (memcmp(hash_bytes, job, SHA1_HASH_LEN * sizeof(char)) == 0)   {
      // If expected hash is equal to the found hash, return the result
      return ducos1res;
    }
  }
  return 0;
}

uint16_t calcsha1(const char *lastblock, const char *newblock, uint16_t difficulty) {
  uint8_t jj[SHA1_HASH_LEN];
  for (size_t i = 0, j = 0; j < SHA1_HASH_LEN; i += 2, j++) {
    jj[j] = ((((newblock[i] & 0x1F) + 9) % 25) << 4) + ((newblock[i + 1] & 0x1F) + 9) % 25;
  }
  for (uint16_t ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++)  {
    Sha1.init();
    Sha1.print(lastblock);
    Sha1.print(ducos1res);
    uint8_t * hash_bytes = Sha1.result();
    if (memcmp(hash_bytes, jj, SHA1_HASH_LEN * sizeof(char)) == 0)   {
      return ducos1res;
    }
  }
  return 0;
}


void hashme() {
  Serial.print(F("Hash me: "));
  unsigned long startTime = millis();
  // Call DUCO-S1A hasher
  ducos1result = ducos1a(lastblockhash, newblockhash, difficulty);
  // Calculate elapsed time
  ducos1tim = uint16_t( millis() - startTime);
  Serial.print(String(ducos1result) + "," + String(ducos1tim) + "," + "\n");
}

void calcme() {
  Serial.print(F("Calc me: "));
  unsigned long startTime = millis();
  ducos1result = calcsha1(glastblock, gnewblock, difficulty);
  ducos1tim = uint16_t( millis() - startTime);
  Serial.print(String(ducos1result) + "," + String(ducos1tim) + "," + "\n");
}


void testdata(uint8_t n) {
  difficulty = 6;
  if (n == 1) {
    lastblockhash = "ccd8d1e580ba517b85c1fa6e2295ec93c0ee0fbd";
    newblockhash =  "29f548079853d5e9514863b370ff75338a7ab919";
    testres = 342;
    return;
  }
  if (n == 2) {
    lastblockhash = "7c54338b487a53970cdd2882bee5221128417635";
    newblockhash = "403d2fb262c14bbddc980fae703739eeb55a95af";
    testres = 549;
    return;
  }
  if (n == 3) {
    lastblockhash = "6b41100f772d34674947145e0ace91f15667614b";
    newblockhash = "51eac4263f3014ec3a4f4644d0c5e00e1217683c";
    testres = 314;
    return;
  }
  if (n == 4) {
    lastblockhash = "c96e5571a211e356857e0db05e470f559ff012c8";
    newblockhash = "81bddf5254185466cae553c44cfa38f32d45e09f";
    testres = 211;
    return;
  }
  if (n == 5) {
    lastblockhash = "ae7ed806933ac562540d19c25218890e86e614cb";
    newblockhash = "4c72843f9d2a21baf7a3a8cdbcfff60ff7e0448a";
    testres = 354;
    return;
  }
  if (n == 6) {
    lastblockhash = "ae7ed806933ac562540d19c25218890e86e614cb";
    newblockhash = "f5e6349595eed1d4f979dc5a5075d8f804b35e79";
    testres = 464;
    return;
  }
  if (n == 7) {
    lastblockhash = "838ce53907bf01b5558c748965e622f8c7305c29";
    newblockhash = "213d54f87f2b75123edec1fa4ab41c937190c470";
    testres = 587;
    return;
  }
  if (n == 8) {
    lastblockhash = "792fd053b57c3511e2854d5faac8fa629bfe6b04";
    newblockhash = "1fafbd030104367c92d1e54087dc31ae8819c7d9";
    testres = 531;
    return;
  }
  if (n == 9) {
    lastblockhash = "7f09c6ebd8dd725dbd992918e9dcd2cae20b8f5e";
    newblockhash = "f6b7b5c86245f62c2999c3a568c661388327a72e";
    testres = 205;
    return;
  }
  msgF(F("No testdata "), n);
}

void convert() {
  String tmp;
  tmp = newblockhash;
  tmp.toCharArray(gnewblock, 41);
  tmp = lastblockhash;
  tmp.toCharArray(glastblock, 41);
}

void info() {
  Serial.println (F("      01234567890123x56789o1234567x90123456789"));
  Serial.print (F("Last  "));
  Serial.println (lastblockhash);
  Serial.print (F("LastC "));
  Serial.println (glastblock);
  Serial.print (F("New   "));
  Serial.println (newblockhash);
  Serial.print (F("NewC  "));
  Serial.println (gnewblock);
  Serial.print (F("Diffi "));
  Serial.println (difficulty);
  Serial.println ();
}

void doCmd( char ch) {
  bool weg = false;
  Serial.print (ch);
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
  Serial.print ("\b");
  switch (ch) {
    case 'c':   //
      calcme();
      break;
    case 'h':   //
    case 'H':   //
      hashme();
      break;
    case 'i':   //
      info();
      break;
    case 't':   //
      msgF(F("testdata"), inp);
      testdata(inp);
      msgF(F("Expected"), testres);
      convert();
      break;

    default:
      msgF(F(" ?? "), ch);
  } // case
}


void setup() {
  const char ich[] = "shatest " __DATE__  " "  __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
}

void loop() {
  char ch;
  if (Serial.available() > 0) {
    ch = Serial.read();
    doCmd(ch);
  } // avail

  currTim = millis();
  if (nexTim < currTim) {
    nexTim = currTim + tick;
  } // tick
}
