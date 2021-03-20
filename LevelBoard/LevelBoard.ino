#include <Servo.h>

Servo servos[3];

void setup() {
  Serial.begin(115200);
  Serial.println("Level plate");

  servos[0].attach(7);
  servos[1].attach(8);
  servos[2].attach(9);

  // Ball is stationary at 1570, 1500, 1605
  servos[0].writeMicroseconds(1570);
  servos[1].writeMicroseconds(1500);
  servos[2].writeMicroseconds(1605);
}

void loop() {
  delay(500);
}
