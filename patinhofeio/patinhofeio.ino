// (c) 2017 Felipe Correa da Silva Sanches <juca@members.fsf.org>
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

#define DEMO 0
#define LED_REGISTER_CLK 3
#define LED_SERIAL_CLK 2
#define LED_SERIAL_DATA 4
#define NUM_LEDS 80

#define INDEX_REG 0
#define RAM_SIZE 256

byte RAM[RAM_SIZE];
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

byte read_index_reg(){
  return RAM[INDEX_REG];
}

void write_index_reg(byte value){
  RAM[INDEX_REG] = value;
}

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

void load_example_hardcoded_program(){
  /*
    HELLO WORLD PROGRAM that prints
    "PATINHO FEIO" to the teletype:
  */

  RAM[0x06] = 0x80;
  RAM[0x07] = 0x9e;
  RAM[0x08] = 0x50;
  RAM[0x09] = 0x1c;
  RAM[0x0A] = 0xca;
  RAM[0x0B] = 0x80;
  RAM[0x0C] = 0xca;
  RAM[0x0D] = 0x21;
  RAM[0x0E] = 0x00;
  RAM[0x0F] = 0x0c;
  RAM[0x10] = 0x9e;
  RAM[0x11] = 0x85;
  RAM[0x12] = 0x20;
  RAM[0x13] = 0x00;
  RAM[0x14] = 0x60;
  RAM[0x15] = 0x1B;
  RAM[0x16] = 0xA0;
  RAM[0x17] = 0x08;
  RAM[0x18] = 0x9D;
  RAM[0x19] = 0x00;
  RAM[0x1A] = 0x06;
  RAM[0x1B] = 0xF2;
  RAM[0x1C] = 'P';
  RAM[0x1D] = 'A';
  RAM[0x1E] = 'T';
  RAM[0x1F] = 'I';
  RAM[0x20] = 'N';
  RAM[0x21] = 'H';
  RAM[0x22] = 'O';
  RAM[0x23] = ' ';
  RAM[0x24] = 'F';
  RAM[0x25] = 'E';
  RAM[0x26] = 'I';
  RAM[0x27] = 'O';
  RAM[0x28] = 0x0D;
  RAM[0x29] = 0x0A;

  //This will make it run
  //automatically one at boot:
  CI(0x006);
  PARADO(false);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_SERIAL_CLK, OUTPUT);
  pinMode(LED_REGISTER_CLK, OUTPUT);
  pinMode(LED_SERIAL_DATA, OUTPUT);

  for (int i=0; i<RAM_SIZE; i++){
    RAM[i] = 0;
  }
  reset_CPU();
  load_example_hardcoded_program();
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


#if DEMO
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
    if ((i+0.5) < (6 + 6 * sin(t*360)))
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
#endif

byte read_ram(int addr){
  return RAM[addr];
}

void write_ram(int addr, byte value){
  RAM[addr % RAM_SIZE] = value;
}

void inc_CI(){
  CI((_CI+1)%RAM_SIZE);
}

int compute_effective_address(int addr){
  //TODO: implement-me!
  return addr;
}

byte opcode;

/*******************************
CPU Instructions implementation
********************************/

void CARX_instruction(){
  /* OPCODE: 0x50 */
  //CARX = "Carga indexada": Load a value from a given indexed memory position into the accumulator
  int tmp = (opcode & 0x0F) << 8 | read_ram(_CI);
  inc_CI();
  int idx = read_index_reg();
  //TODO: compute_effective_address(m_idx + tmp);
  ACC(read_ram(idx + tmp));
}

void LIMPO_instruction(){
  /* OPCODE: 0x80 */
  //LIMPO:
  //     Clear accumulator and flags
  ACC(0);
  TRANSBORDO(0);
  VAI_UM(0);
}

void INC_instruction(){
  /* OPCODE: 0x85 */
  //INC:
  // Increment accumulator
  ACC(_ACC+1);
  TRANSBORDO(0); //TODO: fix-me (I'm not sure yet how to compute the flags here)
  VAI_UM(0); //TODO: fix-me (I'm not sure yet how to compute the flags here)
}

void PARE_instruction(){
  /* OPCODE: 0x9D */
  //PARE="Pare":
  //    Holds execution. This can only be recovered by
  //    manually triggering execution again by
  //    pressing the "Partida" (start) button in the panel
  PARADO(true);
  EXTERNO(false);
}

void TRI_instruction(){
  /* OPCODE: 0x9E */
  //TRI="Troca com Indexador":
  //     Exchange the value of the accumulator with the index register
  byte value = _ACC;
  ACC(read_index_reg());
  write_index_reg(value);
}

void XOR_instruction(){
  /* OPCODE: 0xD2 */
  //XOR: Computes the bitwise XOR of an immediate into the accumulator
  ACC(_ACC ^ read_ram(_CI));
  inc_CI();
  //TODO: update T and V flags
}

void NAND_instruction(){
  /* OPCODE: 0xD4 */
  //NAND: Computes the bitwise XOR of an immediate into the accumulator
  ACC(~(_ACC & read_ram(_CI)));
  inc_CI();
  //TODO: update T and V flags
}

void SOMI_instruction(){
  /* OPCODE: 0xD8 */
  //SOMI="Soma Imediato":
  //Add an immediate into the accumulator
  //TODO: set_flag(V, ((((int16_t) ACC) + ((int16_t) READ_BYTE_PATINHO(PC))) >> 8));
  //TODO: set_flag(T, ((((int8_t) (ACC & 0x7F)) + ((int8_t) (READ_BYTE_PATINHO(PC) & 0x7F))) >> 7) == V);
  ACC(_ACC + read_ram(_CI));
  inc_CI();
}

void PLA_instruction(){
  /* OPCODE: 0x0X */
  //PLA = "Pula": Jump to address
  int addr = (opcode & 0x0F) << 8 | read_ram(_CI);
  addr = compute_effective_address(addr);
  inc_CI();
  CI(addr);
}

void ARM_instruction(){
  /* OPCODE: 0x2X */
  //ARM = "Armazena": Store the value of the accumulator into a given memory position
  int addr = (opcode & 0x0F) << 8 | read_ram(_CI);
  addr = compute_effective_address(addr);
  inc_CI();
  write_ram(addr, _ACC);
}

void SOM_instruction(){
  /* OPCODE: 0x6X */
  //SOM = "Soma": Add a value from a given memory position into the accumulator
  int addr = (opcode & 0x0F) << 8 | read_ram(_CI);
  addr = compute_effective_address(addr);
  inc_CI();
  ACC(_ACC + read_ram(addr));
  //TODO: update V and T flags
}

void PLAN_instruction(){
  /* OPCODE: 0xAX */
  //PLAN = "Pula se ACC negativo": Jump to a given address if ACC is negative
  int addr = (opcode & 0x0F) << 8 | read_ram(_CI);
  addr = compute_effective_address(addr);
  inc_CI();
  if ((signed char) _ACC < 0){
    CI(addr);
  }
}

void SAI_instruction(byte channel){
  /* OPCODE: 0xCX 0x8X */
  /* SAI = "Output data to I/O device" */
  //TODO: handle multiple device channels: m_iodev_write_cb[channel](ACC);
  delay(1000/300); //This is REALLY BAD emulation-wise but it looks nice :-)
  Serial.write(_ACC);
}

void SAL_instruction(byte channel, byte function){
  /* OPCODE: 0xCX 0x2X */
  //SAL="Salta"
  //    Skips a couple bytes if a condition is met
  bool skip = false;
  switch(function){
    case 1:
      //TODO: implement-me! skip = (m_iodev_status[channel] == IODEV_READY);
      skip = true;
      break;
    case 2: 
      /* TODO:
         skip = false;
         if (! m_iodev_is_ok_cb[channel].isnull()
            && m_iodev_is_ok_cb[channel](0))
      */
      skip = true;
      break;
    case 4:
      /*TODO:
         skip =false;
         if (! m_iodev_IRQ_cb[channel].isnull()
            && m_iodev_IRQ_cb[channel](0) == true)
      */
      skip = true;
      break;
  }
  if (skip){
    inc_CI();
    inc_CI();
  }
}



void run_one_instruction(){
  unsigned int tmp;
  byte value, channel, function;
  opcode = read_ram(_CI);

  #ifdef DEBUG
  Serial.print("CI: ");
  Serial.print(_CI, HEX);
  Serial.print(" OPCODE: ");
  Serial.print(opcode, HEX);
  Serial.print(" Mascarado: ");
  Serial.println(opcode & 0xF0, HEX);
  #endif

  inc_CI();

  switch (opcode){
    case 0x50: CARX_instruction(); return;
    case 0x80: LIMPO_instruction(); return;
    case 0x85: INC_instruction(); return;
    case 0x9D: PARE_instruction(); return;
    case 0x9E: TRI_instruction(); return;
    case 0xD2: XOR_instruction(); return;
    case 0xD4: NAND_instruction(); return;
    case 0xD8: SOMI_instruction(); return;
  }

  switch (opcode & 0xF0){
    case 0x00: PLA_instruction(); return;
    case 0x20: ARM_instruction(); return;
    case 0x60: SOM_instruction(); return;
    case 0xA0: PLAN_instruction(); return;
    case 0xC0:
      //Executes I/O functions
      //TODO: Implement-me!
      value = read_ram(_CI);
      inc_CI();
      channel = opcode & 0x0F;
      function = value & 0x0F;
      switch(value & 0xF0){
        //TODO: case 0x10: ...
	    case 0x80: SAI_instruction(channel); return;
        case 0x20: SAL_instruction(channel, function); return;
      }
    default:
      Serial.print("OPCODE INVALIDO: ");
      Serial.println(opcode, HEX);
      PARADO(true);
      return;
    }
}


void loop() {
  if (!_PARADO){
    run_one_instruction();
  }

  send_LED_data();
}
