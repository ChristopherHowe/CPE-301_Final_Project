/* Chris Howe and Neil Babu
 * Final Project
 */



// Define Port L Register Pointers
volatile unsigned char* port_l = (unsigned char*) 0x10B; 
volatile unsigned char* ddr_l  = (unsigned char*) 0x10A; 
volatile unsigned char* pin_l  = (unsigned char*) 0x109; 

void setup() 
{
  //set PB7 to OUTPUT
  set_PL_as_output(0);
  set_PL_as_output(2);
  set_PL_as_output(4);
  set_PL_as_output(6);
}

void loop() 
{
//  // drive PB7 HIGH
//  write_pl(0, 1);
//  write_pl(2, 1);
//  write_pl(4, 1);
//  write_pl(6, 1);
//  // wait 500ms
//  delay(500);
//  // drive PB7 LOW
//  write_pl(0, 0);
//  write_pl(2, 0);
//  write_pl(4, 0);
//  write_pl(6, 0);
//  
//  // wait 500ms
//  delay(500);
}
void set_PL_as_output(unsigned char pin_num)
{
    *ddr_l |= 0x01 << pin_num;
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
