#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_ADS1X15.h"
 //#include <EEPROM.h>
 //#include <EEPROMProvider.h>
 //#include <WlanConnetor.h>
 //#include <textreplacement.h>
 //#include <table.h>
#include <INA226.h>

#define VERSION "1.0"
#define SUS_LED LED_BUILTIN
#define diffvoltageDiode 0.72
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

void InitINA(void);
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

  adcChannel01->voltageDiode = diffvoltageDiode;
  adcChannel02->voltageDiode = diffvoltageDiode;
  inaChannel01->voltageDiode  = diffvoltageDiode;

  adcChannel01->voltageOffset = -0.018;
  adcChannel02->voltageOffset = -0.012;
  inaChannel01->voltageOffset = 0.0;

  adcChannel01->voltageFactor = 5.5;
  adcChannel02->voltageFactor = -14.7;
  inaChannel01->voltageFactor = 0.213;
  
}

void setup() {
  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");

  InitGlobals();
  
  Wire.begin();

  //eeprom = new EEPROMProvider(VERSION);
 // wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, eeprom);

  //wlanConnector->SetCallback(HttpContentFunction);
  InitINA();
  InitADS1X15(adsGND, adcChannel01);
  InitADS1X15(adsVCC, adcChannel02);
  //eeprom->ArmVersionNumber();  
  //wlanConnector->enableWDT(30);
}

unsigned long measureMillisPrintline = 0;
void loop() {
  unsigned long currentMillis = millis();
   //wlanConnector->Process();    
  ADS1X15getValues(adsGND, adcChannel01);
  ADS1X15getValues(adsVCC, adcChannel02);
  INA226getValues(inaChannel01);

  if ((currentMillis- measureMillisPrintline) > 500)
  {
    printvalues("ads01",  adcChannel01);
    printvalues("ads02",  adcChannel02);
    printvalues("ina01",  inaChannel01);
    measureMillisPrintline = currentMillis;
  }
  delay(10);
   
}

unsigned long measureMillisINA226 = 0;
void INA226getValues(Measurement *inaChannel)
{
  unsigned long currentMillis = millis();
  if ((currentMillis- measureMillisINA226) > 1000)
  {
    inaChannel->voltageExtern = ina.getBusVoltage();
    inaChannel->voltageExternEff = inaChannel->voltageExtern + inaChannel->voltageDiode;
    inaChannel->dropVoltage = ina.getShuntVoltage_mV();
    inaChannel->currentExtern = inaChannel->dropVoltage * inaChannel->voltageFactor;
    
    measureMillisINA226 = currentMillis;
  }
}

void InitINA()
{
  Serial.println(__FILE__);
  if (!ina.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
  Serial.println(ina.setMaxCurrentShunt(20.0, 0.00466));
}

bool InitADS1X15(Adafruit_ADS1115 *adcChanel, Measurement *measureResult)
{
  if (measureResult->deviceAvailable) return true;
  
  measureResult->deviceAvailable = adcChanel->begin(measureResult->adress);
  if (!measureResult->deviceAvailable) 
  {
    Serial.print(".");
    return false;
  }
  return true;
}
void printvalues(String name, Measurement *measureResult)
{
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
unsigned long measureMillis = 0;
void ADS1X15getValues(Adafruit_ADS1115 *adcChanel, Measurement *measureResult)
{
  double delta = 0.1;
  if(!InitADS1X15(adcChanel, measureResult))
  {
    //Serial.println("!measureResult->deviceAvailable in get"); 
    return;
  }
  try
  {
    int16_t adc0_new = adcChanel->readADC_SingleEnded(0);
    int16_t adc1_new = adcChanel->readADC_SingleEnded(1);
    int16_t adc2_new = adcChanel->readADC_SingleEnded(2);
    
    if (!measureResult->isInit)
    {
      measureResult->voltageExtern = adcChanel->computeVolts(adc0_new)*10.0;
      measureResult->voltageIntern = adcChanel->computeVolts(adc1_new);
      measureResult->dropVoltage   = adcChanel->computeVolts(adc2_new);  
    }  
    double voltageExtern_new = adcChanel->computeVolts(adc0_new)*10.0;
    if (voltageExtern_new <= 0.0) 
      voltageExtern_new = diffvoltageDiode * (-1.0);
    double voltageIntern_new = adcChanel->computeVolts(adc1_new);
    double dropVoltage_new   = adcChanel->computeVolts(adc2_new);
  }
  catch(const std::exception& e)
  {
    measureResult->deviceAvailable = false;
    Serial.print("exception "); 
    return;
  }

  int16_t adc0_new = adcChanel->readADC_SingleEnded(0);
  int16_t adc1_new = adcChanel->readADC_SingleEnded(1);
  int16_t adc2_new = adcChanel->readADC_SingleEnded(2);

  if (!measureResult->isInit)
  {
    measureResult->voltageExtern = adcChanel->computeVolts(adc0_new)*10.0;
    if (measureResult->voltageExtern <= 0.0) 
      measureResult->voltageExtern = diffvoltageDiode * (-1.0);
    measureResult->voltageIntern = adcChanel->computeVolts(adc1_new);
    measureResult->dropVoltage   = adcChanel->computeVolts(adc2_new);  
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
  
  // if (!measureResult->isInit)
  // {
  //   measureResult->voltageOffset = deltaVoltage;
  // }
  measureResult->currentExtern = (deltaVoltage-measureResult->voltageOffset)*measureResult->voltageFactor ;
  
  if (!measureResult->isInit)
  {
    measureResult->isInit = true;
  }
  measureResult->voltageExternEff = measureResult->voltageExtern + measureResult->voltageDiode;
  measureMillis = millis();
}


 String HttpContentFunction()
 {
      String jsonHtmlPage = "{\n"
"   \"solar\":{\n"
//"    \"V0A\":\""+String(volts0)+"\",\n"
//"    \"V1V\":\""+String(volts1)+"\",\n"
//"    \"V1V\":\""+String(volts2)+"\",\n"
//"      \"current\":\""+String(measurecurrent)+"\",\n"
//"      \"voltage\":\""+String(measurevoltage)+"\",\n"
//"      \"power\":\""+String(measurepower)+"\",\n"
//"   }\n"
"}";
  return jsonHtmlPage; 
 } 



