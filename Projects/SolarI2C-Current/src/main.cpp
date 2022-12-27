#include <Arduino.h>

 #include <Wire.h>
 #include <INA3221.h>
 #include <EEPROM.h>
 #include <EEPROMProvider.h>
 #include <WlanConnetor.h>
 #include <textreplacement.h>
 #include <jsonhtmlpage.h>
 #include <table.h>

#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>

Adafruit_BMP280 bmp; // I2C

Adafruit_AHTX0 aht;
SDL_Arduino_INA3221 ina3221;

#define VERSION "1.0"

#define STATUS_LED 2
#define wifiModePin 5
String HttpContentFunction();
void loopTest(void) ;

// //static const uint8_t SDA = 21;
// //static const uint8_t SCL = 22;
void GetValues() ;
void GetSolarValues();
void mesurePressure() ;
void measure_bmp() ;
#define BATTERY_CHANNEL 1
#define LOAD_CHANNEL 2
#define SOLAR_CELL_CHANNEL 3
double BattaryCurrent=0;
double BattaryVoltage =0;
double LoadCurrent = 0;
double LoadVoltage = 0;
double SolarCurrent = 0;
double SolarVoltage = 0;
double SolarPower = 0;
double BattaryPower = 0;
double LoadPower = 0;
double SumPower = 0;

void printOut();

long timer =0;
long valueTimer =0;
long maxtimer =1000;

EEPROMProvider* eeprom = NULL;
WlanConnector *wlanConnector = NULL;

//SHT75 onewire
//ESP32	Sensor
//3v3	Vcc
//Gnd	Gnd
//SDA	SDA
//SCL	SCL

//AHT10 Temperatur- und Luftfeuchtesensor
//Standard-I2C-Adresse: 0x38, kann auf 0x39 geändert werden
//3v3	Vcc
//Gnd	Gnd
//SDA	SDA
//SCL	SCL

//BME280 Luftdruck-, Luftfeuchtigkeits- und Temperatursensor
 //I²C Adresse lautet 0x77 / 76. 
//3v3	Vcc
//Gnd	Gnd
//SDA	SDA
//SCL	SCL

//https://forum.arduino.cc/t/esp32-with-st7796s-4-0inch-tft-spi/640696



void setup() 
{
  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");

  eeprom = new EEPROMProvider(VERSION);
  wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, eeprom);

  wlanConnector->SetCallback(HttpContentFunction);
/*
 ina3221.begin();
  Serial.print("Manufactures ID=0x");
   int MID;
   MID = ina3221.getManufID();
   Serial.println(MID,HEX);
*/

  int cnt = 10;
  if (! aht.begin()) 
  {
      Serial.println("Could not find AHTxy Check wiring");
      //while (cnt--) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  cnt = 10;
  if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
  //if (!bmp.begin()) {      
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                        "try a different address!"));
      while (cnt--) delay(10);
  }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X4,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
    
    eeprom->ArmVersionNumber();  
    wlanConnector->enableWDT(30);
    valueTimer = millis();
    timer = millis();
} 

void loop() 
{
  wlanConnector->Process();    
  GetValues() ;
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

//  String tmpstrJson = "";
//  String HttpContentFunctionJson()
//  {
//   tmpstrJson = jsonHtmlPage;
//   tmpstrJson.replace(batterycurrent, String (BattaryCurrent) );
//   tmpstrJson.replace(loadcurrent, String (LoadCurrent) );
//   tmpstrJson.replace(solarcurrent, String (SolarCurrent) );
  
//   tmpstrJson.replace(batteryvoltage, String (BattaryVoltage) );
//   tmpstrJson.replace(loadvoltage, String (LoadVoltage) );
//   tmpstrJson.replace(solarvoltage, String (SolarVoltage) );
 
//   return tmpstrJson; 
//  } 
 
 String tmpstrJson = "";
 String HttpContentFunction()
 {
  tmpstrJson = tableHtmlPage;
  tmpstrJson.replace(batterycurrent, String (BattaryCurrent) );
  tmpstrJson.replace(loadcurrent, String (LoadCurrent) );
  tmpstrJson.replace(solarcurrent, String (SolarCurrent) );
  
  tmpstrJson.replace(batteryvoltage, String (BattaryVoltage) );
  tmpstrJson.replace(loadvoltage, String (LoadVoltage) );
  tmpstrJson.replace(solarvoltage, String (SolarVoltage) );

  tmpstrJson.replace(batterypower, String (BattaryPower) );
  tmpstrJson.replace(loadpower, String (LoadPower) );
  tmpstrJson.replace(solarpower, String (SolarPower) );

  tmpstrJson.replace(sumpower, String (SumPower) );
  
 return tmpstrJson; 
 } 


void GetSolarValues() 
{
  
  
      double busvoltage1 = ina3221.getBusVoltage_V(BATTERY_CHANNEL);
      double shuntvoltage1 = ina3221.getShuntVoltage_mV(BATTERY_CHANNEL);
      BattaryCurrent = -ina3221.getCurrent_mA(BATTERY_CHANNEL);
      BattaryVoltage = busvoltage1 + (shuntvoltage1 / 1000);
      BattaryPower = BattaryCurrent*BattaryVoltage;;

      double busvoltage2 = ina3221.getBusVoltage_V(LOAD_CHANNEL);
      double shuntvoltage2 = ina3221.getShuntVoltage_mV(LOAD_CHANNEL);
      LoadCurrent = ina3221.getCurrent_mA(LOAD_CHANNEL);
      LoadVoltage = busvoltage2 + (shuntvoltage2 / 1000);
      LoadPower = LoadCurrent*LoadVoltage;

      double busvoltage3 = ina3221.getBusVoltage_V(SOLAR_CELL_CHANNEL);
      double shuntvoltage3 = ina3221.getShuntVoltage_mV(SOLAR_CELL_CHANNEL);
      SolarCurrent = -ina3221.getCurrent_mA(SOLAR_CELL_CHANNEL);
      SolarVoltage = busvoltage3 + (shuntvoltage3 / 1000);
      SolarPower = SolarCurrent*SolarVoltage;
      
      SumPower = -(SolarPower +BattaryPower);

    //Serial.println(HttpContentFunction());
    //loopTest() ;
  
}

void GetValues() 
{
  
  if (millis() - valueTimer > maxtimer)
  {
      //GetSolarValues();

      measure_bmp();
      mesurePressure() ;

     valueTimer = millis();
  }
}



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

}



void  mesurePressure() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

}

void measure_bmp() {
  // must call this to wake sensor up and get new measurement data
  // it blocks until measurement is complete
  if (bmp.takeForcedMeasurement()) {
    // can now print out the new measurements
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("seaLevelForAltitude = "));
    Serial.print(bmp.seaLevelForAltitude(190.0,bmp.readPressure())); 
    Serial.println(" Pa");

    Serial.println();
    
  } else {
    Serial.println("Forced measurement failed!");
  }

}

