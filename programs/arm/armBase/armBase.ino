#include <Servo.h>
#include <Wire.h>

#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define SERVO3_PIN 11

#define MAX_SEQUENCE_LENGTH 32
#define MAX_QUEUE_LENGTH 32

struct Command {
  int index;
  int argument;
};

struct Sequence {
  int length;
  Command steps [MAX_SEQUENCE_LENGTH];
};

int memorizedPositions[][3] = {{0, 0, 0}, {180, 180, 180}, {90, 90, 90}};

Sequence memorizedSequences[] = {Sequence{4, {Command{4, 0}, Command{0, 5}, Command{4, 1}, Command{0, 5}}},
                                 Sequence{4, {Command{5, 0}, Command{5, 0}, Command{5, 0}, Command{5, 0}}},};

int commandQueueLength = 0;
Command commandQueue[MAX_QUEUE_LENGTH]; 

bool stopped = false;

Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  Serial.begin(115200);
  Wire.begin(1); // join I2C bus with address 0x01
  Wire.onReceive(i2cReceiverHandler);
}

void loop() {
  if (commandQueueLength > 0) {
    executeCommand(deleteFirstCommandFromQueue());
  }
}

void i2cReceiverHandler(int howMany) {
  Serial.println("Receiving data:");
  while (Wire.available() > 0) {
    byte data = Wire.read();
    bool immediate = data>>7;
    int index = (data & 0b01110000) >> 4;
    int argument = data & 0b00001111;
         
    Serial.println("\t" +
                 byteToString(data) + 
                 ", imediate command: " + immediate +
                 ", index: " + index +
                 ", argument: " + argument);

    stopped = false;

    if (immediate) {
      executeCommand(Command{index, argument});
    } else {
      addCommandToQueue(Command{index, argument});
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

Command deleteFirstCommandFromQueue() {
  if (commandQueueLength == 0) {
    return Command{0, 0};
  }

  Command deletedCommand = commandQueue[0];
  for(int i = 0; i<commandQueueLength-1;i++){
    commandQueue[i] = commandQueue[i+1];
  }
  commandQueueLength=max(commandQueueLength-1, 0);
  return deletedCommand;
}

void addCommandToQueue(Command command) {
  if (commandQueueLength < MAX_QUEUE_LENGTH) {
    commandQueue[commandQueueLength] = command;
    commandQueueLength=min(commandQueueLength+1, MAX_QUEUE_LENGTH);
  }
}

void executeCommand(Command command) {
  if (!stopped) {
    int index = command.index;
    int argument = command.argument;

    Serial.println("Executing command: index: " + String(index) + ", argument: " + String(argument));

    switch (index) {
      case 0: //stop
        if (argument == 0) { //stop
          commandQueueLength = 0;
          stopped = true;
        } else { //wait (max = 3000ms [3s])
          delay(argument*200);
        }
        break;
      case 1:   //set mot 1
        servo1.write(argument*12);
        break;
      case 2:   //set mot 2
        servo2.write(argument*12);
        break;
      case 3:   //set mot 3
        servo3.write(argument*12);
        break;
      case 4:   //set memorized pos (every motors)
        servo1.write(memorizedPositions[argument][0]);
        servo2.write(memorizedPositions[argument][1]);
        servo3.write(memorizedPositions[argument][2]);
        break;
      case 5:
        Serial.println("Playing sequence " + String(argument));
        playSequence(memorizedSequences[argument]);
        break;
      case 6:
        break;
      case 7:
        break;
    }
  }
}

void playSequence(Sequence sequence){
  Serial.println("Sequence size: " + String(sequence.length));
  for (int step = 0; step<sequence.length; step++){
    executeCommand(sequence.steps[step]);
  }
}