// (c) 2016 Felipe Correa da Silva Sanches <juca@members.fsf.org>
// Licensed under the General Public License, version 2 (or later)
//
// This is an emulator for the Patinho Feio computer.
// More info about this early 70's Brazilian pioneer computer
//  is available at https://github.com/felipesanches/PatinhoFeio/
//
// This emulator interfaces with a replica
// of the computer front-panel.
//
// The front panel contains:
// * 12 toggle-switches
// * a couple selectors with 2 positions each
// * 10 push-buttons (9 of which had gree LEDs as well)
// * 9 white LEDs
// * 62 red LEDs
//
// The LEDs are controlled by a chain of ten 8-bit shift-registers
//  (serial input, parallel output)
// The status of the push-buttons and toggle-switches will be
//  polled via a chain of three-bit shift-registers
//  (paralell input, serial output)

#define LED_REGISTER_CLK 3
#define LED_SERIAL_CLK 2
#define LED_SERIAL_DATA 4
#define NUM_LEDS 80

bool led[NUM_LEDS];

void setup() {
  pinMode(LED_SERIAL_CLK, OUTPUT);
  pinMode(LED_REGISTER_CLK, OUTPUT);
  pinMode(LED_SERIAL_DATA, OUTPUT);

  for (int i=0; i<NUM_LEDS; i++){
    led[i] = false;
  }
}

void send_LED_data(){
  for (int i=0; i<NUM_LEDS; i++){
    digitalWrite(LED_SERIAL_DATA, led[NUM_LEDS-1-i] ? HIGH : LOW);
    digitalWrite(LED_SERIAL_CLK, LOW);
    delay(1); //is this delay really needed?
    digitalWrite(LED_SERIAL_CLK, HIGH);
  }
  delay(1); //is this delay really needed?
  digitalWrite(LED_REGISTER_CLK, LOW);
  delay(1); //is this delay really needed?
  digitalWrite(LED_REGISTER_CLK, HIGH);
  delay(1); //is this delay really needed?
}

void sine_wave_demo() {
  static double t = 0;
  for (int i=0; i<NUM_LEDS; i++){
    led[i] = (i < (14 + 14.5 * sin(t*360)));
  }
  t+=0.001;
}

void random_blink_demo(){
  delay(10);
  int i = random(0, NUM_LEDS-1);
  led[i] = !led[i];
}

void loop() {
  sine_wave_demo();
//  random_blink_demo();

  send_LED_data();
}
