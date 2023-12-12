/* Name: Chris Howe and Neil Babu
*  Date: 10/30/23
*  CPE 301 Lab #7
*/
#define RDA 0x80
#define TBE 0x20  
#define SENSOR_POWER_PIN 52

volatile unsigned char* my_ADMUX    = (unsigned char*) 0x7C; // Responsible for selecting the channel
volatile unsigned char* my_ADCSRB   = (unsigned char*) 0x7B; // stores configuration of the ADC 
volatile unsigned char* my_ADCSRA   = (unsigned char*) 0x7A; // stores configuration of the ADC 
volatile unsigned int*  my_ADC_DATA = (unsigned int*) 0x78;

// Need to do setup for pin 52 (Power Pin) equal to PB1 
// Note that port b includes 4 pins from the double row and 4 pins from the main row.
// PB ranges from 0-7 so PB1 is second bit.
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; 
volatile unsigned char* pin_b  = (unsigned char*) 0x23; 

void setup() 
{
  // setup the ADC
  adc_init();
  Serial.begin(9600);
  set_PB_as_output(1);
}

void loop() 
{
  write_pb(1,1);
  int val = adc_read(1);
  Serial.print("water level val: ");
  Serial.print(val);
  Serial.print("\n");
  delay(200);
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

void set_PB_as_output(unsigned char pin_num)
{
    *ddr_b |= 0x01 << pin_num;
}

void write_pb(unsigned char pin_num, unsigned char state)
{
  if(state == 0)
  {
    *port_b &= ~(0x01 << pin_num);
  }
  else
  {
    *port_b |= 0x01 << pin_num;
  }
}
