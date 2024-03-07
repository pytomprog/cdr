#include <Servo.h>
#include <Wire.h>

#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define SERVO3_PIN 11

int memorizedPositions[][3] = {{0, 0, 0}, {180, 180, 180}};

Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  Wire.begin(1); // join I2C bus with address 0x01
  Wire.onReceive(i2cReceiverHandler);
  Serial.begin(9600);
}

void loop() {

}

void i2cReceiverHandler(int howMany) {
  while (Wire.available() > 0) {
    byte data = Wire.read();
    bool immediate = data>>7;
    int command = (data & 0b01110000) >> 4;
    int argument = data & 0b00001111;
         
    Serial.print(byteToString(data) + 
                 ", imediate command: " + immediate +
                 ", command: " + command +
                 ", argument: " + argument);
    
    switch (command) {
      case 0:
      	break;
      case 1:
      	if (immediate) {
          	servo1.write(memorizedPositions[argument][0]);
          	servo2.write(memorizedPositions[argument][1]);
          	servo3.write(memorizedPositions[argument][2]);
      	}
      	break;
      case 2:
      	if (immediate) {
        	servo1.write(argument*12);
      	}
      	break;
      case 3:
      	if (immediate) {
        	servo2.write(argument*12);
      	}
      	break;
      case 4:
      	if (immediate) {
        	servo3.write(argument*12);
      	}
      	break;
      case 5:
      	break;
      case 6:
      	break;
      case 7:
      	break;
    }
    
    Serial.println();
  }
  Serial.println();
}

String byteToString(byte b) {
  String result = "";
  for(int i = 7; i >= 0; i--)
    result += bitRead(b,i);
  return result;
}
