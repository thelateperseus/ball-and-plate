#define LOW_PASS_FILTER 1

#include <Adafruit_PWMServoDriver.h>
#include "PID_v1.h"
#include <SoftwareSerial.h>
#include <TouchScreen.h>
#include "BallPositionFilter.h"

#define YP A0  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A2   // can be a digital pin
#define XP A1   // can be a digital pin

// Resistance between X+ and X- is 330 ohms
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 330);
BallPositionFilter ballPosition = BallPositionFilter(&ts);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Pulse values for centre position for each servo
int servoCentre[] = { 328, 315, 333 };

// Aim for centre of plate
#define CENTRE_X 480
#define CENTRE_Y 510
double setPointX = CENTRE_X, setPointY = CENTRE_Y;
int setPointDegrees = 0;

double ballX = -1, ballY = -1;
double outputX, outputY;
const double KP = 0.03;
const double KI = 0.002; //0.003;
const double KD = 0.009;
const double ASPECT_RATIO = 0.71; // Ideally 0.75
PID pidX(&ballX, &outputX, &setPointX, KP, KI, KD, DIRECT);
PID pidY(&ballY, &outputY, &setPointY, KP * ASPECT_RATIO, KI * ASPECT_RATIO, KD * ASPECT_RATIO, DIRECT);

SoftwareSerial bluetoothSerial(2, 3); // RX, TX

int mode = 1;

const int LOOP_MICROS = 20000;

long loopEndTime = 0;

void setServoAngle(uint8_t n, double angle) {
  double pulse = servoCentre[n] + angle * 2.5;
  pulse = constrain(pulse, 260, 380);
  pwm.setPWM(n, 0, pulse);
}

void setPlateAngle(double xAngle, double yAngle) {
  setServoAngle(0, xAngle);
  setServoAngle(1, 0.866 * yAngle - xAngle * 0.5);
  setServoAngle(2, -0.866 * yAngle - xAngle * 0.5);
}

void resetPID() {
  ballX = -1;
  ballY = -1;
  setPlateAngle(0, 0);
  pidX.SetMode(MANUAL);
  pidY.SetMode(MANUAL);
  outputX = 0;
  outputY = 0;
}

void computeSetPoint(int mode) {
  if (mode == 1) {
    setPointX = CENTRE_X;
    setPointY = CENTRE_Y;
  } else if ( mode == 2 ) {
    unsigned int setPointCounter = (millis() / 5000 ) % 4;
    if (setPointCounter == 0) {
      setPointX = CENTRE_X - 175;
      setPointY = CENTRE_Y - 175;
    } else if (setPointCounter == 1) {
      setPointX = CENTRE_X + 175;
      setPointY = CENTRE_Y - 175;
    } else if (setPointCounter == 2) {
      setPointX = CENTRE_X + 175;
      setPointY = CENTRE_Y + 175;
    } else if (setPointCounter == 3) {
      setPointX = CENTRE_X - 175;
      setPointY = CENTRE_Y + 175;
    }
  } else if ( mode == 3 ) {
    setPointDegrees += 3;
    if (setPointDegrees >= 360) {
      setPointDegrees = 0;
    }
    setPointX = CENTRE_X + 200 * sin(DEG_TO_RAD * setPointDegrees);
    setPointY = CENTRE_Y + 200 * cos(DEG_TO_RAD * setPointDegrees);
  }
}

void setup() {
  Serial.begin(115200);
  bluetoothSerial.begin(115200);

  pwm.begin();
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates

  delay(10);
  setPlateAngle(0, 0);

  pidX.SetSampleTime(LOOP_MICROS / 1000);
  pidY.SetSampleTime(LOOP_MICROS / 1000);
  pidX.SetOutputLimits(-15, 15);
  pidY.SetOutputLimits(-15, 15);
  pidX.SetMode(AUTOMATIC);
  pidY.SetMode(AUTOMATIC);

  loopEndTime = micros() + LOOP_MICROS;
}

void loop() {
  int oldMode = mode;
  if (bluetoothSerial.available()) {
    mode = bluetoothSerial.read(); //reads serial input
    mode = mode - '0';
    bluetoothSerial.print("New mode: ");
    bluetoothSerial.println(mode);
  }

  if (mode == 0) {
    delay(100);
    if (oldMode != 0) {
      resetPID();
    }
    return;
  } else if (oldMode == 0) {
    pidX.SetMode(AUTOMATIC);
    pidY.SetMode(AUTOMATIC);
  }

  computeSetPoint(mode);

  boolean wasPresent = ballPosition.isPresent();
  TSPoint p = ballPosition.getPoint();

  if (ballPosition.isPresent()) {
    // Restart the PID controllers if the ball is placed back on the plate
    if (!wasPresent) {
      pidX.SetMode(AUTOMATIC);
      pidY.SetMode(AUTOMATIC);
    }

    ballX = p.x;
    ballY = p.y;

    pidX.Compute();
    pidY.Compute();

    setPlateAngle(outputX, outputY);

    Serial.print("spX:");
    Serial.print(setPointX);
    Serial.print(", spY:");
    Serial.print(setPointY);
    Serial.print(", x:");
    Serial.print(ballX);
    Serial.print(", y:");
    Serial.print(ballY);
    //Serial.print(", outX:");
    //Serial.print(outputX);
    //Serial.print(", outY:");
    //Serial.print(outputY);
    Serial.println();
  }

  // Assume the ball is removed if we have 10 contiguous missed readings
  else {
    resetPID();
  }

  while (loopEndTime > micros());
  loopEndTime += LOOP_MICROS;
}
