#include <stdint.h>
#include "TouchScreen.h"
#include "BallPositionFilter.h"

#define YP A0  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A2   // can be a digital pin
#define XP A1   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 330);
BallPositionFilter ballPosition = BallPositionFilter(&ts);

const int LOOP_MICROS = 20000;

unsigned long loopEndTime = 0;

void setup(void) {
  Serial.begin(115200);

  loopEndTime = micros() + LOOP_MICROS;
}

void loop(void) {
  TSPoint p = ballPosition.getPoint();

  // Assume the ball is removed if we have 10 contiguous missed readings
  if (ballPosition.isPresent()) {
#ifdef RAW_POSITION_DEBUG
    Serial.print(", ");
#endif
    Serial.print("fX: ");
    Serial.print(p.x);
    Serial.print(", fY: ");
    Serial.print(p.y);
    Serial.println();
  }

  while (loopEndTime > micros());
  loopEndTime += LOOP_MICROS;
}
