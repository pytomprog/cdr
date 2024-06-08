#include <Wire.h>

#define ROLLING_BASE_ADDR 0x10

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Wire.begin(); // join I2C bus (address optional for master)
  delay(1000); //Wait for slave to power up
  
  Wire.beginTransmission(ROLLING_BASE_ADDR); // transmit to device
  digitalWrite(LED_BUILTIN, HIGH);
  Wire.write(0b10000000);
  Wire.write(0b11010001);
  digitalWrite(LED_BUILTIN, LOW);
  Wire.endTransmission();
}

void loop() {
  if (Serial.available() > 0) {
  	Wire.beginTransmission(ROLLING_BASE_ADDR); // transmit to device
    digitalWrite(LED_BUILTIN, HIGH);
    while (Serial.available() > 0) {
      bool immediate = Serial.parseInt();
      byte command = Serial.parseInt();
      byte argument = Serial.parseInt();
      byte data = immediate << 7 | command << 4 | argument;
  		Wire.write(data);
      Serial.println(data, BIN);
    }
    digitalWrite(LED_BUILTIN, LOW);
  	Wire.endTransmission();
  }
}
