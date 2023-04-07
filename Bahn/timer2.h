// Stepper control by timer2
uint16_t volatile tim2Count = 0;    // steps remaining
byte volatile tim2CompB = 0;        // debug
byte tim2cs = 6;                    // clock divider
uint16_t volatile stePos = 0;       // position 0..
int16_t steRicht = 0;               // -1 dec 1 inc
bool steDirPlus = true;             // set steDir for increasing position
volatile byte steRamCount = 0;      // Rampe up count
volatile byte steRamDelt = 4;       // Rampe delta
volatile byte steOCR2A = 150;       // OCR2A start
byte steRamAnz = 5;               // Rampe
const byte steEna = 7; //hi disabled, orange      1
const byte steStp = 8; //lo>hi clocks, yell       7
const byte steDir = 9; //green                    8

void timer2Init() {
  // Timer for ctc mode 2: counts to OCR2A, int at OCR2B and OCR2A
  TCNT2  = 0;
  // 7      6       5       4       3       2       1       0
  // COM2A1 COM2A0  COM2B1  COM2B0  –       –       WGM21   WGM20   A
  // 0      0       0       0       0       0       1       0
  TCCR2A = 0x02;
  // FOC2A  FOC2B   –       –       WGM22   CS22    CS21    CS20    B
  // 0      0       0       0       0       c       c       c
  TCCR2B = tim2cs;
  //                                        OCIE2B  OCIE2A  TOIE2   TIMSK2
  // 0      0       0       0       0       1       0       0
  TIMSK2 = 4;                                    // enable Timer interrupts
  OCR2A = steOCR2A;
  OCR2B = 5;
  //ASSR  = 0;
  //GTCCR = 0;
  sei();  //set global interrupt flag to enable interrupts??
}

ISR(TIMER2_OVF_vect) {
}

ISR(TIMER2_COMPA_vect) {
}

ISR(TIMER2_COMPB_vect) {
  digitalWrite(steStp, HIGH);
  tim2CompB = 1;
  if (tim2Count > 0) {
    tim2Count--;
    stePos += steRicht;
    /*
      if (tim2Count<steRamAnz) { // bremsen
      OCR2A=OCR2A+steRamDelt;
      }
      if (steRamCount>0) {      // beschl.
      steRamCount--;
      OCR2A=OCR2A-steRamDelt;
      }
    */
    digitalWrite(steStp, LOW);
  } else {
    TCCR2B = 0; //stop timer
  }
}

void stePosition(uint16_t neu) {
  char str[50];
  if (neu == stePos) return;
  if (neu > stePos) {
    steRicht = 1;
    digitalWrite(steDir, steDirPlus);
    tim2Count = neu - stePos;
  } else {
    steRicht = -1;
    digitalWrite(steDir, !steDirPlus);
    tim2Count = stePos - neu;
  }
/*  if (verbo) {
    sprintf (str, "pos %4u, neu %4u, tim2C %4u", stePos, neu, tim2Count);
    Serial.println(str);
  }
  */
  steRamCount = steRamAnz;
  OCR2A = steOCR2A;
  TCCR2B = tim2cs; //start timer
}

void steSetup() {
  pinMode(steEna, OUTPUT);
  digitalWrite(steEna, HIGH);
  pinMode(steStp, OUTPUT);
  digitalWrite(steStp, LOW);
  pinMode(steDir, OUTPUT);
  timer2Init();
}
