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
void printOut();

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
  //printOut();
  //delay(5);
}
void printOut()
{
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

   double x1 = analogRead(BattaryVoltagePin);
   x1 = (85.458+0.443*x1)/100;
   BattaryVoltage = BattaryVoltage * (1-fileStrength) + x1 * fileStrength;

   double x2 =  analogRead(LoadCurrentPin);
    x2 = (1577.909 - 0.838*x2)/100 - 0.27;
    LoadCurrent = LoadCurrent * (1-fileStrength) + x2 * fileStrength;

    double x3 =  analogRead(BattaryCurrentPin);
    x3 = (-1.0)*((1577.909 - 0.838*x3)/100 -0.45);
    BattaryCurrent = BattaryCurrent * (1-fileStrength) + x3 * fileStrength;

   double x4 =  analogRead(SolarCurrentPin);
   x4 = (-1.0)*((1577.909 - 0.838*x4)/100 -0.11);
   SolarCurrent = SolarCurrent * (1-fileStrength) + x4 * fileStrength;



}