// Stepper control by timer2


void timer2Init() {
  // Timer for ctc mode 2: counts to OCR2A, int at OCR2B and OCR2A
  // 7      6       5       4       3       2       1       0
  // COM2A1 COM2A0  COM2B1  COM2B0  –       –       WGM21   WGM20   TCCR2A
  // 0      0       0       0       0       0       1       0
  TCCR2A = 0x02;
  // FOC2A  FOC2B   –       –       WGM22   CS22    CS21    CS20    TCCR2B
  // 0      0       0       0       0       c       c       c
  TCCR2B = tim2cs;
  //                                        OCIE2B  OCIE2A  TOIE2   TIMSK2
  // 0      0       0       0       0       1       0       0
  TIMSK2 = 4;                                    // enable Timer interrupts
  TCNT2  = 0;
  //OCR2A = 250;
  //OCR2B = 5;
  //ASSR  = 0;
  //GTCCR = 0;
  sei();  //set global interrupt flag to enable interrupts??
}

ISR(TIMER2_OVF_vect) {
}

ISR(TIMER2_COMPA_vect) {
}

ISR(TIMER2_COMPB_vect) {
  tim2Busy = 1;
  for (byte i = 0; i < nStp; i++) {
    if (tim2Count[i] > 0) {
      digitalWrite(steStp[i], HIGH);
      tim2Count[i]--;
      stePos[i] += steRicht[i];
      tim2Busy++;
      digitalWrite(steStp[i], LOW);
    }
  }
  tim2CompB = TCNT2 ;
}

void steSetup() {
  timer2Init();
  OCR2A = 100;
  OCR2B = 5;
  TCCR2B = tim2cs; //start timer
}
