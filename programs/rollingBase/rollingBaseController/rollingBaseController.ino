#include <Wire.h>

#define MOTOR1_DIRECTION_PIN 6
#define MOTOR2_DIRECTION_PIN 7
#define MOTOR3_DIRECTION_PIN 8
#define MOTOR1_PWM_PIN 9
#define MOTOR2_PWM_PIN 10
#define MOTOR3_PWM_PIN 11

constexpr float SQRT3OVER2 = 0.8660254038f;

constexpr float R = 0.03f;
constexpr float L = 0.122f;

constexpr float NMaxPhysically = 346.f; // In tr/min, mean of 3 motors
constexpr float wMaxPhysically = (NMaxPhysically * 2.f * PI) / 60.f; // In rad/s
constexpr int minPWM = 17;
constexpr int maxPWM = 255;
constexpr float k1 = wMaxPhysically;
constexpr float k2 = (maxPWM - minPWM) / wMaxPhysically;
constexpr float k3 = minPWM;

char streamChar[32];
int streamCharPos = 0;
int incomingByte = 0;
unsigned long previousReceivedTime = 0;

int pwm1 = 0;
int pwm2 = 0;
int pwm3 = 0;

float motor1TurningCoeff = 1.f;
float motor2TurningCoeff = 1.f;
float motor3TurningCoeff = 1.f;
float motor1CorrectionCoeff = 1.f;
float motor2CorrectionCoeff = 1.f;
float motor3CorrectionCoeff = 1.f;

inline int clamp(int value, int minValue, int maxValue) { return min(max(value, minValue), maxValue); }
inline float clamp(float value, float minValue, float maxValue) { return min(max(value, minValue), maxValue); }

int percentageToPwmValue(int percentage) { // Deprecated
  if (percentage == 0) {
    return 0;
  } else {
    int pwm = map(abs(percentage), 1, 100, minPWM, maxPWM); // so 1% => 15 because under 15, the motors doesn't turn
    if (percentage < 0) {
      pwm = -pwm;
    }
    return clamp(pwm, -255, 255); 
  }
}

inline float percentageToW(float percentage) {
  return k1*percentage; // w = k1*%
}

inline float wToPWM(float w) {
  float pwm = 0;
  if (w != 0) {
    pwm = k2*abs(w) + k3;
    if (w < 0)
      pwm = -pwm;
  }
  return clamp(round(pwm), -255, 255);
}


int getIntFromStream() {
  bool neg;
  int value = 0;
  if (streamChar[streamCharPos] == '-'){
    neg = true;
    streamCharPos++;
  } else {
    neg = false;
  }
  while (isDigit(streamChar[streamCharPos])) {
    value = value * 10 + streamChar[streamCharPos] - '0';
    streamCharPos++;
  }
  if (neg) {
    value = -value;
  }
  return value;
}

void i2cReceiverHandler() {
  pwm1 = 0;
  pwm2 = 0;
  pwm3 = 0;

  for (streamCharPos = 0; streamCharPos < 32; streamCharPos++){
    streamChar[streamCharPos] = 0;
  }
  streamCharPos = 0;

  while (Wire.available() > 0) {
    incomingByte = Wire.read();
    streamChar[streamCharPos] = incomingByte;
    streamCharPos++;

    if (incomingByte == '\n'){
      //Serial.print(streamChar);
      break;
    }
  }

  streamCharPos = 0;

  switch (streamChar[0]) {
    case 'A': {
      while (!isDigit(streamChar[streamCharPos]) && streamChar[streamCharPos]!='-') {
        streamCharPos++;
      }

      int motor1Percentage = getIntFromStream();
      streamCharPos++;
      int motor2Percentage = getIntFromStream();
      streamCharPos++;
      int motor3Percentage = getIntFromStream();

      motor1Percentage = clamp(motor1Percentage, -100, 100);
      motor2Percentage = clamp(motor2Percentage, -100, 100);
      motor3Percentage = clamp(motor3Percentage, -100, 100);

      pwm1 = percentageToPwmValue(motor1Percentage);
      pwm2 = percentageToPwmValue(motor2Percentage);
      pwm3 = percentageToPwmValue(motor3Percentage);

      break;
    }
    case 'D': {
      while (!isDigit(streamChar[streamCharPos]) && streamChar[streamCharPos]!='-') {
        streamCharPos++;
      }

      while (!isDigit(streamChar[streamCharPos]) && streamChar[streamCharPos]!='-') {
        streamCharPos++;
      }

      float vX = getIntFromStream() / 1000.f;
      streamCharPos++;
      float vY = getIntFromStream() / 1000.f;
      streamCharPos++;
      float vTheta = getIntFromStream() / 1000.f;
      streamCharPos++;
      float percentageMax = getIntFromStream() / 100.f;
      //Serial.println("vX: " + String(vX) + ", vY: " + String(vY) + ", vTheta: " + String(vTheta) + ", %Max: " + String(percentageMax));

      float w1 = (-vY - L*vTheta) / R;
      float w2 = (SQRT3OVER2*vX + 0.5f*vY - L*vTheta) / R;
      float w3 = (-SQRT3OVER2*vX + 0.5f*vY - L*vTheta) / R;
      //Serial.println("Before correction: w1: " + String(w1) + ", w2: " + String(w2) + ", w3: " + String(w3));

      float wMax = percentageToW(percentageMax);
      //Serial.println("wMax: " + String(wMax));

      float w1OverWMax = abs(w1)/wMax;
      float w2OverWMax = abs(w2)/wMax;
      float w3OverWMax = abs(w3)/wMax;

      float maxPercentageOfWMax = max(max(w1OverWMax, w2OverWMax), w3OverWMax);
      //Serial.println("Max percentage of wMax: " + String(maxPercentageOfWMax));

      if (maxPercentageOfWMax >= 1.f) {
         w1 /= maxPercentageOfWMax;
         w2 /= maxPercentageOfWMax;
         w3 /= maxPercentageOfWMax;
      }
      //Serial.println("After correction: w1: " + String(w1) + ", w2: " + String(w2) + ", w3: " + String(w3));

      pwm1 = wToPWM(w1);
      pwm2 = wToPWM(w2);
      pwm3 = wToPWM(w3);
      //Serial.println("Finnaly: pwm1: " + String(pwm1) + ", pwm2: " + String(pwm2) + ", pwm3: " + String(pwm3));

      break;
    }
  }

  while (Wire.available() > 0) {
    Wire.read();
  }

  previousReceivedTime = millis();
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
  if (millis() > previousReceivedTime + 1000) {
    pwm1 = 0;
    pwm2 = 0;
    pwm3 = 0;
  }
  
  //Serial.println("Motor 1: " + String(pwm1) + ", motor 2: " + String(pwm2) + ", motor 3: " + String(pwm3));
  digitalWrite(MOTOR1_DIRECTION_PIN, (pwm1 < 0)); //pwm1
  digitalWrite(MOTOR2_DIRECTION_PIN, (pwm2 < 0));
  digitalWrite(MOTOR3_DIRECTION_PIN, (pwm3 < 0));
  analogWrite(MOTOR1_PWM_PIN, abs(pwm1));
  analogWrite(MOTOR2_PWM_PIN, abs(pwm2));
  analogWrite(MOTOR3_PWM_PIN, abs(pwm3));
  //delay(100);
}
