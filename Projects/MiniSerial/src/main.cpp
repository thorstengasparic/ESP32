//pio device monitor -p COM4 -b 115200
#include <Arduino.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */
void getValues(void);

SoftwareSerial mySerial(2, 3); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  mySerial.begin(9600);

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }


}

String command = "";
void loop() // run over and over
{
   getValues();
  if (mySerial.available())
  {
    char c = mySerial.read();
    //Serial.write(c);
    command += c;
    if (c == 13)
    {
      mySerial.print(command);
      command = "";
    }
  } 
}

double volts0Cur, volts1Volt, volts2Ref;
double measurecurrent, measurevoltage, measurepower;
unsigned long measureMillis = 0;
void getValues(void)
{
  unsigned long currentMillis = millis();
  if ((currentMillis- measureMillis) > 1000)
  {
    
    int16_t adc0 = ads.readADC_SingleEnded(0);
    int16_t adc1 = ads.readADC_SingleEnded(1);
    int16_t adc2 = ads.readADC_SingleEnded(2);
    
    volts0Cur = ads.computeVolts(adc0);
    volts1Volt = ads.computeVolts(adc1);
    volts2Ref = ads.computeVolts(adc2);
    
    double voltsDelta = volts0Cur -volts2Ref;
    measurecurrent = voltsDelta*22.14;
    measurevoltage = volts1Volt*12.0;
    measurepower =measurevoltage*measurecurrent;

    Serial.print("VCur0="); Serial.print(volts0Cur); Serial.print("\t VRef2="); Serial.print(volts2Ref); Serial.print("\t VVolt1="); Serial.print(volts1Volt);
    Serial.print("\tV="); Serial.print(measurevoltage); Serial.print("\t A="); Serial.print(measurecurrent);
    Serial.print("\t P="); Serial.println(measurepower);
    
     measureMillis = millis();
   }
}
