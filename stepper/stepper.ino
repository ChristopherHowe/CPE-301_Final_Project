#include <Stepper.h>
// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


void setup() {
// Nothing to do (Stepper Library sets pins as outputs)
}

void loop() {
  // put your main code here, to run repeatedly:
  myStepper.setSpeed(10);
  myStepper.step(-stepsPerRevolution);
  delay(1000);
}
