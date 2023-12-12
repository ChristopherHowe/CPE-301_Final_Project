/* Chris Howe and Neil Babu
 * Final Project
 */

//Note: these vars are only for reference
int resetPin = 44; // farthest from lights
int powerPin = 42; // near the lights
int disabledLEDPin = 49; // Yellow
int errorLEDPin = 47; // Red
int idleLEDPin = 45; // Green
int runningLEDPin = 43; //Blue


int reset = 5; // farthest from lights
int power = 7; // near the lights
int disabledLED = 0; // Yellow
int errorLED = 2; // Red
int idleLED = 4; // Green
int runningLED = 6; //Blue

// setup should have:
// ddr  01010101
// port 10100000
// pin  00000000


// Define Port L Register Pointers
volatile unsigned char* port_l = (unsigned char*) 0x10B; 
volatile unsigned char* ddr_l  = (unsigned char*) 0x10A; 
volatile unsigned char* pin_l  = (unsigned char*) 0x109; 

void setup() 
{
  //set PB7 to OUTPUT
  set_PL_as_output(disabledLED);
  set_PL_as_output(errorLED);
  set_PL_as_output(idleLED);
  set_PL_as_output(runningLED);
  set_PL_as_pullup_input(reset);
  set_PL_as_pullup_input(power);
}

void loop(){
  if (!read_pl(reset)) {
    write_pl(disabledLED,1);
    write_pl(errorLED,1);
  }
  else {
    write_pl(disabledLED,0);
    write_pl(errorLED,0);
  }
  if (!read_pl(power)) {
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
void set_PL_as_pullup_input(unsigned char pin_num)
{
    *ddr_l &= ~(0x01 << pin_num);
    *port_l |= 0x01 << pin_num;
}

bool read_pl(unsigned char pin_num){
  return *pin_l & 0x01 << pin_num;
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
