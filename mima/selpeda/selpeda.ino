#define KEY_COUNT       6
#define KEY_EMPTY       0xff
uint8_t key_pins[KEY_COUNT] = { 4,  5,  6,  7,  8,  9};
uint8_t led_pins[KEY_COUNT] = {10, 11, 12, 13, A0, A1};
bool key_input[KEY_COUNT];
uint8_t key_order[KEY_COUNT];

// HH begin
bool verbo = true;
void showListe() {
  char str[80];
  byte i;
  Serial.println(" #  inp  order");
  for (i = 0; i < KEY_COUNT; i++) {
    sprintf(str, "%2u   %1s     %2u  %2u ", i, key_input[i] ? "T" : "F", key_order[i]);
    Serial.println(str);
  }
}

void doCmd(byte tmp) {
  switch (tmp) {
    case 's':   //
      showListe();
      break;
    case 'v':   //
      verbo = !verbo;
      if (verbo) {
        Serial.println("Verbose an");
      } else {
        Serial.println("Verbose aus");
      }
      break;
    default:
      Serial.print(tmp);
      Serial.println ("?  0..5, +, -, show, verbose");
  } //case
}



void setup(void)
{
  const char info[] = "Sel6Peda " __DATE__  " " __TIME__;
  Serial.begin(38400);
  Serial.println(info);
  for (uint8_t i = 0; i < KEY_COUNT; i++)
    key_order[i] = KEY_EMPTY;
  for (uint8_t  i = 0; i < KEY_COUNT; i++)  //HH
  {
    pinMode(key_pins[i], INPUT_PULLUP);
    pinMode(led_pins[i], OUTPUT);
  }
}

static void input_keys(void)
{
  for (uint8_t i = 0; i < KEY_COUNT; i++)
    key_input[i] = ! digitalRead( key_pins[i] );  // low is key pressed
}

static void remove_first_key(uint8_t key)
{
  if (key_order[0] != key)                      // not at first
    return;
  for (uint8_t i = 0; i < KEY_COUNT - 1; i++)   // copy down
    key_order[i] = key_order[i + 1];
  key_order[KEY_COUNT - 1] = KEY_EMPTY;
}

static void insert_next_key(uint8_t key)
{
  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
    if (key_order[i] == key)                    // already in list
      break;
    if (key_order[i] == KEY_EMPTY)              // insert here
    {
      key_order[i] = key;
      break;
    }
  }
}

static void handle_keys(void)
{
  for (uint8_t i = 0; i < KEY_COUNT; i++)
    if (key_input[i] == true)
      insert_next_key(i);
    else
      remove_first_key(i);
}

static void first_led_on(void)
{
  for (uint8_t i = 0; i < KEY_COUNT; i++)
    digitalWrite(led_pins[i], i == key_order[0]);       // only first LED on
}

void loop(void)
{

  if (Serial.available() > 0) {
    doCmd(Serial.read());
  }

  input_keys();
  handle_keys();
  first_led_on();
}
