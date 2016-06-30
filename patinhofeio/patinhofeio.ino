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
bool _VAI_UM;
bool _TRANSBORDO;
int _RE; //12-bit "Registrador de Endereço" = Address Register
int _RD; // 8-bit "Registrador de Dados" = Data Register
int _RI; // 8-bit "Registrador de Instrução" = Instruction Register
int _ACC;// 8-bit "Acumulador" = Accumulator Register
int _CI; //12-bit "Contador de Instrução" = Instruction Counter
int _DADOS_DO_PAINEL; //12-bit of data provided by the user
                      //via panel toggle-switches
int _FASE; //determines which cycle of a cpu instruction
          //we are currently executing
bool _PARADO; //CPU is stopped.
bool _EXTERNO; //CPU is waiting interrupt.
int _MODO; //CPU operation modes:
#define NORMAL 1 //normal execution
#define CICLO_UNICO 2 //single-cycle
#define INSTRUCAO_UNICA 3//single-instruction
#define ENDERECAMENTO 4//addressing mode
#define ARMAZENAMENTO 5//data write mode
#define EXPOSICAO 6//data read mode

void DADOS_DO_PAINEL(int value){
  _DADOS_DO_PAINEL = value;
  for (int i=0; i<12; i++){
    led[0 + i] = (value & (1 << i));
  }
}

void VAI_UM(bool value){
  _VAI_UM = value;
  led[12] = value;
}

void TRANSBORDO(bool value){
  _TRANSBORDO = value;
  led[13] = value;
}

void PARADO(bool value){
  // This represents that the CPU is stopped.
  // Only a startup command ("PARTIDA") can restart it.
  _PARADO = value;
  led[14] = value;
}

void EXTERNO(bool value){
  // This represents that the CPU is stopped
  // waiting for an interrupt from an external device.
  _EXTERNO = value;
  led[15] = value;
}

void CI(int value){
  _CI = value;
  for (int i=0; i<12; i++){
    led[16 + i] = (value & (1 << i));
  }
}

void RE(int value){
  _RE = value;
  for (int i=0; i<12; i++){
    led[28 + i] = (value & (1 << i));
  }
}

void RD(int value){
  _RD = value;
  for (int i=0; i<8; i++){
    led[40 + i] = (value & (1 << i));
  }
}

void RI(int value){
  _RI = value;
  for (int i=0; i<8; i++){
    led[48 + i] = (value & (1 << i));
  }
}

void ACC(int value){
  _ACC = value;
  for (int i=0; i<8; i++){
    led[56 + i] = (value & (1 << i));
  }
}

void FASE(int value){
  _FASE = value;
  for (int i=1; i<=7; i++){
    led[64 + (i-1)] = (_FASE == i);
  }
}

void MODO(int value){
  _MODO = value;
  for (int i=1; i<=6; i++){
    led[71 + (i-1)] = (_MODO == i);
  }
}

void LED_ESPERA(bool value){
  //I think this button did not really have a lamp
  led[77] = value;
}

void LED_INTERRUPCAO(bool value){
  //I think this button did not really have a lamp
  led[78] = value;
}

void LED_PREPARACAO(bool value){
  //I think this button did not really have a lamp
  led[79] = value;
}

void reset_CPU(){
  VAI_UM(false);
  TRANSBORDO(false);
  CI(0x000);
  RE(0x000);
  RD(0x00);
  RI(0x00);
  ACC(0x00);
  DADOS_DO_PAINEL(0x000);
  FASE(1);
  PARADO(true);
  EXTERNO(false);
  MODO(NORMAL);
  LED_ESPERA(false);
  LED_INTERRUPCAO(false);
  LED_PREPARACAO(false);
}

void setup() {
  pinMode(LED_SERIAL_CLK, OUTPUT);
  pinMode(LED_REGISTER_CLK, OUTPUT);
  pinMode(LED_SERIAL_DATA, OUTPUT);
  reset_CPU();
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

void register_LEDs_demo() {
  //let's do a sine-wave effect on the
  //12-bit panel data register:
  static double t = 0;
  int value = 0;
  for (int i=0; i<12; i++){
     if (i < (6 + 6 * sin(t*360)))
       value |= (1 << i);
  }
  t+=0.001;
  DADOS_DO_PAINEL(value);

  //the address register will have the a mirrored sine-wave
  //that's why we flip the bits here:
  RE(~value);

  //and let's display an incremental count at
  // the instruction counter register:
  static int count = 0;
  CI(count);

  //while the overflow and carry bits will
  //alternate their blinking:
  VAI_UM((count/10)%2 == 0);
  TRANSBORDO((count/10)%2 == 1);

  //and the same kind of effect for these lights:
  PARADO((count/6)%2 == 0);
  EXTERNO((count/6)%2 == 1);

  //These will blink a lot :-P
  LED_ESPERA(count%2 == 0);
  LED_PREPARACAO(count%2 == 0);
  LED_INTERRUPCAO(count%2 == 0);

  //Finally, these sets of lights will turn on sequentially:
  FASE((count/5)%7 + 1);
  MODO((count/10)%6);

  count = (count+1)%(12*10*15*1000); // This wrap-around value is a multiple of
                                     // the period of all demo-effects
}

void loop() {
//  sine_wave_demo();
//  random_blink_demo();
  register_LEDs_demo();

  send_LED_data();
}
