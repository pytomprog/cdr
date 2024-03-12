#include <Wire.h>

#define ROLLING_BASE_ADDR 1

void setup() {
  Serial.begin(115200);
  Wire.begin(); // join I2C bus (address optional for master)
  delay(1000); //Wait for slave to power up
  Wire.beginTransmission(ROLLING_BASE_ADDR); // transmit to device
  Wire.write(0b10000000);
  Wire.endTransmission();
}

void loop() {
  if (Serial.available() > 0) {
  	Wire.beginTransmission(ROLLING_BASE_ADDR); // transmit to device
    while (Serial.available() > 0) {
      bool immediate = Serial.parseInt();
      byte command = Serial.parseInt();
      byte argument = Serial.parseInt();
      byte data = immediate << 7 | command << 4 | argument;
  		Wire.write(data);
      Serial.println(data, BIN);
    }
  	Wire.endTransmission();
  }
}
