#include "sha1.h"
#include "uniqueID.h"
// Create globals
String lastblockhash = "";
String newblockhash = "";
String DUCOID = "";
uint16_t difficulty = 0;
uint16_t ducos1result = 0;
const uint16_t job_maxsize = 104; // 40+40+20+3 is the maximum size of a job
uint8_t job[job_maxsize];

// Setup stuff
void setup() {
  // Prepare built-in led pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  DUCOID = get_DUCOID();
}

// DUCO-S1A hasher
uint16_t ducos1a(String lastblockhash, String newblockhash, uint16_t difficulty)
{
  // DUCO-S1 algorithm implementation for AVR boards (DUCO-S1A)
  newblockhash.toUpperCase();
  const char *c = newblockhash.c_str();
  size_t final_len = newblockhash.length() >> 1;
  for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
    job[j] = ((((c[i] & 0x1F) + 9) % 25) << 4) + ((c[i + 1] & 0x1F) + 9) % 25;

  // Difficulty loop
  for (uint16_t ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++)
  {
    Sha1.init();
    Sha1.print(lastblockhash + String(ducos1res));
    // Get SHA1 result
    uint8_t *hash_bytes = Sha1.result();
    if (memcmp(hash_bytes, job, SHA1_HASH_LEN) == 0)
    {
      // If expected hash is equal to the found hash, return the result
      return ducos1res;
    }
  }
  return 0;
}

// Grab Arduino chip DUCOID
String get_DUCOID() {
  String ID = "DUCOID";
  char buff[4];
  for (size_t i = 0; i < 8; i++)
  {
    sprintf(buff, "%02X", (uint8_t) UniqueID8[i]);
    ID += buff;
  }
  return ID;
}

void onehash() {
  memset(job, 0, job_maxsize);
  lastblockhash = "2e72b94f686b57c74df859ac15f25b5f89840a31";
  newblockhash = "c0ccd4b92f92ae88449d3a3f70939f64e4c374b1";
  //  lastblockhash = "2e72b94f686b57c74df859ac15f25b5f89840a31";
  //  newblockhash = "4ad1ceb22892ef1efee0d4bfdb0bd03be6ad32fc";
  //  lastblockhash = "aeffe46c4e091373caacf8227d1db05a2d16f4f9";
  //  newblockhash = "5dd0ffeafa0cb50a092ce5f9612635c1e5b0a78d";

  // Read difficulty
  difficulty = 5;
  // Start time measurement
  uint32_t startTime = micros();
  // Call DUCO-S1A hasher
  ducos1result = ducos1a(lastblockhash, newblockhash, difficulty);
  // Calculate elapsed time
  uint32_t elapsedTime = micros() - startTime;
  // Clearing the receive buffer before sending the result.
  // Turn on built-in led
  PORTB = PORTB | B00100000;
  // Wait a bit
  delay(25);
  // Turn off built-in led
  PORTB = PORTB & B11011111;
}
// Infinite loop
void loop() {

}
