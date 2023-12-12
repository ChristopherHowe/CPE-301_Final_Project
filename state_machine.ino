#include <TimeLib.h>
#include <Stepper.h>



#define RDA 0x80
#define TBE 0x20  

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;


 
#define UDRE0 5
#define RXC0 7
 
char state = 'D';

int pot_value=0;

const int stepsPerRevolution = 2046;

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


unsigned int numbers[10]= {0,1,2,3,4,5,6,7,8,9};

unsigned char characters[10]= {'0','1','2','3','4','5','6','7','8','9'};




void setup() {

  U0init(9600);
  setTime(16, 58, 20, 11, 12, 2023);
  adc_init();
}

void loop() {

  if (state == 'D'){
    print_time();
    d_state();
  }
  else if (state == 'I'){
    print_time();
    i_state();
  }
  else if (state == 'E'){
    print_time();
    e_state();
  }
  else if (state == 'R'){
    print_time();
    r_state();
  }
}

void d_state(){
  // Fan off yellow light on
  // should be able to adjust vent
}
void i_state(){
  // Fan off green led on
  // if temp greater than threshold go to running
  // display temp and humidity on LCD
  // polling temp
  // should be able to adjust vent
}
void e_state(){
  // LCD display message red light on
  // only this state can the reset button be pushed
}
void r_state(){
  //blue light on, fan on
  // display temp and humidity on LCD
  // if temp less than threshold go to idle
  // polling temp
  // should be able to adjust vent
}


void stepper_motor(){
  int val = adc_read(0);
  if(val!=pot_value){
    if(val>pot_value){
      int temp = val -pot_value;
      Serial.println(temp);
      myStepper.setSpeed(10);
      myStepper.step(temp);
      pot_value = val;
    }
    else{
      int temp = pot_value - val;
      Serial.println(temp);
      myStepper.setSpeed(10);
      myStepper.step(-temp);
      pot_value = val;
    }
  }
}
void adc_init(){
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 6 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}

unsigned int adc_read(unsigned char adc_channel_num){
  *my_ADMUX  &= 0b11100000; // clear the channel selection bits (MUX 4:0)
  *my_ADCSRB &= 0b11110111;   // clear the channel selection bits (MUX 5)
  
  // set the channel number
  if(adc_channel_num > 7){
    adc_channel_num -= 8;     // set the channel selection bits, but remove the most significant bit (bit 3)
    *my_ADCSRB |= 0b00001000;     // set MUX bit 5

  }
  *my_ADMUX  += adc_channel_num;   // set the channel selection bits
  *my_ADCSRA |= 0x40;   // set bit 6 of ADCSRA to 1 to start a conversion
  while((*my_ADCSRA & 0x40) != 0);   // wait for the conversion to complete
  return *my_ADC_DATA;   // return the result in the ADC data register
}

void print_time(){
  print_to_monitor(hour());
  U0putchar(':');
  print_to_monitor(minute());
  U0putchar(':');
  print_to_monitor(second());
  U0putchar('\n');
}
void print_to_monitor(int time){
  int first = time/10;
  int second = time%10;
  unsigned char char1 = intToChar(first);
  unsigned char char2 = intToChar(second);
  U0putchar(char1);
  U0putchar(char2);
}
char intToChar(int num){
  for(int i=0; i < 10; i++)
      {
        // if it's the num we received...
        if(num == numbers[i]){
          return characters[i];
        }
      }
}
void U0init(unsigned long U0baud){ // Serial.Begin
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

unsigned char U0kbhit() { // Serial.Available
  return *myUCSR0A & RDA;
}
unsigned char U0getchar(){ // Serial.Read
  return *myUDR0;
}
void U0putchar(unsigned char U0pdata) { // Serial.Write
  while (!(*myUCSR0A & TBE));
  *myUDR0=U0pdata;
}





