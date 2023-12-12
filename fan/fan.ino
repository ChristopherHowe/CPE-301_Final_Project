/****
 * Chris Howe and Neil Babu
 */


volatile unsigned char* port_c = (unsigned char*) 0x28; 
volatile unsigned char* ddr_c  = (unsigned char*) 0x27; 
volatile unsigned char* pin_c  = (unsigned char*) 0x26; 

// Pin numbers for reference
 int speedPin=32;
 int dir1Pin=31;
 int dir2Pin=30;
 int mSpeed=90;

// Pin numbers for port c
int speedC = 5;
int dir1C = 6;
int dir2C = 7;

 void setup() {
  // put your setup code here, to run once:
  set_PC_as_output(5);
  set_PC_as_output(6);
  set_PC_as_output(7);
  Serial.begin(9600);
 }

 void loop() {
  //put your main code here, to run repeatedly:
  write_pc(5,1);
  write_pc(6,1);
  write_pc(7,0);
  delay(1000);
  write_pc(5,0);
  delay(1000);
 }

void set_PC_as_output(unsigned char pin_num)
{
    *ddr_c |= 0x01 << pin_num;
}

void write_pc(unsigned char pin_num, unsigned char state)
{
  if(state == 0)
  {
    *port_c &= ~(0x01 << pin_num);
  }
  else
  {
    *port_c |= 0x01 << pin_num;
  }
}
