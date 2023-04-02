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

INA226 ina(0x40, &Wire);

void InitINA(void);
void INA226getValues(void );
String HttpContentFunction();
//EEPROMProvider* eeprom = NULL;
//WlanConnector *wlanConnector = NULL;
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
void InitADS1X15(void );
void ADS1X15getValues(void );
void setup() {
  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");

  //eeprom = new EEPROMProvider(VERSION);
 // wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, eeprom);

  //wlanConnector->SetCallback(HttpContentFunction);
 //InitINA();
 InitADS1X15();
    
  //eeprom->ArmVersionNumber();  
  //wlanConnector->enableWDT(30);
}

void loop() {
   //wlanConnector->Process();    
    ADS1X15getValues();
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

void InitADS1X15()
{
if (!ads.begin(ADS1X15_ADDRESSVCC)) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}
double voltCurrent, refVoltage, voltageExtern, voltageIntern;
double measurecurrent, measurevoltage, measurepower;
unsigned long measureMillis = 0;
double correction = 0.0;
int16_t adc0=0;
int16_t adc1=0;
int16_t adc2=0;
int16_t adc3=0;
double iCur0;
void ADS1X15getValues(void)
{
  int16_t adc0_new = ads.readADC_SingleEnded(0);
  int16_t adc1_new = ads.readADC_SingleEnded(1);
  int16_t adc2_new = ads.readADC_SingleEnded(2);
  int16_t adc3_new = ads.readADC_SingleEnded(3);
  if(adc2 ==0)
  {
    adc0 = adc0_new;
    adc1 = adc1_new;
    adc2 = adc2_new;
    adc3 = adc3_new;
    voltageExtern = ads.computeVolts(adc0);
    voltageIntern = ads.computeVolts(adc1);
     voltCurrent = ads.computeVolts(adc2);
  }
  else
  {
    adc0 = adc0_new;
    adc1 = adc1_new;
    adc2 = adc2_new;
    adc3 = adc3_new;
  }
  double voltageExtern_new = ads.computeVolts(adc0);
  double voltageIntern_new = ads.computeVolts(adc1);
  double voltCurrent_new = ads.computeVolts(adc2);
  double delta = 0.01;
  voltageExtern = (voltageExtern*(1.0-delta)) + (voltageExtern_new * delta);
  voltageIntern = (voltageIntern*(1.0-delta)) + (voltageIntern_new * delta);
  voltCurrent = (voltCurrent*(1.0-delta)) + (voltCurrent_new* delta);
  
  refVoltage = voltageIntern/2;
    double deltaV = voltCurrent-refVoltage;
    if (correction==0.0)
    {
      correction = deltaV;
    }
    
    double iCur0 = (deltaV-correction)*15.0 ;
    
    
  unsigned long currentMillis = millis();

    
    
    
byte len = 5;
  Serial.print("Corr="); Serial.print(correction,len);Serial.print("\t");
    Serial.print("VCur="); Serial.print(voltCurrent,len);Serial.print("\t");
    Serial.print("VRef="); Serial.print(refVoltage,len);Serial.print("\t");
    Serial.print("VExt="); Serial.print(voltageExtern*10.0,len);Serial.print("\t");
    Serial.print("VInt="); Serial.print(voltageIntern,len);Serial.print("\t");
    Serial.print("delta="); Serial.print(deltaV,len);Serial.print("\t");
    Serial.print("iCur0="); Serial.print(iCur0,len);Serial.print("\t");


    /*
    double voltsDelta = volts0Cur -volts2Ref;
    measurecurrent = voltsDelta*22.14;
    measurevoltage = volts1Volt*12.0;
    measurepower =measurevoltage*measurecurrent;

    Serial.print("VCur0="); Serial.print(volts0Cur); Serial.print("\t VRef2="); Serial.print(volts2Ref); Serial.print("\t VVolt1="); Serial.print(volts1Volt);
    Serial.print("\tV="); Serial.print(measurevoltage); Serial.print("\t A="); Serial.print(measurecurrent);
    Serial.print("\t P="); Serial.println(measurepower);
    */
   Serial.println("");
   measureMillis = millis();
}


 String HttpContentFunction()
 {
      String jsonHtmlPage = "{\n"
"   \"solar\":{\n"
//"    \"V0A\":\""+String(volts0)+"\",\n"
//"    \"V1V\":\""+String(volts1)+"\",\n"
//"    \"V1V\":\""+String(volts2)+"\",\n"
"      \"current\":\""+String(measurecurrent)+"\",\n"
"      \"voltage\":\""+String(measurevoltage)+"\",\n"
"      \"power\":\""+String(measurepower)+"\",\n"
"   }\n"
"}";
  return jsonHtmlPage; 
 } 



