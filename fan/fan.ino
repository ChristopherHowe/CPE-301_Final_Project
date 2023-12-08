/****
 * Chris Howe and Neil Babu
 */


// All three pins should be digital, speed pin outputs a square wave using PWM
 int speedPin=32;
 int dir1=30;
 int dir2=31;
 int mSpeed=90;

 void setup() {
  // put your setup code here, to run once:
  pinMode(speedPin, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  Serial.begin(9600);
 }

 void loop() {
  //put your main code here, to run repeatedly:
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, HIGH);
  analogWrite(speedPin,255); // Should be using a digital pin, analog write is used to implment PWM.
  delay(1000);
  analogWrite(speedPin,mSpeed);
  delay(1000);
 }
