//pio device monitor -p COM4 -b 115200
#include <Arduino.h>

#define RXD2 16
#define TXD2 17
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}
String command = "";
void loop() 
{
  if (Serial.available())
  {
    char c = Serial.read();
    Serial.write(c);
    command += c;
    if (c == 13)
    {
      Serial2.println(command);
      command = "";
    }
  } 

  if (Serial2.available())
  {
    Serial.write(Serial2.read());
  } 
    
}