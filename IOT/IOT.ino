#include <Servo.h>                           // Include servo library
Servo servoLeft;                             // Declare left and right servos
Servo servoRight;
const int setpoint = 2;                      // Target distances
const int kpl = -50;                         // Proportional control constants
const int kpr = -50;
int speed = 0;

// Initialize the threshold variable
const int threshold = 1; // Adjust this value based on your requirements
 
void setup()                                 // Built-in initialization block
{ pinMode(10, INPUT);  pinMode(9, OUTPUT);   // Left IR LED & Receiver
  pinMode(3, INPUT);  pinMode(2, OUTPUT);    // Right IR LED & Receiver
  pinMode(8, OUTPUT); pinMode(7, OUTPUT);// Indicator LEDs
  tone(4, 3000, 1000);                       // Play tone for 1 second
  delay(1000);                               // Delay to finish tone
  servoLeft.attach(13);                      // Attach left signal to pin 13
  servoRight.attach(12);
  // Attach right signal to pin 12
  pinMode(setpoint, OUTPUT); //추가
} 
void loop()
{
  int irLeft = irDistance(9, 10);  // Measure left distance
  int irRight = irDistance(2, 3); // Measure right distance

  digitalWrite(8, !irLeft);  // LED states opposite of IR
  digitalWrite(7, !irRight);

  // Check if an obstacle is detected
  if (irLeft < threshold || irRight < threshold)
  {
    // Stop the car if an obstacle is detected
    maneuver(0, 0, 250); // Set both servo speeds to 0, msTime = -1 to stop indefinitely

    // Turn off the LEDs
    digitalWrite(8, LOW);
    digitalWrite(7, LOW);

    // Delay for a short time to allow the car to come to a complete stop
    delay(700);

    // Maneuver to the right if an obstacle is detected in front
    maneuver(70, -70, 500);
    // Adjust the values based on your requirement
    
  }
  else
  {
    // Calculate drive levels only if no obstacle is detected
    int driveLeft = (setpoint - irLeft) * kpl;
    int driveRight = (setpoint - irRight) * kpr;
    maneuver(driveLeft, driveRight, 20); // drive levels set speeds
  }
}
int irDistance(int irLedPin, int irReceivePin)  // IR distance measurement function
{ int distance = 0;
  for(long f = 38000; f <= 42000; f += 1000) {
    distance += irDetect(irLedPin, irReceivePin, f);
  }
  return distance;
}
// IR Detection function
int irDetect(int irLedPin, int irReceiverPin, long frequency)
{tone(irLedPin, frequency, 8);        // IRLED 38 kHz for at least 1 ms
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  delay(1);                                  // Down time before recheck
  return ir;                                 // Return 1 no detect, 0 detect
}     
void maneuver(int speedLeft, int speedRight, int msTime)
{ // speedLeft, speedRight ranges: Backward  Linear Stop Linear Forward
  //                               -200      -100......0......100       200
  servoLeft.writeMicroseconds(1500 + speedLeft);   // Set Left servo speed
  servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
  if(msTime==-1)                                   // if msTime = -1
  { servoLeft.detach();                            // Stop servo signals
    servoRight.detach();   
  }
  delay(msTime);                                   // Delay for msTime
}
