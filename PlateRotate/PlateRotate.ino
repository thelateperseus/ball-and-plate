/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815
  
  These drivers use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

#define MAX_ANGLE 15
#define ANGLE_STEP 0.5

int servoCentre[] = { 328, 315, 333 };

void setServoAngle(uint8_t n, double angle) {
  double pulse = servoCentre[n] + angle * 2.5;
  pulse = constrain(pulse, 220, 390);
  pwm.setPWM(n, 0, pulse);
}

void setPlateAngle(double xAngle, double yAngle) {
  setServoAngle(0, xAngle);
  setServoAngle(1, 0.866 * yAngle - xAngle * 0.5);
  setServoAngle(2, -0.866 * yAngle - xAngle * 0.5);
}

void setup() {
  Serial.begin(115200);

  pwm.begin();
  // In theory the internal oscillator is 25MHz but it really isn't
  // that precise. You can 'calibrate' by tweaking this number till
  // you get the frequency you're expecting!
  //pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
  setPlateAngle(0, 0);
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
