/* Chris Howe and Neil Babu
 * Final Project
 */
// Used to configure interrupts.
int resetPin = 3; // farthest from lights
int powerPin = 2; // near the lights
//Note: these vars are only for reference
int disabledLEDPin = 49; // Yellow
int errorLEDPin = 47; // Red
int idleLEDPin = 45; // Green
int runningLEDPin = 43; //Blue


int reset = 5; // farthest from lights
int power = 4; // near the lights
int disabledLED = 0; // Yellow
int errorLED = 2; // Red
int idleLED = 4; // Green
int runningLED = 6; //Blue

// Define Port L Register Pointers - used to control the LEDs
volatile unsigned char* port_l = (unsigned char*) 0x10B; 
volatile unsigned char* ddr_l  = (unsigned char*) 0x10A; 
volatile unsigned char* pin_l  = (unsigned char*) 0x109; 

// Define Port E Register Pointers - used to control the interrupt buttons
volatile unsigned char* port_e = (unsigned char*) 0x2E; 
volatile unsigned char* ddr_e  = (unsigned char*) 0x2D; 
volatile unsigned char* pin_e  = (unsigned char*) 0x2C; 

bool on = false;
bool idle = false;
bool triggered = false;

void setup() 
{
  //set PB7 to OUTPUT
  set_PL_as_output(disabledLED);
  set_PL_as_output(errorLED);
  set_PL_as_output(idleLED);
  set_PL_as_output(runningLED);
  set_PE_as_pullup_input(reset);
  set_PE_as_pullup_input(power);
  attachInterrupt(digitalPinToInterrupt(resetPin), handleReset, CHANGE);
  attachInterrupt(digitalPinToInterrupt(powerPin), handlePower, CHANGE);
}

void handlePower(){
  if (read_pe(power)){
    on = !on;
  }
}

void handleReset(){
  if (read_pe(reset)){
    // set state to idle;
    idle=true;
  }
}

void loop(){
  if (idle) {
    write_pl(disabledLED,1);
    write_pl(errorLED,1);
    delay(5000);
    idle=false;
  }
  else {
    write_pl(disabledLED,0);
    write_pl(errorLED,0);
  }
  if (on) {
    write_pl(idleLED,1);
    write_pl(runningLED,1);
  }
  else {
    write_pl(idleLED,0);
    write_pl(runningLED,0);
  }
}
void set_PL_as_output(unsigned char pin_num)
{
    *ddr_l |= 0x01 << pin_num;
}
void set_PE_as_pullup_input(unsigned char pin_num)
{
    *ddr_e &= ~(0x01 << pin_num);
    *port_e |= 0x01 << pin_num;
}

bool read_pe(unsigned char pin_num){
  return *pin_e & 0x01 << pin_num;
}

void write_pl(unsigned char pin_num, unsigned char state)
{
  if(state == 0)
  {
    *port_l &= ~(0x01 << pin_num);
  }
  else
  {
    *port_l |= 0x01 << pin_num;
  }
}
