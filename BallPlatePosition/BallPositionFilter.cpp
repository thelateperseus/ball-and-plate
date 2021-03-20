#include <Arduino.h>
#include "BallPositionFilter.h"

#define SPIKE_THRESHOLD 60
#define MISSED_READING_LIMIT 10
#define SPIKE_LIMIT 2

BallPositionFilter::BallPositionFilter(TouchScreen* ts) {
  touchScreen = ts;
  previousX = -1;
  previousY = -1;
  spikeCount = 0;

  // Start by assuming the ball is not present
  missedReadingCount = MISSED_READING_LIMIT;

  // Ignore first reading, which always seems to have an X value, even when there is no ball.
  //TSPoint p = touchScreen -> getPoint();

  filterX = LowPassFilter();
  filterY = LowPassFilter();
}

TSPoint BallPositionFilter::getPoint() {
  TSPoint p = touchScreen -> getPoint();

  bool xPresent = p.x > 0;
  bool spikeX = previousX != -1 && abs(previousX - p.x) >= SPIKE_THRESHOLD;
  bool spikeY = previousY != -1 && abs(previousY - p.y) >= SPIKE_THRESHOLD;
  bool spike = (spikeX || spikeY) && spikeCount < SPIKE_LIMIT;

  // Ignore any reading where there is no X value, or where the X or Y value dramatically by 50 (out of 1024).
  bool skipReading = !xPresent || spike;

  if (!skipReading) {
    previousX = p.x;
    previousY = p.y;
    spikeCount = 0;
    missedReadingCount = 0;

#ifdef RAW_POSITION_DEBUG
    Serial.print("rX:"); Serial.print(p.x);
    Serial.print(", rY:"); Serial.print(p.y);
#endif

#ifdef LOW_PASS_FILTER
    p.x = filterX.step(p.x);
    p.y = filterY.step(p.y);
#endif

#ifdef INTERMEDIATE_POSITION_DEBUG
    Serial.print(", iX:"); Serial.print(p.x);
    Serial.print(", iY:"); Serial.print(p.y);
#endif

  } else {
    if (isPresent()) {
      missedReadingCount += 1;
    }
    if (isPresent()) {
#ifdef RAW_POSITION_DEBUG
      Serial.print("rX:"); Serial.print(p.x);
      Serial.print(", rY:"); Serial.print(p.y);
#endif

#ifdef INTERMEDIATE_POSITION_DEBUG
      Serial.print(", iX:"); Serial.print(previousX);
      Serial.print(", iY:"); Serial.print(previousY);
#endif

#ifdef LOW_PASS_FILTER
      p.x = filterX.step(previousX);
      p.y = filterY.step(previousY);
#else
      p.x = previousX;
      p.y = previousY;
#endif
    } else {
      p.x = 0;
      p.y = 0;
      p.z = 0;
      previousX = -1;
      previousY = -1;
#ifdef LOW_PASS_FILTER
      filterX.reset();
      filterY.reset();
#endif
    }

    if (spike) {
      spikeCount += 1;
    }
  }

  return p;
}

bool BallPositionFilter::isPresent() {
  return missedReadingCount < MISSED_READING_LIMIT;
}
