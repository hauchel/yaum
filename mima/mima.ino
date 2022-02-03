// MischMaschine
//
#include <ServoTimer2.h>  // the servo library timer 2
#include <EEPROM.h>

int cnt = 0;
int cnts = 100;         // timer counts down cnt until 0
int stepPos = 0;         // current stepper position
int stepPtr = 0;         // pointe stepper position
int servPos = 1200;         // current servo position
const byte anzSoll = 7;
int stepSoll[anzSoll];
int servSoll[anzSoll];

bool upp = true;      // direction stepper
int dauer = 0;        // 1=controlled by analog inputs
int eadr = 0;       // EEprom Addr
byte state = 0;
byte stateOld = 0;
int lop;

ServoTimer2 greifer;
unsigned long zeit;
unsigned long watch;  // watchdog timer

// analog
int av1, av2, oav1, oav2;
// tim1 begin
byte cs20, comab, wgm10, wgm11, wgm12, wgm13;
const byte servoPin = 4;  // pink
const byte rePin = 5;     // white   input rechts switch low active
const byte liPin = 6;     // white   input links switch low active
const byte enaPin = 7;    // green   high disabled
const byte stepPin = 8;   // yell    low-> hi advance
const byte dirPin = 9;    // orange  whatever
const byte trigPin = 10;  // green to lifter
const byte liftPin = 11;  // yell from lifter

void timer1CrAB() {
  TCCR1A =  comab |
            (wgm11 <<  WGM11) |
            (wgm10 <<  WGM10) ;

  TCCR1B = (0 <<  ICNC1) |
           (0 <<  ICES1) |
           (wgm13 <<  WGM13) |
           (wgm12 <<  WGM12) |
           cs20;

  Serial.print("Timer1  Control  ");
  Serial.print(TCCR1A, BIN);
  Serial.print("    ");
  Serial.println(TCCR1B, BIN);
}

/* timer1 different modes
  define
  comab   output OC1A/OC1B
  wgm     waveform
  clock   speed
  timsk   ints

*/
void timer1CTC() {
  // count no outpus
  wgm13 = 0;
  wgm12 = 1;
  wgm11 = 0;
  wgm10 = 0;
  comab =   (0 << COM1A1) |
            (0 << COM1A0) |
            (0 << COM1B1) |
            (0 << COM1B0) ;
}

void timer1Setup() {
  /*  CTC Mode
      COM1A1:0: Compare Output Mode for Channel A  Pin 9
      COM1B1:0: Compare Output Mode for Channel B  Pin 10 note that the Data Direction Register (DDR) bit corresponding
       cs20
      0 0 0 No clock source (Timer/Counter stopped).
      0 0 1 clkI/O/1 (No prescaling)
      0 1 0 clkI/O/8 (From prescaler)
      0 1 1 clkI/O/64 (From prescaler)
      1 0 0 clkI/O/256 (From prescaler)
      1 0 1 clkI/O/1024 (From prescaler)
      1 1 0 External clock source on T1 pin. Clock on falling edge.
      1 1 1 External clock source on T1 pin. Clock on rising edge.
  */
  cs20 = 2;
  timer1CTC();
  timer1CrAB();

  TCNT1  = 1234;
  // set compare match register
  OCR1A = 3248;
  OCR1B =  5000;
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  TIMSK1 |= (1 << OCIE1B);
  TIMSK1 |= (1 << TOIE1);
  Serial.println(TIMSK1, BIN);
  interrupts();
}

ISR(TIMER1_OVF_vect) {

}

ISR(TIMER1_COMPA_vect) {
  int tmp;
  if (cnt > 0) {
    digitalWrite(stepPin, HIGH);
    if (upp) {
      stepPos++;
      tmp = digitalRead(rePin);
      if (tmp == 0) {
        stepPos = 795;
        cnt = 1;
      }
    } else {
      stepPos--;
      tmp = digitalRead(liPin);
      if (tmp == 0) {
        stepPos = 0;
        cnt = 1;
      }
    }
    cnt --;
    digitalWrite(stepPin, LOW);
  } else {    //  Stopping
    if (cnt == 0) {
      digitalWrite(enaPin, HIGH);
      cnt = -1;
    }
  } // cnt
}

ISR(TIMER1_COMPB_vect) {
}

//ISR(TIMER1 CAPT_vect) {
//
//}

// tim1 end


void msn(const char txt[], int n) {
  Serial.print(txt);
  Serial.print(" ");
  Serial.print(n);
}
void msg(const char txt[], int n) {
  Serial.print(txt);
  Serial.print(" ");
  Serial.println(n);
}

void zero() {
  cnt = 0;
  digitalWrite(enaPin, HIGH);
  state = 0;
}

void toEpromInt(int v) {
  byte b;
  b = lowByte(v);
  EEPROM.update(eadr, b) ;
  eadr++;
  b = highByte(v);
  EEPROM.update(eadr, b) ;
  eadr++;
}

void toEprom() {
  eadr = 0;
  for (int i = 0; i < anzSoll; i++) {
    msg ("toEprom serv", servSoll[i]);
    toEpromInt( servSoll[i]);
  }
  for (int i = 0; i < anzSoll; i++) {
    msg ("toEprom step", stepSoll[i]);
    toEpromInt( stepSoll[i]);
  }
}

int fromEpromInt() {
  byte bl, bh;
  int tmp;
  bl = EEPROM.read(eadr) ;
  eadr++;
  bh = EEPROM.read(eadr) ;
  eadr++;
  tmp = 256 * bh + bl;
  return tmp;
}

void fromEprom() {
  eadr = 0;
  for (int i = 0; i < anzSoll; i++) {
    servSoll[i] = fromEpromInt();
    msg ("fromEprom serv", servSoll[i]);
  }
  for (int i = 0; i < anzSoll; i++) {
    stepSoll[i] = fromEpromInt();
    msg ("fromEprom step", stepSoll[i]);
  }
}

void readAn() {
  int di;
  bool change = false;
  av1 = analogRead(A1);
  di = abs(av1 - oav1);
  if (di > 3) {
    oav1 = av1;
    OCR1A = av1 * 8;
    Serial.print("OCR1A ");
    Serial.print(av1);
    Serial.print("  =  ");
    Serial.println(OCR1A);
    change = true;
  }
  av2 = analogRead(A2);
  di = abs(av2 - oav2);
  if (di > 3) {
    oav2 = av2;
    servPos = av2 * 2;
    msg("AV2 ", av2);
    change = true;
  }
}

void info() {
  int tmp;
  msn ("StepPos=", stepPos);
  msg(" StepPtr=", stepPtr);
  msg ("Servo=", servPos);
  tmp = digitalRead(liftPin);
  msn("Lift=", tmp);
  msg (" State=", state);
}

void stepLeft(int stp) {
  msg("stepLeft", stp);
  digitalWrite(dirPin, HIGH);
  upp = false;
  cnt = stp;
}

void stepRght(int stp) {
  msg("stepRght", stp);
  digitalWrite(dirPin, LOW);
  upp = true;
  cnt = stp;
}

void stepTo(int po) {
  int tmp;
  digitalWrite(enaPin, LOW);
  msg("stepTo=", po);
  msg("stepPos=", stepPos);
  if (po < stepPos ) {
    tmp = stepPos - po;
    stepLeft(tmp);
  } else {
    tmp =  po - stepPos ;
    stepRght(tmp);
  }
}

void waitCnt0() {
  while (cnt > 0) {
    msn(" c", cnt);
    delay(50);
  }
}

void greif(int po) {
  greifer.write(po);
  servPos = po;
  msg("greif ", servPos);
}

void docmd(char tmp) {
  if ((tmp >= '0') && (tmp <= '6')) {
    stepPtr = (tmp - 48);
    msg("stepPtr=", stepPtr);
    stepTo(stepSoll[stepPtr]);
  } else {
    switch (tmp) {
      case 'a':   //
        digitalWrite(enaPin, LOW);
        Serial.println("ena LOW, enabled");
        break;
      case 'b':   //
        digitalWrite(enaPin, HIGH);
        Serial.println("ena HIGH, disabled");
        break;
      case 'n':   //
        digitalWrite(trigPin, LOW);
        Serial.println("trig LOW");
        break;
      case 'm':   //
        digitalWrite(trigPin, HIGH);
        Serial.println("trig HIGH");
        break;
      case 'i':   //
        info();
        break;

      case 's':   //
        digitalWrite(enaPin, LOW);
        stepLeft(cnts);
        break;
      case 'd':   //
        cnt = 0;
        digitalWrite(enaPin, HIGH);
        break;
      case 'f':   //
        digitalWrite(enaPin, LOW);
        stepRght(cnts);
        break;

      case 'g':   //
        greifer.attach(servoPin);
        msg("attached", av2);
        break;
      case 'h':   //
        greifer.detach();
        msg("detached", 0);
        break;
      case 'j':   //
        greif(servSoll[0]);
        break;
      case 'J':   //
        servSoll[0] = servPos;
        msg("Set", 0);
        break;
      case 'k':   //
        greif(servSoll[1]);
        break;
      case 'K':   //
        servSoll[1] = servPos;
        msg("Set", 1);
        break;
      case 'l':   //
        greif(servSoll[2]);
        break;
      case 'L':   //
        servSoll[2] = servPos;
        msg("Set", 2);
        break;

      case 'e':   //
        fromEprom();
        break;
      case 'E':   //
        toEprom();
        break;

      case 'r':   //
        stepTo(stepPos - 10) ;
        break;
      case 't':   //
        stepTo(stepPos - 2) ;
        break;
      case 'z':   //
        stepTo(stepPos + 2) ;
        break;
      case 'u':   //
        stepTo(stepPos + 10) ;
        break;
      case 'o':   //
        stepTo(-999) ;
        break;
      case 'c':   //
        msg("State 1, was ", state) ;
        state = 1;
        break;
      case 'p':   //
        msg("State 0, was ", state) ;
        state = 0;
        break;
      case 'x':   //
        msg("State 5, was ", state) ;
        state = 5;
        break;
      case 'v':   //
        if (dauer == 0) {
          dauer = 1;
        }
        else {
          dauer = 0;
        }
        break;
      case 'y':   //
        stepSoll[stepPtr] = stepPos;
        msg("stepSoll of ", stepPtr);
        break;

      case '*':   //
        greif(servPos + 10);
        break;
      case '+':   //
        greif(servPos + 1);
        break;
      case '-':   //
        greif(servPos - 1);
        break;
      case '_':   //
        greif(servPos - 10);
        break;
      default:
        Serial.println("Servo: _-+* g h jkl JKL");
        Serial.println("StepEnable: a b rtzu o sdf  0..6 y");
        Serial.println("State p 0, c 1, x 5  n,m Trig");
    } // case
  } // else
}

void setup() {
  pinMode(enaPin, OUTPUT);   //high disabled
  digitalWrite(enaPin, HIGH);
  const char ich[] = "mima  "__DATE__ " " __TIME__;
  Serial.begin(38400);
  Serial.println(ich);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(liPin, INPUT_PULLUP);
  pinMode(rePin, INPUT_PULLUP);
  pinMode(liftPin, INPUT_PULLUP);
  pinMode(dirPin, OUTPUT);
  digitalWrite(dirPin, HIGH);
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, HIGH);

  pinMode(stepPin, OUTPUT);
  digitalWrite(stepPin, LOW);
  timer1Setup();
  fromEprom();
}

void loop() {
  char tmp;
  byte zwi;
  if (Serial.available() > 0) {
    Serial.println();
    tmp = Serial.read();
    docmd(tmp);
  } // avail
  zeit = millis();
  if (watch > 0) {
    if (watch < zeit) {
      msg("Watchdog State",state);
      watch=0;
    }
  }
  if (state != stateOld) {
    msn("|",state);
    stateOld=state;
  }
  switch (state) {
    case 0:   //  initial, disabled
      break;
    case 1:  // Just left
      stepTo(-999) ;
      waitCnt0();
      state = 5;
      break;
    case 5:  //  Step to 5
      greifer.attach(servoPin);
      greif(servSoll[1]);
      stepTo(stepSoll[5]) ;
      waitCnt0();
      state = 10;
      break;
    case 10:  // at 5, trigger
      digitalWrite(trigPin, LOW);
      watch=zeit+500;
      state = 15;
      lop=22;
      break;
    case 15:  // wait for lift ready
      digitalWrite(trigPin, HIGH);
      zwi = digitalRead(liftPin);
      if (zwi != 0) {
        lop--;
        if(lop<1) {
        state = 20;
        }
      } else {
        lop=20;
      }
      break;
    case 20:  // lift ready, moveto
      stepTo(stepSoll[6]) ;
      waitCnt0();
      greif(servSoll[0]);
      state = 25;
      watch=zeit+500;
      break;
    case 25:  // gripping
      break;
    default:
      msg ("state kaputt", state);
      state = 0;
  } // case state
  if (dauer != 0) {
    readAn();
    if (cnt >= 0) {
      msg("Cnt", cnt);
    }
    delay(100);
  }
}



















