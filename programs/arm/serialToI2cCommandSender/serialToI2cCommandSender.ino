#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(); // join I2C bus (address optional for master)
  delay(1000); //Wait for slave to power up
  Wire.beginTransmission(1); // transmit to device 0x01
  Wire.write(0b10000000);
  Wire.endTransmission();
}

void loop() {
  if (Serial.available() > 0) {
  	Wire.beginTransmission(1); // transmit to device 0x01
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
