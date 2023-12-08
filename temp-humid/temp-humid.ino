#include <dht.h> //install the DHTLib library
dht DHT;
#define DHTPIN 22
void setup(){
  Serial.begin(9600);
}
void loop(){
  int chk = DHT.read11(DHTPIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  delay(60000);
}
