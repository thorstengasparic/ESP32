#include <Arduino.h>

#include <EEPROM.h>

#include <EEPROMProvider.h>
#include <WlanConnetor.h>
#include <HTTPClient.h>

#define VERSION "0.x"

#include <jsonhtmlpage.h>
#define STATUS_LED 2
#define wifiModePin 5
String HttpContentFunction();


#define BattaryVoltagePin 34
#define LoadCurrentPin 35
#define BattaryCurrentPin 32
#define SolarCurrentPin 33

void GetSolarValues() ;

double BattaryCurrent=0;
double LoadCurrent = 0;
double SolarCurrent = 0;
double BattaryVoltage =0;

long timer =0;
long maxtimer =1000;

EEPROMProvider* eeprom = NULL;
WlanConnector *wlanConnector = NULL;

void setup() 
{
  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");

  eeprom = new EEPROMProvider(VERSION);
  wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, eeprom);

  wlanConnector->SetCallback(HttpContentFunction);

  eeprom->ArmVersionNumber();  
  //wlanConnector->enableWDT(30);
  timer = millis();
} 

void loop() 
{
  wlanConnector->Process();    
  GetSolarValues() ;
  if (millis() - timer > maxtimer)
  {
     Serial.print(BattaryVoltage);
     Serial.print("\t");
     Serial.print(SolarCurrent);
     Serial.print("\t");
     Serial.print(BattaryCurrent);
     Serial.print("\t");
     Serial.print(LoadCurrent);
     Serial.println("");
     timer = millis();
  }
  //delay(5);
}

String tmpstr = "";
 String HttpContentFunction()
 {
  tmpstr = jsonHtmlPage;
  tmpstr.replace(batteryvoltage, String (BattaryVoltage) );
  tmpstr.replace(solarcurrent, String (SolarCurrent) );
  tmpstr.replace(batterycurrent, String (BattaryCurrent) );
  tmpstr.replace(loadycurrent, String (LoadCurrent) );
 return tmpstr; 
 } 

void GetSolarValues() 
{
  
  double fileStrength = 0.05;

   uint16_t potValue = analogRead(BattaryVoltagePin);
   double x = potValue;
   x = (85.458+0.443*x)/100;
   BattaryVoltage = BattaryVoltage * (1-fileStrength) + x * fileStrength;

   
   uint16_t potCur = analogRead(LoadCurrentPin);
    x = potCur;
    x = (1577.909 - 0.838*x)/100 - 0.27;
    LoadCurrent = LoadCurrent * (1-fileStrength) + x * fileStrength;

    potCur = analogRead(BattaryCurrentPin);
    x = potCur;
    x = (1577.909 - 0.838*x)/100 -0.31;
    BattaryCurrent = BattaryCurrent * (1-fileStrength) + x * fileStrength;

   potCur = analogRead(SolarCurrentPin);
    x = potCur;
   x = (1577.909 - 0.838*x)/100;
   SolarCurrent = SolarCurrent * (1-fileStrength) + x * fileStrength;

}