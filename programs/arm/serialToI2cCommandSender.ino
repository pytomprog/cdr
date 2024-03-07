#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join I2C bus (address optional for master)
  delay(1000); //Wait for slave to power up
}

byte x = 0;

void loop() {
  if (Serial.available() > 0) {
  	Wire.beginTransmission(1); // transmit to device 0x01
    while (Serial.available() > 0) {
  		Wire.write(Serial.parseInt());
    }
  	Wire.endTransmission();
  }
    
  x++;
  delay(500);
}
