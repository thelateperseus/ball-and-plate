//#define LOW_PASS_FILTER

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
    double step(double x) //class II
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = v[3];
      v[3] = v[4];
      v[4] = (9.464345202318061312e-2 * x)
             + (-0.00522871651374407463 * v[0])
             + (-0.01347637374357090259 * v[1])
             + (-0.26869137916361751639 * v[2])
             + (-0.22689876294995714456 * v[3]);
      return
        (v[0] + v[4])
        + 4 * (v[1] + v[3])
        + 6 * v[2];
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
    bool previousSpike;
    LowPassFilter filterX, filterY;
};
