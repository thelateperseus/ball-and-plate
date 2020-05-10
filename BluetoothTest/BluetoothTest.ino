#include <SoftwareSerial.h>

SoftwareSerial bt(2,3); // RX, TX

int thisByte = 33;
char mode = '1';

void setup() {
  bt.begin(9600);
  bt.println("ASCII Table ~ Character Map");
}

void loop() {
  if (bt.available())
  {
    mode = bt.read(); //reads serial input
    bt.print("mode: ");
    bt.print(mode);
    bt.print(" ");
    bt.println((int)mode);
    //delay(1000);
  }

  /*if (mode=='0') {
    return;
  }

  bt.write(thisByte);

  bt.print(", dec: ");
  bt.print(thisByte);
  bt.print(", hex: ");
  bt.print(thisByte, HEX);
  bt.print(", oct: ");
  bt.print(thisByte, OCT);
  bt.print(", bin: ");
  bt.println(thisByte, BIN);

  if(thisByte == 126) {
    thisByte = 32;
  }
  thisByte++;
  delay(200);*/
}
