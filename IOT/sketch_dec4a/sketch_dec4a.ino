#include <Servo.h>
#include <SoftwareSerial.h>

Servo servoLeft;
Servo servoRight;

const int setpoint = 2;
const int kpl = -50;
const int kpr = -50;
int speed = 0;

const int threshold = 1; // 장애물이 특정한 행동을 보증할 수 있을 정도로 가까운지 여부를 결정한다.

SoftwareSerial BT(5, 6);  // RX, TX

void setup() {
  pinMode(10, INPUT);
  pinMode(9, OUTPUT);
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(setpoint, OUTPUT);

  servoLeft.attach(13);
  servoRight.attach(12);

  tone(4, 3000, 1000);
  delay(1000);

  // Bluetooth setup
  BT.begin(9600);
  Serial.begin(9600);
}

bool shouldMoveForward = false;
bool shouldResumeCommands = true;

void loop() {
  // Bluetooth control
  if (BT.available()) {
    char command = BT.read();
    handleBluetoothCommand(command);
  }

  // Obstacle avoidance logic
  int irLeft = irDistance(9, 10);
  int irRight = irDistance(2, 3);

  digitalWrite(8, !irLeft);
  digitalWrite(7, !irRight);

    // Light intensity measurement
  float tLight = float(rcTime(11));  // Get right light & make float
  float ndShade = (tLight / 1000) - 0.5; // Calculate normalized differential shade

  // Display heading
  Serial.println("ndShade     tRight");
  Serial.print(tLight);           // Display tRight value
  Serial.print("       ");       // Display spaces
  Serial.print(ndShade);          // Display ndShade value
  Serial.println();               // Add an extra newline
  delay(500);                    // 1-second delay

  if (irLeft < threshold && irRight < threshold) {
    tone(4, 3000, 1000);
    delay(1000);
    stopCar();
    digitalWrite(8, LOW);
    digitalWrite(7, LOW);

    // Notify the device about the obstacle and wait for a command
    BT.println("Obstacle detected! Send '1' to turn right, '2' to stop and exit.");

    while (true) {
      if (BT.available()) {
        char command = BT.read();
        handleBluetoothCommand(command);
        break; // Exit the loop after handling the command
      }
    }
  } else if (irLeft < threshold) {
    turnSlightRight(); // Slightly turn to the right to avoid walls, obstacles, etc.
    moveForward();
  } else if (irRight < threshold) {
    turnSlightLeft(); // Slightly turn to the left to avoid walls, obstacles, etc.
    moveForward();
  } else if(tLight < 3000){
    stopCar();
  } else if(tLight > 3000){
    moveForward();
  }

  // Automatically move forward if the flag is set
  if (shouldMoveForward) {
    moveForward();
    shouldMoveForward = false;     // Reset the flag after moving forward
    shouldResumeCommands = true;   // Resume command execution
  }
}

int irDistance(int irLedPin, int irReceivePin) {
  int distance = 0;
  for (long f = 41000; f <= 42000; f += 500) {
    distance += irDetect(irLedPin, irReceivePin, f);
  }
  return distance;
}

int irDetect(int irLedPin, int irReceiverPin, long frequency) {
  tone(irLedPin, frequency, 8);
  delay(1);
  int ir = digitalRead(irReceiverPin);
  delay(1);
  return ir;
}

void maneuver(int speedLeft, int speedRight, int msTime) {
  servoLeft.writeMicroseconds(1500 + speedLeft);
  servoRight.writeMicroseconds(1500 - speedRight);
  if (msTime == -1) {
    servoLeft.detach();
    servoRight.detach();
  }
  delay(msTime);
}

long rcTime(int pin) {
  pinMode(pin, OUTPUT);       // Charge capacitor
  digitalWrite(pin, HIGH);   // Set pin output-high
  delay(5);                   // Wait for 5 ms
  pinMode(pin, INPUT);        // Set pin to input
  digitalWrite(pin, LOW);    // Set pin to low (no pullup)
  long time = micros();       // Mark the time
  while (digitalRead(pin));   // Wait for voltage < threshold
  time = micros() - time;     // Calculate decay time
  return time;                // Returns decay time
}

void handleBluetoothCommand(char command) {
  switch (command) {
    case '1':
      // Turn right immediately without moving forward
      turnRight();  // Adjust the speed and duration as needed
      moveForward(); // Move forward after the turn
      shouldMoveForward = true;  // Set the flag to continue moving forward
      break;
    case '2':
      // Turn left immediately without moving forward
      turnLeft();  // Adjust the speed and duration as needed
      moveForward(); // Move forward after the turn
      shouldMoveForward = true;  // Set the flag to continue moving forward
      break;
    case '3':
      moveBackward();
      shouldResumeCommands = true; // Stop command execution
      break;
    case '4':
      moveForward();
      shouldResumeCommands = true;  // Resume command execution
      break;
    case '5':
      stopCar();
      shouldResumeCommands = false; // Stop command execution
      break;
    // Add more commands as needed
  }
}

void moveForward() {
  maneuver(200, 200, 600);
}

void moveBackward() {
  maneuver(-200, -200, 600);
}

void stopCar() {
  maneuver(0, 0, 250);
  delay(500);
}

void turnRight() {
  maneuver(200, -200, 600);
}

void turnSlightRight() {
  maneuver(100, -100, 1000);
}

void turnLeft() {
  maneuver(-200, 200, 600);
}

void turnSlightLeft() {
  maneuver(100, -100, 1000);
}
