#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_Qwiic_OTOS_Arduino_Library.h"
#include "trajectory.hpp"

#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include "utils.hpp"
#include "secrets.hpp"
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "robot.pb.h"
#include <mutex>

#define MAIN_I2C_WIRE Wire
#define OTOS_I2C_WIRE Wire1

#define MAIN_I2C_SDA_PIN 0
#define MAIN_I2C_SCL_PIN 1
#define OTOS_I2C_SDA_PIN 2
#define OTOS_I2C_SCL_PIN 3

#define MOTOR1_IN1_PIN 12
#define MOTOR1_IN2_PIN 13
#define MOTOR2_IN1_PIN 10
#define MOTOR2_IN2_PIN 11

constexpr float R = 0.03f; // Wheels radius, in meter
constexpr float B = 0.255f; // Width, in meter
constexpr float halfB = B/2.f * 1.27f; // 1.27f is there as a correcting coefficient, I don't know why
constexpr float M = 2.8f; // In kg, Robot mass TODO: Ajust it: Ajust it, really not sure about the value
constexpr float J = 0.05f; //0.1f; // In kg.m^2, Robot moment of inertia TODO: Ajust it, really not sure about the value

constexpr float Umax = 12.0f; // In V
constexpr float Imax = 3.7f; // In A
constexpr float NMaxPhysically = 346.f; // In tr/min, mean of 2 motors
constexpr float wMaxPhysically = (NMaxPhysically * TWO_PI) / 60.f; // In rad/s
constexpr int minPWM = 17;
constexpr int maxPWM = 255;
constexpr int deltaPWM = maxPWM - minPWM;
constexpr float speedConst = Umax / wMaxPhysically; // Motor speed/torque constant (for a virtual motor composed of the real motor and the reductor)
constexpr float invEffTorque = 3.5f; //1.754f; // Inverse of torque efficiency (motoreductor) TODO: Ajust it, really not sure about the value
//constexpr float resTorque = 0.05f; // In N.m, Motor resistive torque TODO: Ajust it, really not sure about the value
constexpr float motR = 3.24f; // In Ohm, Motor internal resistor (varies with motor temperature) TODO: Ajust it, really not sure about the value

// TODO: Deprecate it
constexpr float k1 = wMaxPhysically;
constexpr float k2 = deltaPWM / wMaxPhysically;
constexpr float k3 = minPWM;

bool motorsEnabled = false;

int pwm1 = 0;
int pwm2 = 0;

Trajectory2D trajectory;
unsigned long startingAbsoluteTime;
float kX = 10.f;
float kY = 50.f;
float kTheta = 50.f;

bool trajectoryPreviouslyFinished = false;

QwiicOTOS myOtos;
sfe_otos_pose2d_t currentPosition, currentVelocity;

// [FIX] Added required mutexes for safe concurrent operations
mutex currentPosVelMtx;
mutex trajectoryMtx; 
mutex serialMtx; // Protects Serial from locking up if both cores print at the same time

bool wifiAndMqttReady = false; // TODO: used for test purpose, remove it because robot could be stuck in games or at least loose a lot of time
bool positionInitializedFromCamera = false; //TODO: Robot should also be able to initialize its position manually with the borders

// [FIX] Global variables to safely pass data from ISR to Core 1 loop
volatile bool newTargetReceived = false;
volatile float incomingTargetX = 0.0f;
volatile float incomingTargetY = 0.0f;

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
  return clamp((int)round(pwm), -255, 255);
}

inline float wAndTToPWM(float w, float T) {
  float pwm = deltaPWM * (motR*T/speedConst + speedConst*w)/Umax;
  if (pwm != 0) { pwm = (pwm > 0 ? pwm+minPWM : pwm-minPWM ); }
  return clamp((int)round(pwm), -255, 255);
}

void setup() {
  Serial.begin(115200);

  OTOS_I2C_WIRE.setSDA(OTOS_I2C_SDA_PIN);
  OTOS_I2C_WIRE.setSCL(OTOS_I2C_SCL_PIN);
  OTOS_I2C_WIRE.setClock(400000);
  OTOS_I2C_WIRE.begin();

  // [FIX] Initialize all mutexes
  mutex_init(&currentPosVelMtx);
  mutex_init(&trajectoryMtx);
  mutex_init(&serialMtx);

  while (myOtos.begin(OTOS_I2C_WIRE) == false) {
    mutex_enter_blocking(&serialMtx);
    Serial.println("OTOS not connected, check your wiring and I2C address!");
    mutex_exit(&serialMtx);
    delay(1000);
  }
  
  mutex_enter_blocking(&serialMtx);
  Serial.println("OTOS connected!");
  mutex_exit(&serialMtx);

  myOtos.calibrateImu();
  myOtos.setLinearUnit(kSfeOtosLinearUnitMeters);
  myOtos.setAngularUnit(kSfeOtosAngularUnitRadians);
  myOtos.setLinearScalar(1.0);
  myOtos.setAngularScalar(0.98743);
  myOtos.resetTracking();

  pinMode(MOTOR1_IN1_PIN, OUTPUT);
  pinMode(MOTOR1_IN2_PIN, OUTPUT);
  pinMode(MOTOR2_IN1_PIN, OUTPUT);
  pinMode(MOTOR2_IN2_PIN, OUTPUT);
  analogWrite(MOTOR1_IN1_PIN, 0);
  analogWrite(MOTOR1_IN2_PIN, 0);
  analogWrite(MOTOR2_IN1_PIN, 0);
  analogWrite(MOTOR2_IN2_PIN, 0);

  trajectory.m_linearLimit = Limit{0.2f, 0.2f, 0.2f}; //Limit{0.5f, 0.5f, 0.8f};
  trajectory.m_angularLimit = Limit{5.f, 5.f, 5.f}; //Limit{10.f, 10.f, 5.f};
  /*trajectory.m_steps.push_back(Trajectory2DStep{WAIT, 3.f, 0.f});//, -0.2f, 0.f});
  trajectory.m_steps.push_back(Trajectory2DStep{ROTATE_TO, HALF_PI, 0.f});*/

  //trajectory.m_steps.push_back(Trajectory2DStep{ROTATE_AND_GO_TO, -0.6f, 2.398f});//, -0.6f, 1.f});
  //trajectory.m_steps.push_back(Trajectory2DStep{WAIT, 0.2f, 0.f});
  //trajectory.m_steps.push_back(Trajectory2DStep{ROTATE_AND_GO_TO, -1.396f, 2.398f});//, -1.f, 1.f});
  //trajectory.m_steps.push_back(Trajectory2DStep{WAIT, 0.2f, 0.f});
  //trajectory.m_steps.push_back(Trajectory2DStep{ROTATE_AND_GO_TO, -1.396f, 0.6f});//, -1.f, 0.6f});
  //trajectory.m_steps.push_back(Trajectory2DStep{WAIT, 0.2f, 0.f});
  //trajectory.m_steps.push_back(Trajectory2DStep{ROTATE_AND_GO_TO, -0.6f, 0.6f});//, -0.6f, 0.6f});
  //trajectory.m_steps.push_back(Trajectory2DStep{WAIT, 0.2f, 0.f});
  //trajectory.m_steps.push_back(Trajectory2DStep{ROTATE_TO, 0.f, 0.f});
  //delay(5000);

  while (!(wifiAndMqttReady && positionInitializedFromCamera)) {
    delay(50);
  }

  //mutex_enter_blocking(&currentPosVelMtx);
  //myOtos.getPosition(currentPosition);
  //myOtos.getVelocity(currentVelocity);
  // TODO : Instead, use : myOtos.getPosVelAccAndStdDev(pos, vel, acc, posStdDev, velStdDev, accStdDev);
  //mutex_exit(&currentPosVelMtx);
  //Serial.printf(">pX:%f\n>pY:%f\n>pTheta:%f\n", currentPosition.x, currentPosition.y, currentPosition.h);
  //trajectory.computeSubTrajectories(currentPosition.x, currentPosition.y, currentPosition.h);
  //startingAbsoluteTime = micros();
}

void loop() {
  // [FIX] Local copy to prevent tearing if Core 1 updates the global position mid-calculation
  sfe_otos_pose2d_t localPosition;
  
  mutex_enter_blocking(&currentPosVelMtx);
  myOtos.getPosition(currentPosition);
  myOtos.getVelocity(currentVelocity);
  // TODO : Instead, use : myOtos.getPosVelAccAndStdDev(pos, vel, acc, posStdDev, velStdDev, accStdDev);
  localPosition = currentPosition; // Safe copy
  mutex_exit(&currentPosVelMtx);

  mutex_enter_blocking(&serialMtx);
  Serial.printf(">pX:%f\n>pY:%f\n>pTheta:%f\n", localPosition.x, localPosition.y, localPosition.h);
  Serial.printf(">p:%f:%f|xy\n", localPosition.x, localPosition.y);
  mutex_exit(&serialMtx);

  if (motorsEnabled) {
    float currentTime = (micros() - startingAbsoluteTime)/1000000.f;
    
    // [FIX] Protect trajectory reading
    mutex_enter_blocking(&trajectoryMtx);
    DesiredState2D desiredState = trajectory.getDesiredState2D(currentTime);
    mutex_exit(&trajectoryMtx);
    
    // THE FOLLOWING SECTION IS FOR A TEST PURPOSE ONLY
    //constexpr float freq = 1.0f;
    //constexpr float amplitude = 0.05f;
    //constexpr float pulsation = TWO_PI*freq;
    //DesiredState2D desiredState = {{-0.6f, 0.75f-amplitude*cosf(pulsation*currentTime), 0.f}, amplitude*pulsation*sinf(TWO_PI*freq*currentTime), 0.f, amplitude*pulsation*pulsation*cosf(pulsation*currentTime), 0.f, false};
    // END OF TEST SECTION

    mutex_enter_blocking(&serialMtx);
    Serial.printf(">pXD:%f\n>pYD:%f\n>pThetaD:%f\n>vXYD:%f\n>vThetaD:%f\n", desiredState.m_pos.x, desiredState.m_pos.y, desiredState.m_pos.h, desiredState.m_XYVel, desiredState.m_thetaVel);
    Serial.printf(">pD:%f:%f|xy\n", desiredState.m_pos.x, desiredState.m_pos.y);
    mutex_exit(&serialMtx);
    
    if (desiredState.m_trajectoryFinished) {
      if (!trajectoryPreviouslyFinished) {
        trajectoryPreviouslyFinished = true;
        // TODO : SEND TRAJ FINISHED SIGNAL TO RASPI 4
      }
    }

    // ============================= Motors speed computation =============================

    float xE = - (desiredState.m_pos.x - localPosition.x) * sinf(localPosition.h) + (desiredState.m_pos.y - localPosition.y) * cosf(localPosition.h);
    float yE = (desiredState.m_pos.x - localPosition.x) * cosf(localPosition.h) + (desiredState.m_pos.y - localPosition.y) * sinf(localPosition.h);
    float thetaE = desiredState.m_pos.h - localPosition.h;
    
    mutex_enter_blocking(&serialMtx);
    Serial.printf(">xE:%f\n>yE:%f\n>thetaE:%f\n", xE, yE, thetaE);
    mutex_exit(&serialMtx);

    float vX = desiredState.m_XYVel * cosf(thetaE) + kX * xE;
    float vTheta = desiredState.m_thetaVel + max(desiredState.m_XYVel, 0.01) * (kY * (-yE) + kTheta * sin(thetaE));
    //Serial.printf(">vX:%f\n>vTheta:%f\n", vX, vTheta);

    float w1 = (vX + vTheta*halfB) / R;
    float w2 = - (vX - vTheta*halfB) / R;
    //Serial.println("Before correction: w1: " + String(w1) + ", w2: " + String(w2));

    float wMax = percentageToW(100);
    //Serial.println("wMax: " + String(wMax));

    float w1OverWMax = abs(w1)/wMax;
    float w2OverWMax = abs(w2)/wMax;

    float maxPercentageOfWMax = max(w1OverWMax, w2OverWMax);
    //Serial.println("Max percentage of wMax: " + String(maxPercentageOfWMax));

    if (maxPercentageOfWMax >= 1.f) {
        w1 /= maxPercentageOfWMax;
        w2 /= maxPercentageOfWMax;
    }
    //Serial.println("After correction: w1: " + String(w1) + ", w2: " + String(w2));

    // Computation of motors torque linear & angular parts
    float T1 = - R * ((M/2)*desiredState.m_XYAccel - (J*desiredState.m_thetaAccel)/halfB);
    float T2 = R * ((M/2)*desiredState.m_XYAccel + (J*desiredState.m_thetaAccel)/halfB);
    // TODO: Determine if resTorque is usefull but there already is minPWM
    //if (T1 != 0) { T1 = (T1 > 0 ? T1+resTorque : T1-resTorque); }
    //if (T2 != 0) { T2 = (T2 > 0 ? T2+resTorque : T2-resTorque); }
    //T1 *= 5;
    //T2 *= 5;
    //Serial.printf(">T1:%f\n>T2:%f\n", T1, T2);
    T1 *= invEffTorque;
    T2 *= invEffTorque;
    // TODO: Ajust M, J, resTorque values

    pwm1 = wAndTToPWM(w1, T1);
    pwm2 = wAndTToPWM(w2, T2);
  } else {
    pwm1 = 0;
    pwm2 = 0;    
  }
  //Serial.println("Finnaly: pwm1: " + String(pwm1) + ", pwm2: " + String(pwm2));
  //Serial.printf(">pwm1:%d\n>pwm2:%d\n", pwm1, pwm2);
  

  // ============================= Motors speed update =============================

  if (pwm1 >= 0) {
    analogWrite(MOTOR1_IN1_PIN, 1023);
    analogWrite(MOTOR1_IN2_PIN, 255-abs(pwm1));
  } else {
    analogWrite(MOTOR1_IN1_PIN, 255-abs(pwm1));
    analogWrite(MOTOR1_IN2_PIN, 1023);
  }
  if (pwm2 >= 0) {
    analogWrite(MOTOR2_IN1_PIN, 1023);
    analogWrite(MOTOR2_IN2_PIN, 255-abs(pwm2));
  } else {
    analogWrite(MOTOR2_IN1_PIN, 255-abs(pwm2));
    analogWrite(MOTOR2_IN2_PIN, 1023);
  }
  //delay(100);
}



// =================================== OTHER CORE ===================================

char ssid[] = "Ironbot";        // your network SSID (name)
char pass[] = WIFI_PASSWORD;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress wifiAddress(192, 168, 8, 21);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);

const char mqttBroker[] = "192.168.8.10";
int mqttBrokerPort = 1883;
const char mqttPosTopic[] = "/static_camera/own_robot_position";

char streamChar[32];
int streamCharPos = 0;
int incomingByte = 0;

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

void mainI2cReceiverHandler(int someUnusedParam) {
  for (streamCharPos = 0; streamCharPos < 32; streamCharPos++){
    streamChar[streamCharPos] = 0;
  }

  streamCharPos = 0;
  while (MAIN_I2C_WIRE.available() > 0) {
    incomingByte = MAIN_I2C_WIRE.read();
    
    // [FIX] Buffer overflow protection
    if (streamCharPos < sizeof(streamChar) - 1) {
      streamChar[streamCharPos] = incomingByte;
      streamCharPos++;
    }

    if (incomingByte == '\n'){
      //Serial.print("Command received from main I2C bus : ");
      //Serial.println(streamChar);
      break;
    }
  }

  streamCharPos = 0;
  switch (streamChar[0]) {
    case 'E': {
      while (!isDigit(streamChar[streamCharPos]) && streamChar[streamCharPos]!='-') {
        streamCharPos++;
      }

      // [FIX] Store values in globals instead of computing trajectory in ISR
      incomingTargetX = getIntFromStream() / 1000.f;
      streamCharPos++;
      incomingTargetY = getIntFromStream() / 1000.f;
      
      newTargetReceived = true; // Flag for loop1 to process safely

      break;
    }
    case 'S': {
      motorsEnabled = false;
    }
  }

  while (MAIN_I2C_WIRE.available() > 0) {
    MAIN_I2C_WIRE.read();
  }
}

void setup1() {
  //Initialize serial and wait for port to open:
  /*Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/

  MAIN_I2C_WIRE.setSDA(MAIN_I2C_SDA_PIN);
  MAIN_I2C_WIRE.setSCL(MAIN_I2C_SCL_PIN);
  MAIN_I2C_WIRE.begin(0x10);
  MAIN_I2C_WIRE.onReceive(mainI2cReceiverHandler);

  while (!wifiAndMqttReady) {
    // attempt to connect to Wifi network:
    mutex_enter_blocking(&serialMtx);
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    mutex_exit(&serialMtx);
    
    WiFi.config(wifiAddress, primaryDNS, gateway, subnet);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      // failed, retry
      //Serial.print(".");
      delay(500);
    }
    
    mutex_enter_blocking(&serialMtx);
    Serial.println("You're connected to the network.");
    Serial.println();
    Serial.println("Attempting to ping the MQTT broker");
    mutex_exit(&serialMtx);
    
    if (WiFi.ping(mqttBroker) == -1) {
      mutex_enter_blocking(&serialMtx);
      Serial.println("Can't ping the MQTT broker");
      mutex_exit(&serialMtx);

      WiFi.end();
      continue;
    }

    mutex_enter_blocking(&serialMtx);
    Serial.print("Attempting to connect to the MQTT broker");
    mutex_exit(&serialMtx);
    
    if (!mqttClient.connect(mqttBroker, mqttBrokerPort)) {
      mutex_enter_blocking(&serialMtx);
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
      mutex_exit(&serialMtx);

      WiFi.end();
      continue;
    }

    mutex_enter_blocking(&serialMtx);
    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
    mutex_exit(&serialMtx);

    mqttClient.onMessage(onMqttMessage);

    mqttClient.subscribe(mqttPosTopic);

    mutex_enter_blocking(&serialMtx);
    Serial.print("Subscribed to topic: ");
    Serial.println(mqttPosTopic);
    Serial.println();
    mutex_exit(&serialMtx);

    wifiAndMqttReady = true;
  }
}

void loop1() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // [FIX] Process ISR data here, safely protected by mutexes
  if (newTargetReceived) {
    mutex_enter_blocking(&currentPosVelMtx);
    float currentX = currentPosition.x;
    float currentY = currentPosition.y;
    float currentH = currentPosition.h;
    mutex_exit(&currentPosVelMtx);

    mutex_enter_blocking(&trajectoryMtx);
    trajectory.m_steps.clear();
    trajectory.m_steps.push_back(Trajectory2DStep{ROTATE_AND_GO_TO, incomingTargetX, incomingTargetY});
    trajectory.computeSubTrajectories(currentX, currentY, currentH);
    startingAbsoluteTime = micros();
    trajectoryPreviouslyFinished = false; // Reset the finish flag
    mutex_exit(&trajectoryMtx);

    motorsEnabled = true;
    newTargetReceived = false;
  }
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  /*Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");*/

  uint8_t buf[1024];
  int len = mqttClient.readBytes(buf, sizeof(buf));
  //Serial.printf("%i bytes read\n", len);
  pb_istream_t istream = pb_istream_from_buffer(buf, len);

  // [FIX] Retrieve current velocity safely before checking it
  mutex_enter_blocking(&currentPosVelMtx);
  float cvX = currentVelocity.x;
  float cvY = currentVelocity.y;
  float cvH = currentVelocity.h;
  mutex_exit(&currentPosVelMtx);

  static bool wasRobotStoppedBefore = false;
  static unsigned long absoluteTimeSinceRobotIsStopped;
  if (abs(cvX)<0.05 && abs(cvY)<0.05 && abs(cvH)<0.314) {
    if (!wasRobotStoppedBefore) {
      absoluteTimeSinceRobotIsStopped = micros();
    }

    if ((micros() - absoluteTimeSinceRobotIsStopped)/1000000.f > 1.5f) { // We wait one second after the robot is stopped before using camera measurements to counter the camera delay
      robot_Position receivedPos = robot_Position_init_zero;
      if (!pb_decode(&istream, robot_Position_fields, &receivedPos)) {
        mutex_enter_blocking(&serialMtx);
        Serial.print("Decode failed: ");
        Serial.println(PB_GET_ERROR(&istream));
        Serial.println();
        mutex_exit(&serialMtx);
      } else if (isnanf(receivedPos.x) || isnanf(receivedPos.y) || isnanf(receivedPos.theta)) {
        //Serial.printf("Invalid position received: x=%f, y=%f, theta=%f\n", receivedPos.x, receivedPos.y, receivedPos.theta);
      } else {
        //Serial.print("x="); Serial.println(receivedPos.x);
        //Serial.print("y="); Serial.println(receivedPos.y);
        //Serial.print("theta="); Serial.println(receivedPos.theta);
        //Serial.print("ts_ms="); Serial.println(receivedPos.ts_ms);
        //Serial.printf(">pXC:%f\n>pYC:%f\n>pThetaD:%f\n", receivedPos.x, receivedPos.y, receivedPos.theta);
        Serial.printf(">pXC:%f\n>pYC:%f\n>pThetaD:%f\n", receivedPos.x*0.001f, receivedPos.y*0.001f, receivedPos.theta);
        
        sfe_otos_pose2d_t newCurrentPosition = {receivedPos.x*0.001f, receivedPos.y*0.001f, receivedPos.theta}; // TODO X and Y are inverted and need to be sent in meter from the camera
        mutex_enter_blocking(&currentPosVelMtx);
        myOtos.setPosition(newCurrentPosition);
        mutex_exit(&currentPosVelMtx);
        positionInitializedFromCamera |= 1;
      }
    }

    wasRobotStoppedBefore = true;
  } else {
    wasRobotStoppedBefore = false;
  }
}