#define LOW_PASS_FILTER
//#define RAW_POSITION_DEBUG
//#define INTERMEDIATE_POSITION_DEBUG

#include "TouchScreen.h"

class LowPassFilter
{
  public:
    LowPassFilter()
    {
      reset();
    }
  private:
    double v[5];
  public:
    float step(float x) //class II
    {
      v[0] = v[1];
      v[1] = (2.452372752527856026e-1 * x)
             + (0.50952544949442879485 * v[0]);
      return
        (v[0] + v[1]);
    }

    void reset() {
      for (int i = 0; i <= 4; i++) {
        v[i] = 0.0;
      }
    }
};

class BallPositionFilter {
  public:
    BallPositionFilter(TouchScreen* ts);

    TSPoint getPoint();
    bool isPresent();

  private:
    TouchScreen* touchScreen;
    int16_t previousX, previousY;
    int missedReadingCount;
    int spikeCount;
    LowPassFilter filterX, filterY;
};
