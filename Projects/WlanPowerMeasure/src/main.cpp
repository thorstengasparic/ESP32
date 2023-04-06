#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
 //#include <EEPROM.h>
 //#include <EEPROMProvider.h>
 //#include <WlanConnetor.h>
 //#include <textreplacement.h>
 //#include <table.h>
#include <INA226.h>

#define VERSION "1.0"
#define SUS_LED LED_BUILTIN

struct Measurement
{
  bool isInit;
  bool deviceAvailable;
  double voltageExtern;
  double voltageIntern;
  double currentExtern;
  double voltageOffset;
  double dropVoltage;
  byte adress;
};

Measurement* adcChannel01;
Measurement* adcChannel02;
Measurement* inaChannel01;

void InitINA(void);
void INA226getValues(void );
String HttpContentFunction();
INA226 ina(0x40, &Wire);

void InitADS1X15(Adafruit_ADS1115 *adcChanel, Measurement *measureResult);
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

  adcChannel01->adress = ADS1X15_ADDRESS;
  adcChannel02->adress = ADS1X15_ADDRESSVCC;

  adcChannel01->isInit = false;
  adcChannel02->isInit  = false;
  inaChannel01->isInit  = false;

  adcChannel01->deviceAvailable = false;
  adcChannel02->deviceAvailable = false;
  inaChannel01->deviceAvailable = false;
}

void setup() {
  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");

  InitGlobals();

  //eeprom = new EEPROMProvider(VERSION);
 // wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, eeprom);

  //wlanConnector->SetCallback(HttpContentFunction);
 //InitINA();
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
  //ADS1X15getValues(adsVCC, adcChannel02);

  if ((currentMillis- measureMillisPrintline) > 500)
  {
    printvalues("ads01",  adcChannel01);
    printvalues("ads02",  adcChannel02);
    Serial.println("");
    measureMillisPrintline = currentMillis;
  }
  delay(10);
   //INA226getValues();
}

unsigned long measureMillisINA226 = 0;
void INA226getValues(void)
{
  unsigned long currentMillis = millis();
  if ((currentMillis- measureMillisINA226) > 1000)
  {
    Serial.println("\nBUS\tSHUNT\tCURRENT\tPOWER");
      Serial.print(ina.getBusVoltage(), 3);
    Serial.print("\t");
    Serial.print(ina.getShuntVoltage_mV(), 3);
    Serial.print("\t");
    Serial.print(ina.getCurrent_mA(), 3);
    Serial.print("\t");
    Serial.print(ina.getPower_mW(), 3);
    Serial.print("\t");
    Serial.print(ina.getShuntVoltage_mV()/0.00466, 3);
    Serial.println();

    delay(1000);
      measureMillisINA226 = currentMillis;
  }
}

void InitINA()
{
    Serial.println(__FILE__);

  Wire.begin();
  if (!ina.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
  Serial.println(ina.setMaxCurrentShunt(20.0, 0.00466));
  
}

void InitADS1X15(Adafruit_ADS1115 *adcChanel, Measurement *measureResult)
{
  if (measureResult->deviceAvailable) return;
  if (!adcChanel->begin(measureResult->adress)) 
  {
    Serial.print("Failed to initialize ADS on ");
    Serial.println(measureResult->adress);
  }
  measureResult->deviceAvailable = true;
}
void printvalues(String name, Measurement *measureResult)
{
  Serial.print(name);
  Serial.print("\t V=");
  Serial.print(measureResult->voltageExtern, 3);
  Serial.print("\t A=");
  Serial.print(measureResult->currentExtern, 3);
  Serial.print("\t Vd=");
  Serial.print(measureResult->dropVoltage, 3);
  Serial.print("\t Vi=");
  Serial.print(measureResult->voltageIntern, 3);
  Serial.print("\t Vo=");
  Serial.print(measureResult->voltageOffset, 3);
  Serial.println();
}
unsigned long measureMillis = 0;
void ADS1X15getValues(Adafruit_ADS1115 *adcChanel, Measurement *measureResult)
{
  double delta = 0.1;
  if(!measureResult->deviceAvailable)
  {
    InitADS1X15(adcChanel, measureResult);    
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
  double voltageIntern_new = adcChanel->computeVolts(adc1_new);
  double dropVoltage_new   = adcChanel->computeVolts(adc2_new);
}
catch(const std::exception& e)
{
  measureResult->deviceAvailable = false;
  Serial.print("exeption "); 
  Serial.println(e.what());
  return;
}

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
  double voltageIntern_new = adcChanel->computeVolts(adc1_new);
  double dropVoltage_new   = adcChanel->computeVolts(adc2_new);
  
  
  measureResult->voltageExtern = (measureResult->voltageExtern*(1.0-delta)) + (voltageExtern_new * delta);
  measureResult->voltageIntern = (measureResult->voltageIntern*(1.0-delta)) + (voltageIntern_new * delta);
  measureResult->dropVoltage = (measureResult->dropVoltage*(1.0-delta)) + (dropVoltage_new* delta);
  
  double refVoltage = measureResult->voltageIntern/2;
  double deltaVoltage = measureResult->dropVoltage-refVoltage;
  
  if (!measureResult->isInit)
  {
    measureResult->voltageOffset = deltaVoltage;
    //measureResult->voltageOffset = 0.011;
  }
  measureResult->currentExtern = (deltaVoltage-measureResult->voltageOffset)*15.0 ;
  
  if (!measureResult->isInit)
  {
    measureResult->isInit = true;
  }
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



