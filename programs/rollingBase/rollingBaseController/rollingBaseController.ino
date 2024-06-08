#include <Wire.h>

#define MOTOR1_DIRECTION_PIN 6
#define MOTOR2_DIRECTION_PIN 7
#define MOTOR3_DIRECTION_PIN 8
#define MOTOR1_PWM_PIN 9
#define MOTOR2_PWM_PIN 10
#define MOTOR3_PWM_PIN 11

#define TEST_SPEED 50

char streamChar[32];
int streamCharPos = 0;
int incomingByte = 0;

int motor1Speed = 0;
int motor2Speed = 0;
int motor3Speed = 0;

int dX = 0;
int dY = 0;
int dTheta = 0;

float motor1TurningCoeff = 1.f;
float motor2TurningCoeff = 1.f;
float motor3TurningCoeff = 1.f;
float motor1CorrectionCoeff = 1.f;
float motor2CorrectionCoeff = 1.f;
float motor3CorrectionCoeff = 1.f;

unsigned char percentageToPwmValue(unsigned int percentage) {
  if (percentage == 0) {
    return 0;
  } else {
    return map(percentage, 1, 100, 15, 255); // so 1% => 15 because under 15, the motors doesn't turn
  }
}

void i2cReceiverHandler() {
  bool neg;
  motor1Speed = 0;
  motor2Speed = 0;
  motor3Speed = 0;
  dX = 0;
  dY = 0;
  dTheta = 0;

  for (streamCharPos = 0; streamCharPos < 32; streamCharPos++){
    streamChar[streamCharPos] = 0;
  }
  streamCharPos = 0;

  while (Wire.available() > 0) {
    incomingByte = Wire.read();
    streamChar[streamCharPos] = incomingByte;
    streamCharPos++;

    if (incomingByte == '\n'){
      Serial.print(streamChar);
      break;
    }
  }

  streamCharPos = 0;

  switch (streamChar[0]) {
    case 'A':
      while (!isDigit(streamChar[streamCharPos]) && streamChar[streamCharPos]!='-') {
        streamCharPos++;
      }

      // Set motor 1 speed
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        motor1Speed = motor1Speed * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        motor1Speed = -motor1Speed;
      }
      streamCharPos++;

      // Set motor 2 speed
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        motor2Speed = motor2Speed * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        motor2Speed = -motor2Speed;
      }
      streamCharPos++;

      // Set motor 3 speed
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        motor3Speed = motor3Speed * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        motor3Speed = -motor3Speed;
      }

      motor1Speed = min(max(motor1Speed , -100), 100);
      motor2Speed = min(max(motor2Speed , -100), 100);
      motor3Speed = min(max(motor3Speed , -100), 100);

      break;

    case 'B':
      while (!isDigit(streamChar[streamCharPos]) && streamChar[streamCharPos]!='-') {
        streamCharPos++;
      }

      // Set dX
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        dX = dX * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        dX = -dX;
      }
      streamCharPos++;

      // Set dY
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        dY = dY * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        dY = -dY;
      }
      streamCharPos++;

      // Set dTheta
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        dTheta = dTheta * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        dTheta = -dTheta;
      }

      motor1Speed = int((-dY - motor1TurningCoeff*dTheta) * motor1CorrectionCoeff);
      motor2Speed = int((0.866*dX + 0.5*dY - motor2TurningCoeff*dTheta) * motor2CorrectionCoeff); // 0.866 = sqrt(3.0)/2.0
      motor3Speed = int((-0.866*dX + 0.5*dY - motor3TurningCoeff*dTheta) * motor3CorrectionCoeff); // 0.866 = sqrt(3.0)/2.0

      break;

    case 'C':
      while (!isDigit(streamChar[streamCharPos]) && streamChar[streamCharPos]!='-') {
        streamCharPos++;
      }

      // Set dX
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        dX = dX * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        dX = -dX;
      }
      streamCharPos++;

      // Set dY
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        dY = dY * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        dY = -dY;
      }
      streamCharPos++;

      // Set dTheta
      if (streamChar[streamCharPos] == '-'){
        neg = true;
        streamCharPos++;
      } else {
        neg = false;
      }
      while (isDigit(streamChar[streamCharPos])) {
        dTheta = dTheta * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      if (neg) {
        dTheta = -dTheta;
      }
      streamCharPos++;

      // Set maxPercentage
      int maxPercentage = 0;
      while (isDigit(streamChar[streamCharPos])) {
        maxPercentage = maxPercentage * 10 + streamChar[streamCharPos] - '0';
        streamCharPos++;
      }
      int maxPwmValue = percentageToPwmValue(maxPercentage);
      Serial.println(maxPwmValue);

      motor1Speed = int((-dY - motor1TurningCoeff*dTheta) * motor1CorrectionCoeff);
      motor2Speed = int((0.866*dX + 0.5*dY - motor2TurningCoeff*dTheta) * motor2CorrectionCoeff); // 0.866 = sqrt(3.0)/2.0
      motor3Speed = int((-0.866*dX + 0.5*dY - motor3TurningCoeff*dTheta) * motor3CorrectionCoeff); // 0.866 = sqrt(3.0)/2.0

      float maxPercentageOfMaxPwmValue = max(max((abs(motor1Speed) / float(maxPwmValue)), (abs(motor2Speed) / float(maxPwmValue))), (abs(motor3Speed) / float(maxPwmValue)));

      motor1Speed = int(motor1Speed/maxPercentageOfMaxPwmValue);
      motor2Speed = int(motor2Speed/maxPercentageOfMaxPwmValue);
      motor3Speed = int(motor3Speed/maxPercentageOfMaxPwmValue);

      break;
  }
}

void setup() {
  Wire.begin(0x10);
  Serial.begin(115200);

  pinMode(MOTOR1_PWM_PIN, OUTPUT);
  pinMode(MOTOR2_PWM_PIN, OUTPUT);
  pinMode(MOTOR3_PWM_PIN, OUTPUT);
  analogWrite(MOTOR1_PWM_PIN, 0);
  analogWrite(MOTOR2_PWM_PIN, 0);
  analogWrite(MOTOR3_PWM_PIN, 0);

  pinMode(MOTOR1_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR2_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR3_DIRECTION_PIN, OUTPUT);
  digitalWrite(MOTOR1_DIRECTION_PIN, 0);
  digitalWrite(MOTOR2_DIRECTION_PIN, 0);
  digitalWrite(MOTOR3_DIRECTION_PIN, 0);

  Wire.onReceive(i2cReceiverHandler);
}

void loop() {
  Serial.println("Motor 1: " + String(motor1Speed) + ", motor 2: " + String(motor2Speed) + ", motor 3: " + String(motor3Speed));
  digitalWrite(MOTOR1_DIRECTION_PIN, (motor1Speed < 0));
  digitalWrite(MOTOR2_DIRECTION_PIN, (motor2Speed < 0));
  digitalWrite(MOTOR3_DIRECTION_PIN, (motor3Speed < 0));
  analogWrite(MOTOR1_PWM_PIN, percentageToPwmValue(abs(motor1Speed)));
  analogWrite(MOTOR2_PWM_PIN, percentageToPwmValue(abs(motor2Speed)));
  analogWrite(MOTOR3_PWM_PIN, percentageToPwmValue(abs(motor3Speed)));
  delay(100);
}
