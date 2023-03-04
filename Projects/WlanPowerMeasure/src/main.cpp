// #include <Adafruit_ADS1X15.h>

// //Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

// void setup(void)
// {
//   Serial.begin(115200);
//   Serial.println("Hello!");

//   Serial.println("Getting single-ended readings from AIN0..3");
//   Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

//   // The ADC input range (or gain) can be changed via the following
//   // functions, but be careful never to exceed VDD +0.3V max, or to
//   // exceed the upper and lower limits if you adjust the input range!
//   // Setting these values incorrectly may destroy your ADC!
//   //                                                                ADS1015  ADS1115
//   //                                                                -------  -------
//   // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
//   // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
//   // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
//   // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
//   // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
//   // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

//   if (!ads.begin()) {
//     Serial.println("Failed to initialize ADS.");
//     while (1);
//   }
//   ads.setGain()
// }

// void loop(void)
// {
//   int16_t adc0, adc1, adc2, adc3;
//   float volts0, volts1, volts2, volts3;

//   adc0 = ads.readADC_SingleEnded(0);
//   adc1 = ads.readADC_SingleEnded(1);
//   adc2 = ads.readADC_SingleEnded(2);
//   adc3 = ads.readADC_SingleEnded(3);

//   volts0 = ads.computeVo2<<lts(adc0);
//   volts1 = ads.computeVolts(adc1);
//   volts2 = ads.computeVolts(adc2);
//   volts3 = ads.computeVolts(adc3);

//   Serial.println("-----------------------------------------------------------");
//   Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
//   Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println("V");
//   //Serial.print("AIN2: "); Serial.print(adc2); Serial.print("  "); Serial.print(volts2); Serial.println("V");
//   //Serial.print("AIN3: "); Serial.print(adc3); Serial.print("  "); Serial.print(volts3); Serial.println("V");

//   delay(1000);
// }

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
 #include <EEPROM.h>
 #include <EEPROMProvider.h>
 #include <WlanConnetor.h>
 #include <textreplacement.h>
 #include <table.h>

#define VERSION "1.0"

#define STATUS_LED LED_BUILTIN

String HttpContentFunction();
EEPROMProvider* eeprom = NULL;
WlanConnector *wlanConnector = NULL;
 void getValues(void);

Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */

#if defined(ESP8266)
#pragma message "ESP8266"
#define wifiModePin D6
#elif defined(ESP32)
#pragma message "ESP32"
#define wifiModePin 5
#else
#error "This ain't a ESP8266 or ESP32"
#endif

void setup() {
  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");

  eeprom = new EEPROMProvider(VERSION);
 wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, eeprom);

  wlanConnector->SetCallback(HttpContentFunction);

  if (!ads.begin(ADS1X15_ADDRESS+2)) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  
  eeprom->ArmVersionNumber();  
  wlanConnector->enableWDT(30);
}

void loop() {
   wlanConnector->Process();    
    getValues();
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
    measurecurrent = voltsDelta*22.wm14;
    measurevoltage = volts1Volt*12.0;
    measurepower =measurevoltage*measurecurrent;

    Serial.print("VCur0="); Serial.print(volts0Cur); Serial.print("\t VRef2="); Serial.print(volts2Ref); Serial.print("\t VVolt1="); Serial.print(volts1Volt);
    Serial.print("\tV="); Serial.print(measurevoltage); Serial.print("\t A="); Serial.print(measurecurrent);
    Serial.print("\t P="); Serial.println(measurepower);
    
     measureMillis = millis();
   }
}


 String HttpContentFunction()
 {
      String jsonHtmlPage = "{\n"
"   \"solar\":{\n"
"    \"V0A\":\""+String(volts0Cur)+"\",\n"
"    \"V1V\":\""+String(volts1Volt)+"\",\n"
"    \"V1V\":\""+String(volts2Ref)+"\",\n"
"      \"current\":\""+String(measurecurrent)+"\",\n"
"      \"voltage\":\""+String(measurevoltage)+"\",\n"
"      \"power\":\""+String(measurepower)+"\",\n"
"   }\n"
"}";
  return jsonHtmlPage; 
 } 



