#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PID_v1.h>
#include <Pixy2.h>
#include <SoftwareSerial.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Run at 50 samples per second (20ms per sample)
#define SAMPLE_TIME 20

// Pulse values for centre position for each servo
int servoCentre[] = { 315, 315, 295 };

// Aim for centre of plate
double setPointX = 158, setPointY = 104;
int setPointDegrees = 0;

double ballX, ballY;
double outputX, outputY;
PID pidX(&ballX, &outputX, &setPointX, 0.15, 0.0, 0.05, DIRECT);
PID pidY(&ballY, &outputY, &setPointY, 0.15, 0.0, 0.05, DIRECT);
// 0.12, 0, 0.02

Pixy2 pixy;

SoftwareSerial bluetoothSerial(2,3); // RX, TX

int mode = 0;

void setServoAngle(uint8_t n, float angle) {
  double pulse = servoCentre[n] + angle * 2.5;
  pulse = constrain(pulse, 220, 390);
  pwm.setPWM(n, 0, pulse);
}

void setPlateAngle(int xAngle, float yAngle) {
  setServoAngle(0, -yAngle);
  setServoAngle(1, 0.866 * xAngle + yAngle * 0.5);
  setServoAngle(2, -0.866 * xAngle + yAngle * 0.5);
}

void setup() {
  Serial.begin(115200);
  bluetoothSerial.begin(9600);

  pixy.init();

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
    setPointX = 158;
    setPointY = 104;
  } else if ( mode == 2 ) {
    unsigned int setPointCounter = (time / 5000 ) % 4;
    if (setPointCounter == 0) {
      setPointX = 98;
      setPointY = 64;
    } else if (setPointCounter == 1) {
      setPointX = 218;
      setPointY = 64;
    } else if (setPointCounter == 2) {
      setPointX = 218;
      setPointY = 144;
    } else if (setPointCounter == 3) {
      setPointX = 98;
      setPointY = 144;
    }
  } else if ( mode == 3 ) {
    setPointDegrees += 5;
    if (setPointDegrees >= 360) {
      setPointDegrees = 0;
    }
    setPointX = 158 + 50 * sin(DEG_TO_RAD * setPointDegrees);
    setPointY = 104 + 40 * cos(DEG_TO_RAD * setPointDegrees);
  }

  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks) {
    ballX = pixy.ccc.blocks[0].m_x;
    ballY = pixy.ccc.blocks[0].m_y;

    int delayTime = SAMPLE_TIME - (millis() - time);
    if (delayTime > 0) {
      delay(delayTime);
    }
  
    pidX.Compute();
    pidY.Compute();

    setPlateAngle(outputX, outputY);

    Serial.print(setPointX);
    Serial.print(" ");
    Serial.print(setPointY);
    Serial.print(" ");
    Serial.print(ballX);
    Serial.print(" ");
    Serial.print(ballY);
    Serial.print(" ");
    Serial.print(outputX);
    Serial.print(" ");
    Serial.println(outputY);
  }
}
