#include <Adafruit_PWMServoDriver.h>
#include "PID_v1.h"
#include <SoftwareSerial.h>
#include <TouchScreen.h>

#define YP A0  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A2   // can be a digital pin
#define XP A1   // can be a digital pin

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 330);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Pulse values for centre position for each servo
int servoCentre[] = { 328, 315, 333 };

// Aim for centre of plate
double setPointX = 512, setPointY = 512;
int setPointDegrees = 0;

double ballX = -1, ballY = -1;
double outputX, outputY;
double kp = 0.01;
double ki = 0.003;
double kd = 0.008;
PID pidX(&ballX, &outputX, &setPointX, kp, ki, kd, DIRECT);
PID pidY(&ballY, &outputY, &setPointY, kp * 0.75, ki * 0.75, kd * 0.75, DIRECT);

SoftwareSerial bluetoothSerial(2, 3); // RX, TX

int mode = 1;

const int LOOP_MICROS = 20000;

long loopEndTime = 0;
int missedReadingCount = 10;

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

void setup() {
  Serial.begin(115200);
  bluetoothSerial.begin(38400);

  // Ignore first reading, which always seems to have an X value, even when there is no ball.
  TSPoint p = ts.getPoint();

  pwm.begin();
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates

  delay(10);
  setPlateAngle(0, 0);

  pidX.SetSampleTime(LOOP_MICROS / 1000);
  pidY.SetSampleTime(LOOP_MICROS / 1000);
  pidX.SetOutputLimits(-10, 10);
  pidY.SetOutputLimits(-10, 10);
  pidX.SetMode(AUTOMATIC);
  pidY.SetMode(AUTOMATIC);

  loopEndTime = micros() + LOOP_MICROS;
}

void loop() {
  /*
    if (bluetoothSerial.available()) {
    mode = bluetoothSerial.read(); //reads serial input
    mode = mode - '0';
    bluetoothSerial.print("New mode: ");
    bluetoothSerial.println(mode);
    }

    if (mode == 0) {
      setPlateAngle(0, 0);
      delay(100);
      return;
    }

    if (mode == 1) {
      setPointX = 512;
      setPointY = 512;
    } else if ( mode == 2 ) {
      unsigned int setPointCounter = (time / 5000 ) % 4;
      if (setPointCounter == 0) {
        setPointX = 256;
        setPointY = 256;
      } else if (setPointCounter == 1) {
        setPointX = 768;
        setPointY = 256;
      } else if (setPointCounter == 2) {
        setPointX = 768;
        setPointY = 768;
      } else if (setPointCounter == 3) {
        setPointX = 256;
        setPointY = 768;
      }
    } else if ( mode == 3 ) {
      setPointDegrees += 5;
      if (setPointDegrees >= 360) {
        setPointDegrees = 0;
      }
      setPointX = 512 + 300 * sin(DEG_TO_RAD * setPointDegrees);
      setPointY = 512 + 300 * cos(DEG_TO_RAD * setPointDegrees);
    }
  */
  TSPoint p = ts.getPoint();

  // Ignore any reading where there is no X value, or where the X or Y value changes by 50 (out of 1024).
  boolean xPresent = p.x > 0;
  boolean noXSpike = ballX == -1 || abs(ballX - p.x) < 50;
  boolean noYSpike = ballY == -1 || abs(ballY - p.y) < 50;

  boolean validReading = xPresent && noXSpike && noYSpike;
  if (validReading) {
    // Restart the PID controllers if the ball is placed back on the plate
    if (missedReadingCount >= 10) {
      pidX.SetMode(AUTOMATIC);
      pidY.SetMode(AUTOMATIC);
    }

    ballX = p.x;
    ballY = p.y;
    missedReadingCount = 0;

  } else if (!xPresent && missedReadingCount < 10) {
    // Missed reading (not counting invalid "spike" readings)
    missedReadingCount += 1;
  }

  while (loopEndTime > micros());
  loopEndTime += LOOP_MICROS;

  // Assume the ball is removed if we have 10 contiguous missed readings
  if (missedReadingCount >= 10) {
    ballX = -1;
    ballY = -1;
    setPlateAngle(0, 0);
    pidX.SetMode(MANUAL);
    pidY.SetMode(MANUAL);
    outputX = 0;
    outputY = 0;

  } else {
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
}
