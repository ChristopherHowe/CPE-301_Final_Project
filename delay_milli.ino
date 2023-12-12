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

#define INTERVAL 60000
unsigned long currentMillis = millis();

void delay_lcd(int interval, char state){
  if (state != errorState){
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      displayTempHumidLCD(DHT.temperature, DHT.humidity)
    }
  }
}
