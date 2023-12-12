#include <TimeLib.h>

#define RDA 0x80
#define TBE 0x20  
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 
#define UDRE0 5
#define RXC0 7
 
//
// main()
//

unsigned int numbers[10]= {0,1,2,3,4,5,6,7,8,9};

unsigned char characters[10]= {'0','1','2','3','4','5','6','7','8','9'};

void setup()
{
 // initialize the serial port on USART0:
  U0init(9600);
  setTime(16, 58, 20, 11, 12, 2023);
}

void loop()
{    
  print_time(hour());
  U0putchar(':');
  print_time(minute());
  U0putchar(':');
  print_time(second());
  U0putchar('\n');

  delay(10000);
}

void print_time(int time){
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

