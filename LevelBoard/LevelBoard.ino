#include <Servo.h>

Servo servos[3];

// Ball is stationary at 1570, 1500, 1605 with MG996R
int servoCentre[] = { 1580, 1545, 1470 };

void setup() {
  Serial.begin(115200);
  Serial.println("Level plate");

  servos[0].attach(7);
  servos[1].attach(8);
  servos[2].attach(9);

  for (int i = 0; i < 3; i++) {
    servos[i].writeMicroseconds(servoCentre[i]);
  }
}

void loop() {
  delay(500);
}
