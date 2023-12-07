/* Name: Chris Howe and Neil Babu
*  Date: 10/9/23
*  CPE 301 Lab #4
*/

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
void setup()
{
 // initialize the serial port on USART0:
 U0init(9600);
}
void loop()
{
  unsigned char cs1;
  while (U0kbhit()==0){}; // wait for RDA = true
  cs1 = U0getchar();    // read character
  int first = cs1/16;
  int second = cs1%16;
  unsigned char char1 = intToHex(first);
  unsigned char char2 = intToHex(second);
  U0putchar('0');
  U0putchar('x');
  U0putchar(char1);
  U0putchar(char2);
  U0putchar('\n');

}
char intToHex(int input) {
    switch(input){
      case 1:
        return '1';
        break;
      case 2:
        return '2';
        break;
      case 3:
        return '3';
        break;
      case 4:
        return '4';
        break;
      case 5:
        return '5';
        break;
      case 6:
        return '6';
        break;                
      case 7:
        return '7';
        break;
      case 8:
        return '8';
        break;
      case 9:
        return '9';
        break;
      case 10:
        return 'A';
        break;
      case 11:
        return 'B';
        break;
      case 12:
        return 'C';
        break;
      case 13:
        return 'D';
        break;
      case 14:
        return 'E';
        break;
      case 15:
        return 'F';
        break;
      }
      return '0';
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
