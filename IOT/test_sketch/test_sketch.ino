//centering
#include <Servo.h>
Servo servoR;
Servo servoL;

void setup() {
  // put your setup code here, to run once:
  servoR.attach(13);
  servoL.attach(12);

  servoR.writeMicroseconds(1500);
  servoL.writeMicroseconds(1500);
}

void loop() {
  // put your main code here, to run repeatedly:

} 
