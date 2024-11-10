#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_ADS1X15.h"
 #include <EEPROM.h>
 #include <EEPROMProvider.h>
 #include <WlanConnetor.h>
 #include <textreplacement.h>
 #include <RestfullRespose.h>
 #include <table.h>
#include <INA226.h>

#include <mutex>

#define VERSION "1.0"
#define STATUS_LED LED_BUILTIN
#define diffvoltageDiode 0.8

String HttpContentFunction();
EEPROMProvider* eeprom = NULL;
WlanConnector *wlanConnector = NULL;
void getValues(void);
std::mutex value_mtx;
struct Measurement
{
  bool isInit;
  bool deviceAvailable;
  double voltageExtern;
  double voltageExternEff;
  double voltageIntern;
  double currentExtern;
  double voltageOffset;
  double dropVoltage;
  double voltageDiode;
  double voltageFactor;
  byte adress;
};

Measurement* adcChannel01;
Measurement* adcChannel02;
Measurement* inaChannel01;

bool InitINA(Measurement *values);
void INA226getValues(Measurement *values);
String HttpContentFunction();
INA226 ina(0x40, &Wire);

bool InitADS1X15(Adafruit_ADS1115 *adcChanel, Measurement *measureResult);
void ADS1X15getValues(Adafruit_ADS1115 *adcChanel, Measurement *measureResult);
Adafruit_ADS1115* adsVCC;
Adafruit_ADS1115* adsGND;     

#if defined(ESP8266)
#pragma message "ESP8266"
#define wifiModePin D6
#elif defined(ESP32)
#pragma message "ESP32"
#define wifiModePin 5
#else
#error "This ain't a ESP8266 or ESP32"
#endif

void printvalues(String name, Measurement *measureResult);
void InitGlobals(void)
{
  Wire.begin();

  adsVCC = new Adafruit_ADS1115();
  adsGND = new Adafruit_ADS1115();     
  adcChannel01 = new Measurement();
  adcChannel02 = new Measurement();
  inaChannel01 = new Measurement();

  adcChannel01->isInit = false;
  adcChannel02->isInit  = false;
  inaChannel01->isInit  = false;

  adcChannel01->deviceAvailable = false;
  adcChannel02->deviceAvailable = false;
  inaChannel01->deviceAvailable = false;

  adcChannel01->adress = ADS1X15_ADDRESS;
  adcChannel02->adress = ADS1X15_ADDRESS+1;

}
void Calibrate(void)
{
  adcChannel01->voltageDiode = diffvoltageDiode;
  adcChannel01->voltageOffset = 0.0;
  adcChannel01->voltageFactor = 5.5;

  adcChannel02->voltageDiode = diffvoltageDiode;
  adcChannel02->voltageOffset = -0.0134;
  adcChannel02->voltageFactor = -14.5;

  inaChannel01->voltageDiode = diffvoltageDiode;
  inaChannel01->voltageOffset = 0.0;
  inaChannel01->voltageFactor = 0.213;
  
}

void setup() {

  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");

  InitGlobals();
  Calibrate();
  
  InitINA(inaChannel01);
  InitADS1X15(adsGND, adcChannel01);
  InitADS1X15(adsVCC, adcChannel02);

  eeprom = new EEPROMProvider(VERSION);
  wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, eeprom);
  wlanConnector->SetCallback(HttpContentFunction);

  eeprom->ArmVersionNumber();  

  wlanConnector->enableWDT(30);
}

unsigned long measureMillisPrintline = 0;
void loop() 
{
  unsigned long currentMillis = millis();

  wlanConnector->Process();    

  if ((currentMillis- measureMillisPrintline) > 1000)
  {
    ADS1X15getValues(adsGND, adcChannel01);
    ADS1X15getValues(adsVCC, adcChannel02);
    INA226getValues(inaChannel01);  
    
    printvalues("Sol", adcChannel01);
    printvalues("Bat", adcChannel02);
    printvalues("Loa", inaChannel01);
    measureMillisPrintline = currentMillis;
  }
  delay(10);
}

void INA226getValues(Measurement *inaChannel)
{

  const std::lock_guard<std::mutex> lock(value_mtx);  
  if (!inaChannel->deviceAvailable)
  {
    if (!InitINA(inaChannel01))
    {
      inaChannel->voltageExternEff = -1.0;
      inaChannel->currentExtern = -1.0;
      return;
    } 
  }

  inaChannel->voltageExtern = ina.getBusVoltage();
  inaChannel->voltageExternEff = inaChannel->voltageExtern + inaChannel->voltageDiode;
  inaChannel->dropVoltage = ina.getShuntVoltage_mV();
  inaChannel->currentExtern = inaChannel->dropVoltage * inaChannel->voltageFactor;
}

bool InitINA(Measurement* inaChannel)
{
  inaChannel->deviceAvailable = ina.isConnected();
  if (!inaChannel->deviceAvailable)
  {
    inaChannel->deviceAvailable = ina.begin();
  }
  if (inaChannel->deviceAvailable)
  {
     Serial.println("INA connected");
     return true;
  }
  else
  {
    Serial.println("could not connect INA");
    return false;
  }
  return false;
}

bool InitADS1X15(Adafruit_ADS1115 *adcChanel, Measurement *measureResult)
{
  Serial.println(String("Adr: ") +String(measureResult->adress));
  if (measureResult->deviceAvailable) return true;
  
  measureResult->voltageExternEff = 0.0;
  measureResult->currentExtern = 0.0;
  measureResult->deviceAvailable = adcChanel->begin(measureResult->adress);
  if (!measureResult->deviceAvailable) 
  {
    Serial.println(String("could not connect ADS1X15: ") +String(measureResult->adress));
    return false;
  }
  else{
    Serial.println(String("ADS1X15 connected: ") +String(measureResult->adress));
    return true;
  }
}
void printvalues(String name, Measurement *measureResult)
{
  return;
  const std::lock_guard<std::mutex> lock(value_mtx);
  Serial.println("");
  Serial.print(name);
  Serial.print("\t V=");
  Serial.print(measureResult->voltageExternEff , 3);
  Serial.print("\t A=");
  Serial.print(measureResult->currentExtern, 3);
  Serial.print("\t Vd=");
  Serial.print(measureResult->dropVoltage, 3);
  Serial.print("\t Vi=");
  Serial.print(measureResult->voltageIntern, 3);
  Serial.print("\t Vo=");
  Serial.print(measureResult->voltageOffset, 3);
}

void ADS1X15getValues(Adafruit_ADS1115 *adcChanel, Measurement *measureResult)
{
  const std::lock_guard<std::mutex> lock(value_mtx);

  double delta = 1.0;
  if(!InitADS1X15(adcChanel, measureResult))
  {
    measureResult->voltageExternEff = -1.0;
    measureResult->currentExtern = -1.0;
    measureResult->deviceAvailable = false;
    return;
  }

  int16_t adc0_new;
  int16_t adc1_new;
  int16_t adc2_new;
  try
  {
    adc0_new = adcChanel->readADC_SingleEnded(0);
    adc1_new = adcChanel->readADC_SingleEnded(1);
    adc2_new = adcChanel->readADC_SingleEnded(2);
    if (adc0_new == 0 && adc1_new == 0 && adc2_new == 0)
    {
      measureResult->voltageExternEff = -2.0;
      measureResult->currentExtern = -2.0;
      measureResult->deviceAvailable = false;
      return;
    }
  }
  catch(const std::exception& e)
  {
    measureResult->deviceAvailable = false;
    measureResult->voltageExternEff = -3.0;
    measureResult->currentExtern = -3.0;
    Serial.print("exception "); 
    return;
  }

  double voltageExtern_new = adcChanel->computeVolts(adc0_new)*10.0;
  if (voltageExtern_new <= 0.0) voltageExtern_new = diffvoltageDiode * (-1.0);
  double voltageIntern_new = adcChanel->computeVolts(adc1_new);
  double dropVoltage_new   = adcChanel->computeVolts(adc2_new);
  
  
  measureResult->voltageExtern = (measureResult->voltageExtern*(1.0-delta)) + (voltageExtern_new * delta);
  measureResult->voltageIntern = (measureResult->voltageIntern*(1.0-delta)) + (voltageIntern_new * delta);
  measureResult->dropVoltage = (measureResult->dropVoltage*(1.0-delta)) + (dropVoltage_new* delta);
  
  double refVoltage = measureResult->voltageIntern/2;
  double deltaVoltage = measureResult->dropVoltage-refVoltage;
  measureResult->currentExtern = (deltaVoltage-measureResult->voltageOffset)*measureResult->voltageFactor ;
  
  if (!measureResult->isInit)
  {
    measureResult->isInit = true;
  }
  measureResult->voltageExternEff = measureResult->voltageExtern + measureResult->voltageDiode;
}

int count = 0;
String tmpstrJson = "";
String HttpContentFunction()
{
  const std::lock_guard<std::mutex> lock(value_mtx);

  tmpstrJson = jsonpage;//tableHtmlPage;
  
  tmpstrJson.replace(RPLC_SOLARCURRENT, String (adcChannel01->currentExtern*-1.0) );
  tmpstrJson.replace(RPLC_BATTCURRENT, String (adcChannel02->currentExtern*-1.0) );
  tmpstrJson.replace(RPLC_LOADCURRENT, String (inaChannel01->currentExtern*-1.0) );
  
  tmpstrJson.replace(RPLC_SOLARVOLTAGE, String (adcChannel01->voltageExternEff) );
  tmpstrJson.replace(RPLC_BATTVOLTAGE, String (adcChannel02->voltageExternEff) );
  tmpstrJson.replace(RPLC_LOADVOLTAGE, String (inaChannel01->voltageExternEff) );

  double SolarPower = adcChannel01->voltageExternEff * adcChannel01->currentExtern;  
  tmpstrJson.replace(RPLC_SOLARPOWER, String (SolarPower) );
  
  double BattaryPower = adcChannel02->voltageExternEff * adcChannel02->currentExtern;
  tmpstrJson.replace(RPLC_BATTPOWER, String (BattaryPower) );
  
  double LoadPower = inaChannel01->voltageExternEff * inaChannel01->currentExtern;
  tmpstrJson.replace(RPLC_LOADPOWER, String (LoadPower) );
  

  tmpstrJson.replace(RPLC_INFO, String (count++) );
  
 return tmpstrJson; 
}
