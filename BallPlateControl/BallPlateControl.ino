#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PID_v1.h>
#include <SoftwareSerial.h>
#include <stdint.h>
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

// Run at 50 samples per second (20ms per sample)
#define SAMPLE_TIME 20

// Pulse values for centre position for each servo
int servoCentre[] = { 328, 315, 333 };

// Aim for centre of plate
double setPointX = 512, setPointY = 512;
int setPointDegrees = 0;

double ballX, ballY;
double outputX, outputY;
PID pidX(&ballX, &outputX, &setPointX, 0.05, 0.0, 0.007, DIRECT);
PID pidY(&ballY, &outputY, &setPointY, 0.0, 0.0, 0.0, DIRECT);
// 0.12, 0, 0.02

SoftwareSerial bluetoothSerial(2,3); // RX, TX

int mode = 0;

void setServoAngle(uint8_t n, float angle) {
  double pulse = servoCentre[n] + angle * 2.5;
  pulse = constrain(pulse, 220, 390);
  pwm.setPWM(n, 0, pulse);
}

void setPlateAngle(int xAngle, int yAngle) {
  setServoAngle(0, xAngle);
  setServoAngle(1, 0.866 * yAngle - xAngle * 0.5);
  setServoAngle(2, -0.866 * yAngle - xAngle * 0.5);
}

void setup() {
  Serial.begin(115200);
  bluetoothSerial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates

  delay(10);
  setPlateAngle(0, 0);

  pidX.SetSampleTime(SAMPLE_TIME);
  pidY.SetSampleTime(SAMPLE_TIME);
  pidX.SetOutputLimits(-25, 25);
  pidY.SetOutputLimits(-25, 25);
  pidX.SetMode(AUTOMATIC);
  pidY.SetMode(AUTOMATIC);
}

void loop() {
  unsigned long time = millis();

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

  TSPoint p = ts.getPoint();

  // TODO: use previous measurement if no reading, or if previous reading was +-50 different
  // TODO: reset controller if no readings for 10 counts
  if (p.x > 0) {
    ballX = p.x;
    ballY = p.y;

    int delayTime = SAMPLE_TIME - (millis() - time);
    if (delayTime > 0) {
      delay(delayTime);
    }
  
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
    /*Serial.print(" ");
    Serial.print(outputX);
    Serial.print(" ");
    Serial.print(outputY);*/
    Serial.println();
  }
}
