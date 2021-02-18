#include <stdint.h>
#include "TouchScreen.h"

#define YP A0  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A2   // can be a digital pin
#define XP A1   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 330);

const int LOOP_MICROS = 20000;

int16_t previousX = -1;
int16_t previousY = -1;
unsigned long loopEndTime = 0;
int16_t missedReadingCount = 10;

void setup(void) {
  Serial.begin(115200);

  // Ignore first reading, which always seems to have an X value, even when there is no ball.
  TSPoint p = ts.getPoint();

  loopEndTime = micros() + LOOP_MICROS;
}

void loop(void) {
  TSPoint p = ts.getPoint();

  // Ignore any reading where there is no X value, or where the X or Y value changes by 50 (out of 1024).
  boolean xPresent = p.x > 0;
  boolean noXSpike = previousX == -1 || abs(previousX - p.x) < 50;
  boolean noYSpike = previousY == -1 || abs(previousY - p.y) < 50;
  if (xPresent && noXSpike && noYSpike) {
    previousX = p.x;
    previousY = p.y;
    missedReadingCount = 0;
  } else if (!xPresent && missedReadingCount < 10) {
    missedReadingCount += 1;
  }

  // Assume the ball is removed if we have 10 contiguous missed readings
  if (missedReadingCount >= 10) {
    previousX = -1;
    previousY = -1;
  } else {
    //Serial.print(", rX:"); Serial.print(p.x);
    //Serial.print(", rY:"); Serial.print(p.y);
    Serial.print(", fX:"); Serial.print(previousX);
    Serial.print(", fY:"); Serial.print(previousY);
    Serial.println();
  }

  while (loopEndTime > micros());
  loopEndTime += LOOP_MICROS;
}
