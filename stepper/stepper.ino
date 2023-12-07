#include <Stepper.h>

const int stepsPerRevolution = 1023;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


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

int pot_value=0;
void setup() 
{
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();
}
void loop() 
{
  int val = adc_read(0);
  Serial.println(val);
  if(val!=pot_value){
    Serial.println("Hello");
    if(val>pot_value){
      int temp = val -pot_value;
      Serial.println(temp);
      myStepper.step(temp);
      pot_value = val;
    }
    else{
      int temp = pot_value - val;
      Serial.println(temp);
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

void U0init(int U0baud){
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

unsigned char U0kbhit(){
  return *myUCSR0A & RDA;
}

unsigned char U0getchar(){
  return *myUDR0;
}

void U0putchar(unsigned char U0pdata){
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}