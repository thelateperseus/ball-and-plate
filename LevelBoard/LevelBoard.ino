#include <Servo.h>

Servo servos[3];

//int servoCentre[] = { 1500, 1500, 1500 }; // centering
//int servoCentre[] = { 1570, 1500, 1605 }; // MG996R, long arms, ball is stationary
//int servoCentre[] = { 1580, 1545, 1470 }; // JX PDI-6113MG, ball close to stationary
int servoCentre[] = { 1580, 1480, 1480 }; // MG996R, short arms, ball is stationary
//int servoCentre[] = { 1540, 1520, 1560 }; // JX PDI-6113MG, short arms, ball is stationary

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
