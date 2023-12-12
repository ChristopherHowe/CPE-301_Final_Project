void setup() {
  // set the digital pin as output:
  Serial.begin(9600);
}

void loop() {
  Serial.println("hello");
  delay_using_milli(1000);
}

void delay_using_milli(int interval){
  unsigned long time_now = millis();
  while(millis() < time_now + interval){}
}