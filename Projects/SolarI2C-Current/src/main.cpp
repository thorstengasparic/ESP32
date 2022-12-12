#include <Arduino.h>

#include <Wire.h>
#include "SDL_Arduino_INA3221.h"

#include <EEPROM.h>

#include <EEPROMProvider.h>
#include <WlanConnetor.h>
#include <HTTPClient.h>

#define VERSION "0.x"

#include <jsonhtmlpage.h>

#define STATUS_LED 2
#define wifiModePin 5
String HttpContentFunction();
void loopTest(void) ;

SDL_Arduino_INA3221 ina3221;
// //static const uint8_t SDA = 21;
// //static const uint8_t SCL = 22;
void GetSolarValues() ;
#define BATTERY_CHANNEL 1
#define LOAD_CHANNEL 2
#define SOLAR_CELL_CHANNEL 3
double BattaryCurrent=0;
double BattaryVoltage =0;
double LoadCurrent = 0;
double LoadVoltage = 0;
double SolarCurrent = 0;
double SolarVoltage = 0;

void printOut();

long timer =0;
long valueTimer =0;
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
  wlanConnector->enableWDT(30);
  valueTimer = millis();
  timer = millis();
  ina3221.begin();

   Serial.print("Manufactures ID=0x");
   int MID;
   MID = ina3221.getManufID();
   Serial.println(MID,HEX);
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
  tmpstr.replace(batterycurrent, String (BattaryCurrent) );
  tmpstr.replace(loadcurrent, String (LoadCurrent) );
  tmpstr.replace(solarcurrent, String (SolarCurrent) );
  
  tmpstr.replace(batteryvoltage, String (BattaryVoltage) );
  tmpstr.replace(loadvoltage, String (LoadVoltage) );
  tmpstr.replace(solarvoltage, String (SolarVoltage) );
  
 return tmpstr; 
 } 

void GetSolarValues() 
{
  
  if (millis() - valueTimer > maxtimer)
  {
      double busvoltage1 = ina3221.getBusVoltage_V(BATTERY_CHANNEL);
      double shuntvoltage1 = ina3221.getShuntVoltage_mV(BATTERY_CHANNEL);
      BattaryCurrent = -ina3221.getCurrent_mA(BATTERY_CHANNEL);
      BattaryVoltage = busvoltage1 + (shuntvoltage1 / 1000);

      double busvoltage2 = ina3221.getBusVoltage_V(LOAD_CHANNEL);
      double shuntvoltage2 = ina3221.getShuntVoltage_mV(LOAD_CHANNEL);
      LoadCurrent = ina3221.getCurrent_mA(LOAD_CHANNEL);
      LoadVoltage = busvoltage2 + (shuntvoltage2 / 1000);

      double busvoltage3 = ina3221.getBusVoltage_V(SOLAR_CELL_CHANNEL);
      double shuntvoltage3 = ina3221.getShuntVoltage_mV(SOLAR_CELL_CHANNEL);
      SolarCurrent = -ina3221.getCurrent_mA(SOLAR_CELL_CHANNEL);
      SolarVoltage = busvoltage3 + (shuntvoltage3 / 1000);


    //Serial.println(HttpContentFunction());
    //loopTest() ;
     valueTimer = millis();
  }
}


//   SDL_Arduino_INA3221 Library Test Code
//   SDL_Arduino_INA3221.cpp Arduino code - runs in continuous mode
//   Version 1.2
//   SwitchDoc Labs   September 2019


// This was designed for SunAirPlus - Solar Power Controller - www.switchdoc.com




// #include <Wire.h>
// #include "SDL_Arduino_INA3221.h"

// SDL_Arduino_INA3221 ina3221;


// //static const uint8_t SDA = 21;
// //static const uint8_t SCL = 22;

// // the three channels of the INA3221 named for SunAirPlus Solar Power Controller channels (www.switchdoc.com)
// #define LIPO_BATTERY_CHANNEL 1
// #define SOLAR_CELL_CHANNEL 2
// #define OUTPUT_CHANNEL 3

// void setup(void) 
// {
    
//   Serial.begin(115200);
//   Serial.println("SDA_Arduino_INA3221_Test");
  
//   Serial.println("Measuring voltage and current with ina3221 ...");
//   ina3221.begin();

//   Serial.print("Manufactures ID=0x");
//   int MID;
//   MID = ina3221.getManufID();
//   Serial.println(MID,HEX);
// }

void loopTest(void) 
{
  
  Serial.println("------------------------------");
  float shuntvoltage1 = 0;
  float busvoltage1 = 0;
  float current_mA1 = 0;
  float loadvoltage1 = 0;


  busvoltage1 = ina3221.getBusVoltage_V(BATTERY_CHANNEL);
  shuntvoltage1 = ina3221.getShuntVoltage_mV(BATTERY_CHANNEL);
  current_mA1 = -ina3221.getCurrent_mA(BATTERY_CHANNEL);  // minus is to get the "sense" right.   - means the battery is charging, + that it is discharging
  loadvoltage1 = busvoltage1 + (shuntvoltage1 / 1000);
  
  Serial.print("LIPO_Battery Bus Voltage:   "); Serial.print(busvoltage1); Serial.println(" V");
  Serial.print("LIPO_Battery Shunt Voltage: "); Serial.print(shuntvoltage1); Serial.println(" mV");
  Serial.print("LIPO_Battery Load Voltage:  "); Serial.print(loadvoltage1); Serial.println(" V");
  Serial.print("LIPO_Battery Current 1:       "); Serial.print(current_mA1); Serial.println(" mA");
  Serial.println("");

  float shuntvoltage2 = 0;
  float busvoltage2 = 0;
  float current_mA2 = 0;
  float loadvoltage2 = 0;

  busvoltage2 = ina3221.getBusVoltage_V(SOLAR_CELL_CHANNEL);
  shuntvoltage2 = ina3221.getShuntVoltage_mV(SOLAR_CELL_CHANNEL);
  current_mA2 = -ina3221.getCurrent_mA(SOLAR_CELL_CHANNEL);
  loadvoltage2 = busvoltage2 + (shuntvoltage2 / 1000);
  
  Serial.print("Solar Cell Bus Voltage 2:   "); Serial.print(busvoltage2); Serial.println(" V");
  Serial.print("Solar Cell Shunt Voltage 2: "); Serial.print(shuntvoltage2); Serial.println(" mV");
  Serial.print("Solar Cell Load Voltage 2:  "); Serial.print(loadvoltage2); Serial.println(" V");
  Serial.print("Solar Cell Current 2:       "); Serial.print(current_mA2); Serial.println(" mA");
  Serial.println("");

  float shuntvoltage3 = 0;
  float busvoltage3 = 0;
  float current_mA3 = 0;
  float loadvoltage3 = 0;

  busvoltage3 = ina3221.getBusVoltage_V(LOAD_CHANNEL);
  shuntvoltage3 = ina3221.getShuntVoltage_mV(LOAD_CHANNEL);
  current_mA3 = ina3221.getCurrent_mA(LOAD_CHANNEL);
  loadvoltage3 = busvoltage3 + (shuntvoltage3 / 1000);
  
  Serial.print("Output Bus Voltage 3:   "); Serial.print(busvoltage3); Serial.println(" V");
  Serial.print("Output Shunt Voltage 3: "); Serial.print(shuntvoltage3); Serial.println(" mV");
  Serial.print("Output Load Voltage 3:  "); Serial.print(loadvoltage3); Serial.println(" V");
  Serial.print("Output Current 3:       "); Serial.print(current_mA3); Serial.println(" mA");
  Serial.println("");

//  delay(2000);
}
