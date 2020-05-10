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
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

Adafruit_MPU6050 mpu;

int servoCentre[] = { 315, 315, 295 };

void setServoAngle(uint8_t n, int angle) {
  double pulse = servoCentre[n] + angle * 2.5;
  pulse = constrain(pulse, 220, 390);
  pwm.setPWM(n, 0, pulse);
}

void setPlateAngle(int xAngle, int yAngle) {
  setServoAngle(0, -yAngle);
  setServoAngle(1, 0.866 * xAngle + yAngle * 0.5);
  setServoAngle(2, -0.866 * xAngle + yAngle * 0.5);

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print(a.acceleration.x);
  Serial.print("\t");
  Serial.print(a.acceleration.y);
  Serial.print("\t");
  Serial.print(xAngle);
  Serial.print("\t");
  Serial.println(yAngle);
}

void setup() {
  Serial.begin(115200);

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pwm.begin();
  // In theory the internal oscillator is 25MHz but it really isn't
  // that precise. You can 'calibrate' by tweaking this number till
  // you get the frequency you're expecting!
  //pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates

  delay(10);
  setPlateAngle(0, 0);
}

void loop() {
  for (int yAngle = 0; yAngle < 20; yAngle++) {
    setPlateAngle(0, yAngle);
    delay(50);
  }

  for (int xAngle = 0; xAngle < 20; xAngle++) {
    setPlateAngle(xAngle, 20);
    delay(50);
  }

  for (int yAngle = 20; yAngle > -20; yAngle--) {
    setPlateAngle(20, yAngle);
    delay(50);
  }

  for (int xAngle = 20; xAngle > -20; xAngle--) {
    setPlateAngle(xAngle, -20);
    delay(50);
  }

  for (int yAngle = -20; yAngle < 20; yAngle++) {
    setPlateAngle(-20, yAngle);
    delay(50);
  }

  for (int xAngle = -20; xAngle < 0; xAngle++) {
    setPlateAngle(xAngle, 20);
    delay(50);
  }

  for (int yAngle = 20; yAngle >= 0; yAngle--) {
    setPlateAngle(0, yAngle);
    delay(50);
  }
  delay(5000);
}
