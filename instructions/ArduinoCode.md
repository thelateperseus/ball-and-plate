Aruino Code
===========

The main controller code lives in the [BallPlateControl.ino](../BallPlateControl/BallPlateControl.ino) sketch. However, you should use the other sketches listed below first to check that the system components are functioning correctly.

Check Touch Panel
-----------------

1. Connect the USB cable to the Arduino, but do not connect DC power to the PCB.
2. Using the Arduino IDE, upload the [BallPlatePosition.ino](../BallPlatePosition/BallPlatePosition.ino) sketch to the Arduino.
3. Open the Serial Monitor to view the output.
4. Roll the ball slowly across the touch panel.
5. Check that the displayed X and Y positions are as expected. If not, try unplugging the touch panel from the PCB and plugging it in the other way around.

Configure Bluetooth Module
--------------------------

1. Set the HC-05 bluetooth module to AT mode as per [How to Use HC-05 Bluetooth Modules](https://core-electronics.com.au/tutorials/how-to-use-bluetooth-modules.html). The actual steps will depend on which variant of the HC-05 you have.
2. Connect the USB cable to the Arduino, but do not connect DC power to the PCB.
3. Using the Arduino IDE, upload the [BluetoothSetup.ino](../BluetoothSetup/BluetoothSetup.ino) sketch to the Arduino.
4. Open the Serial Monitor to view the output.
5. Type the AT commands below to configure the HC-05 module. These settings will be saved to permanent storage.
6. Uplug the USB cable from the Arduino to reset the HC-05 module back to normal mode..

```
AT+NAME:BallPlate
AT+PSWD="2222"
AT+UART:115200,0,0
```

Level Touch Panel
-----------------

1. Ensure that SW1 is open, i.e. no jumper connects the 2 pins.
2. Connect the USB cable to the Arduino, and also connect DC power to the PCB.
3. Using the Arduino IDE, upload the [LevelBoard.ino](../LevelBoard/LevelBoard.ino) sketch to the Arduino.
4. Place the ball on the touch panel.
5. Adjust the values in the `servoCentre` array and re-upload the code to the Arduino until the ball stays stationary.

Controller Software
-----------------

1. Ensure that SW1 is open, i.e. no jumper connects the 2 pins.
2. Connect the USB cable to the Arduino, and also connect DC power to the PCB.
3. Using the Arduino IDE, open the [BallPlateControl.ino](../BallPlateControl/BallPlateControl.ino) sketch.
4. Adjust the values in the `servoCentre` array as per the previous "Level Touch Panel" step.
5. Upload the sketch to the Arduino.
6. Place the ball on the touch panel. The controller should try to keep it in the centre.
7. Adjust PID constants if further tuning is required.

**Important Note:** SW1 on the PCB controls whether the Arduino is powered from the 5V regulator on the PCB, or from USB. 

* To power from the 5V regulator, close SW1 using a jumper (aka busbar). Insert the micro USB dust cover into the Arduino USB port so you don't accidentally connect the USB power as well and fry everything.
* If you need to make changes to the code or perform debugging, make sure you remove the jumper from SW1 before connecting the USB cable!!!
