/* Name: Chris Howe and Neil Babu
*  Date: 10/23/23
*  CPE 301 Lab #6
*/

#include <LiquidCrystal.h>
#include <Keypad.h>


// LCD pins <--> Arduino pins
const int RS = 13, EN = 12, D4 = 7, D5 = 6, D6 = 5, D7 = 4;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); // Creates a new lcd object

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  lcd.begin(16,2);
    lcd.clear();
  lcd.setCursor(0,0);
  lcd.print('h');

}

void loop() {

}
