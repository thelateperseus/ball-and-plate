#include <Servo.h>

#define MAX_ANGLE 25
#define ANGLE_STEP 0.5

Servo servos[3];

// Pulse values for centre position for each servo
int servoCentre[] = { 1570, 1500, 1605 };

void setServoAngle(uint8_t n, double angle) {
  double pulse = servoCentre[n] + angle * 5.556;
  int offset = servoCentre[n] - 1500;
  pulse = constrain(pulse, 1300 + offset, 1700 + offset);
  servos[n].writeMicroseconds(pulse);
  Serial.print("n:");
  Serial.print(n);
  Serial.print(", angle:");
  Serial.print(angle);
  Serial.print(", pulse:");
  Serial.print(pulse);
  Serial.println();
}

void setPlateAngle(double xAngle, double yAngle) {
  setServoAngle(0, xAngle);
  setServoAngle(1, 0.866 * yAngle - xAngle * 0.5);
  setServoAngle(2, -0.866 * yAngle - xAngle * 0.5);
}

void setup() {
  Serial.begin(115200);

  servos[0].attach(7);
  servos[1].attach(8);
  servos[2].attach(9);

  for (int i = 0; i < 3; i++) {
    servos[i].writeMicroseconds(servoCentre[i]);
  }
}

void loop() {
  for (double yAngle = 0; yAngle < MAX_ANGLE; yAngle += ANGLE_STEP) {
    setPlateAngle(0, yAngle);
    delay(50);
  }

  for (double xAngle = 0; xAngle < MAX_ANGLE; xAngle += ANGLE_STEP) {
    setPlateAngle(xAngle, MAX_ANGLE);
    delay(50);
  }

  for (double yAngle = MAX_ANGLE; yAngle > -MAX_ANGLE; yAngle -= ANGLE_STEP) {
    setPlateAngle(MAX_ANGLE, yAngle);
    delay(50);
  }

  for (double xAngle = MAX_ANGLE; xAngle > -MAX_ANGLE; xAngle -= ANGLE_STEP) {
    setPlateAngle(xAngle, -MAX_ANGLE);
    delay(50);
  }

  for (double yAngle = -MAX_ANGLE; yAngle < MAX_ANGLE; yAngle += ANGLE_STEP) {
    setPlateAngle(-MAX_ANGLE, yAngle);
    delay(50);
  }

  for (double xAngle = -MAX_ANGLE; xAngle < 0; xAngle += ANGLE_STEP) {
    setPlateAngle(xAngle, MAX_ANGLE);
    delay(50);
  }

  for (double yAngle = MAX_ANGLE; yAngle > 0; yAngle -= ANGLE_STEP) {
    setPlateAngle(0, yAngle);
    delay(50);
  }
  delay(5000);
}
