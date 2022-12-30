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

void loop() 
{
  while (Serial.available())
  {
    char c = Serial.read();
    Serial.write(c);
    Serial2.write(c);
    return;
  } 

  while(Serial2.available())
  {
    Serial.write("'");
    Serial.write(Serial2.read());
    Serial.println("'");
  } 
    
}