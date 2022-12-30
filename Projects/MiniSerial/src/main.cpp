#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  mySerial.begin(115200);
}

void loop() // run over and over
{
  while (mySerial.available())
  {
    char c = mySerial.read();
    mySerial.write(c);
    Serial.write(c);
  }
  
  // if (Serial.available())
  // {
  //   String erg = "-";55
  //   erg += Serial.read();
  //   Serial.write(erg.c_str());
  // } 
    
}

