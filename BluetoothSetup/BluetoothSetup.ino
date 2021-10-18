// Connect the Hc-05 module and communicate using the serial monitor
//
// The HC-05 needs to be set to AT mode when powered on.
// The default baud rate is 38400
// The Hc-05 requires all AT commands to be in uppercase. NL+CR MUST be added to the command string

// Connect the HC-05 TX to the Arduino Nano Every RX on pin 0.
// Connect the HC-05 RX to the Arduino Nano Every TX on pin 1.
//


void setup()
{
  Serial.begin(38400);
  while (!Serial);
  Serial.println("Enter AT commands:");

  // HC-06 default serial speed is 9600
  Serial1.begin(38400);
}

void loop()
{

  // Keep reading from HC-06 and send to Arduino Serial Monitor
  if (Serial1.available()) {
    Serial.write(Serial1.read());
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  // Keep reading from Arduino Serial Monitor and send to HC-06
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }

}
