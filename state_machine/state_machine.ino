#include <TimeLib.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <dht.h>


#define DHT1_PIN 22


#define RDA 0x80
#define TBE 0x20

#define UDRE0 5
#define RXC0 7

#define WATER_THRESHOLD 100
#define TEMP_THRESHOLD 24
#define LCD_INTERVAL 60000


// Define UART registers - serial monitor communication
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

// ADC registers - used to convert water level sensor data, potentiometer for angle.
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

// Define Port L Register Pointers - used to control the LEDs
volatile unsigned char* port_l = (unsigned char*) 0x10B; 
volatile unsigned char* ddr_l  = (unsigned char*) 0x10A; 
volatile unsigned char* pin_l  = (unsigned char*) 0x109; 

// Define Port E Register Pointers - used to control the interrupt buttons
volatile unsigned char* port_e = (unsigned char*) 0x2E; 
volatile unsigned char* ddr_e  = (unsigned char*) 0x2D; 
volatile unsigned char* pin_e  = (unsigned char*) 0x2C; 

// Define Port B Register Pointers - used to control water level.
volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; 
volatile unsigned char* pin_b  = (unsigned char*) 0x23; 

//Define Port C Register Pointers - used to control fan
volatile unsigned char* port_c = (unsigned char*) 0x28; 
volatile unsigned char* ddr_c  = (unsigned char*) 0x27; 
volatile unsigned char* pin_c  = (unsigned char*) 0x26; 

// Pin numbers for reference for fa
int speedPin=32;
int dir1Pin=31;
int dir2Pin=30;
int mSpeed=90;

// Pin numbers for port c for fan
int speedC = 5;
int dir1C = 6;
int dir2C = 7;

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



///////////////////
/*****************/
// State storage //
enum state {
  idleState,
  runningState,
  errorState,
  disabledState,
};

enum state currentState = disabledState;
/*****************/
///////////////////


//////////////////////
// Global Variables //
//////////////////////


// Stepper motor globs
const int stepsPerRevolution = 2046;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
int pot_value=0;

// temp globs
dht DHT;

// clock globs
unsigned int numbers[10]= {0,1,2,3,4,5,6,7,8,9};
unsigned char characters[10]= {'0','1','2','3','4','5','6','7','8','9'};

// lcd globs
unsigned long previousMillis = millis();
const int RS = 13, EN = 12, D4 = 7, D5 = 6, D6 = 5, D7 = 4;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); // Creates a new lcd object


/////////////////////////////////
// ISRs for control interrupts //
/////////////////////////////////


// defines functionality for when the power button is pressed
void handlePower(){
  if (read_pe(power)){
    if(!disabledState) {
      setState(disabledState);
    }
    else {
      setState(idleState);
    }
  }
}

// defines functionality for when the reset button is pressed.
void handleReset(){
  if (read_pe(reset)){
    if (currentState == errorState) {
      setState(idleState);
    }
  }
}

////////////////////
// Main functions //
////////////////////


void setup() {
  //set PB7 to OUTPUT
  // Setup for controls, Port L = LEDs Port E = buttons
  set_PL_as_output(disabledLED);
  set_PL_as_output(errorLED);
  set_PL_as_output(idleLED);
  set_PL_as_output(runningLED);
  set_PE_as_pullup_input(reset);
  set_PE_as_pullup_input(power);
  attachInterrupt(digitalPinToInterrupt(resetPin), handleReset, CHANGE);
  attachInterrupt(digitalPinToInterrupt(powerPin), handlePower, CHANGE);
  
  // Motor Setup
  set_PC_as_output(speedC);
  set_PC_as_output(dir1C);
  set_PC_as_output(dir2C);

  // Serial Monitor Setup
  U0init(9600);

  // Real time clock setup
  setTime(16, 58, 20, 11, 12, 2023);
  
  // ADC setup
  adc_init();

  // Water Sensor Setup
  set_PB_as_output(1);
}

// MAIN LOOP
void loop() {
  print_time();
  if (currentState == disabledState){
    d_state();
  }
  else if (currentState == idleState){
    i_state();
  }
  else if (currentState == errorState){
    e_state();
  }
  else if (currentState == runningState){
    r_state();
  }

  delay_lcd(LCD_INTERVAL, currentState);
}

///////////////////////////////////////////////////////////////////////////////////////
// State functions, include everything that should be executed for a particular state//
///////////////////////////////////////////////////////////////////////////////////////

void setState(enum state newState){
  switch (currentState) {
    case disabledState:
      if (newState == idleState) {
        currentState = newState;
      }
      break;
    case errorState:
      if (newState == idleState || newState == disabledState){
        currentState = newState;
      }
      break;
    case runningState:
      currentState = newState;
      break;
    case idleState:
      currentState = newState;
      break;
  }
}

/******************/
// disabled state //
/******************/

void d_state(){
  // the fan should be off in the disabled state
  turn_off_fan();
  // set the leds to indicate disabled
  setLEDs(1,0,0,0);
  // Adjust vent
  stepper_motor();

  // yes should be able to adjust vent
}

/***************/
// error state //
/***************/

void e_state(){
  setLEDs(0,1,0,0);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Error, the water level is too low");
  // No stepper allowed
  // can only change state when reset is pressed (handled by interrupt)
}

/*****************/
// running state //
/*****************/

void r_state(){
  turn_on_fan();
  // set the LEDs to show running
  setLEDs(0,0,0,1);
  // test if the water is too low
  if (waterTooLow()){
    setState(errorState);
  }
  // make sure the temp is right
  if (DHT.temperature < TEMP_THRESHOLD){
    setState(idleState);
  }
  // run the angled stepper
  stepper_motor();
}

/**************/
// idle state //
/**************/

void i_state(){
  // Turning on the fan
  turn_off_fan();
  // Check for water level
  if (waterTooLow()){
    setState(errorState);
  }
  // make sure the temp is right
  if (DHT.temperature > TEMP_THRESHOLD){
    setState(runningState);
  }
  // run the angled stepper
  stepper_motor();
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


///////////////////////////////////////////////////////////////////
// Helper funcs for ADC - used for potentiometer and water level //
///////////////////////////////////////////////////////////////////

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

///////////////////////////
// Time Helper Functions //
///////////////////////////

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

/////////////////////////////////////////
// Helper funcs for UART Communication //
/////////////////////////////////////////

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

///////////////////////////////////////
// Helper functions for control bits //
///////////////////////////////////////
void setLEDs(int disabled, int error, int idle, int running){
  write_pl(disabledLED,disabled);
  write_pl(errorLEDPin,error);
  write_pl(idleLEDPin,idle);
  write_pl(runningLEDPin,running);
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

void write_pl(unsigned char pin_num, unsigned char state1)
{
  if(state1 == 0)
  {
    *port_l &= ~(0x01 << pin_num);
  }
  else
  {
    *port_l |= 0x01 << pin_num;
  }
}

////////////////////////////////////////////////////
// Helper functions for water level sensor //
////////////////////////////////////////////////////

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

bool waterTooLow(){
  if (adc_read(1) < WATER_THRESHOLD) {
    return true;
  }
  return false;
}

//////////////////////////
// Helper function for fan
//////////////////////////

void turn_on_fan(){
  write_pc(5,1);
  write_pc(6,1);
  write_pc(7,0);
}
void turn_off_fan(){
  write_pc(5,1);
  write_pc(6,1);
  write_pc(7,0);
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

//////////////////////////////////
// Helper funcs for tempurature //
//////////////////////////////////


void displayTempHumidLCD(float temp, float humidity){
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
}



void delay_lcd(int interval, char state){
  unsigned long currentMillis = millis();
  if (state != errorState){
    if (currentMillis - previousMillis >= interval) {
      int chk = DHT.read11(DHT1_PIN);
      previousMillis = currentMillis; // previousMillis = last time LCD was updated.
      displayTempHumidLCD(DHT.temperature, DHT.humidity);
    }
  }
}


