#include <stdint.h>
#include "TouchScreen.h"

#define YP A0  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A2   // can be a digital pin
#define XP A1   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 330);

const int LOOP_MICROS = 20000;

int16_t previousX = 0;
int16_t previousY = 0;
long loopEndTime = 0;

void setup(void) {
  Serial.begin(115200);
  loopEndTime = micros() + LOOP_MICROS;
}

void loop(void) {
  TSPoint p = ts.getPoint();

  if (p.x > 0) {
    previousX = p.x;
    previousY = p.y;
  }

  Serial.print("rX:"); Serial.print(p.x);
  Serial.print(", rY:"); Serial.print(p.y);
  Serial.print(", fX:"); Serial.print(previousX);
  Serial.print(", fY:"); Serial.print(previousY);
  Serial.println();

  long durationToEndTime = loopEndTime - micros();
  if (durationToEndTime > 0) {
    delayMicroseconds(LOOP_MICROS);
  }
  loopEndTime += LOOP_MICROS;
}
