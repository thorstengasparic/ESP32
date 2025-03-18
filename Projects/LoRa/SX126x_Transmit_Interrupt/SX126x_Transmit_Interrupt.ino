/*
  https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>
#include <Wire.h>    
#include "SSD1306Wire.h"
#include <pins_arduino.h>

#include <Adafruit_AHT10.h>
Adafruit_AHT10 aht;

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);  
SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);
#define SEN_SDA 45
#define SEN_SCL 46

int transmissionState = RADIOLIB_ERR_NONE;
void setup() 
{
  
  Serial.begin(115200);
  while(!Serial){ delay(100); }
  InitDisplay();
  InitRadio();
  BlockingTransmit("Hello Message");
  
  InitSensor();
  
}
volatile bool transmittedFlag = false;
long count = 0;
void loop() 
{
  if(!transmittedFlag) return;
  transmittedFlag = false;
  
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  String str = String(temp.temperature)+" °C / " + String(humidity.relative_humidity)+ " % rel";
  transmit((char*)str.c_str());
}


ICACHE_RAM_ATTR
void setFlag(void) 
{
  transmittedFlag = true;
}

void transmit(char* msg)
{
  

  String sm = "-> " +String(msg);
  DisplayPrint(sm);
  if (transmissionState != RADIOLIB_ERR_NONE) 
  {
    String s = "failed, code " +String(transmissionState);
    DisplayPrint(s);
  }

  radio.finishTransmit();
  count++;
  delay(10000);
  transmissionState = radio.startTransmit(msg);
  
}

void VextON(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void)  //Vext default OFF
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

void displayReset(void) {
  // Send a reset
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
  digitalWrite(RST_OLED, LOW);
  delay(1);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
}

void DisplayPrint(String text)
{
  DisplayPrintOut((char*)text.c_str());
}

void DisplayPrint( char* text)
{
  DisplayPrintOut(text);
}

void DisplayPrintOut( char* text)
{
  display.clear();

  display.setFont(ArialMT_Plain_10);
  display.drawString( 0, 0*10, text);

   display.setFont(ArialMT_Plain_10);
  String s = "Loop: " + String(count);
  display.drawString( 0, 1*10, (char*)s.c_str());
  display.display();
  Serial.println(text);
}
void InitDisplay()
{
  VextON();
  displayReset();
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  DisplayPrint("Display init ok");
  
}

void InitRadio(void)
{
  // initialize SX1262 with default settings
   DisplayPrint("Init Radio...");

  int state = radio.begin(868.1, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 18, 8, 1.6, false);
  if (state == RADIOLIB_ERR_NONE) {
    DisplayPrint("Init Radio success.");
  } else {
    String s= "failed, code: "+ String(state);
    DisplayPrint(s);
    while (true);
  }
  radio.setCurrentLimit(60.0);
  radio.setDio2AsRfSwitch(true);
  radio.explicitHeader();
  radio.setCRC(2);
  
  radio.setPacketSentAction(setFlag);
  
}
void InitSensor()
{
  Wire1.begin(SEN_SCL, SEN_SDA);

if (! aht.begin(&Wire1)) {
    DisplayPrint("Could not find AHT10? Check wiring");
    transmit("Could not find AHT10? Check wiring");
    while (1) delay(10);
  }
  DisplayPrint("AHT10 found");
}
void BlockingTransmit(char* text){
  transmissionState = radio.startTransmit(text);
  while (!transmittedFlag) ;
  transmittedFlag = true;
  radio.finishTransmit();
  };
/*
Carrier frequency: 434.0 MHz
Bandwidth: 125.0 kHz (dual-sideband)
Spreading factor: 9
Coding rate: 4/7
Sync word: RADIOLIB_SX126X_SYNC_WORD_PRIVATE (0x12)
Output power: 10 dBm
Preamble length: 8 symbols
TCXO reference voltage: 1.6 V (SX126x module with TCXO)
LDO regulator mode: disabled (SX126x module with DC-DC power supply)
Other:
Over-current protection: 60.0 mA
DIO2 as RF switch control: enabled
LoRa header mode: explicit
LoRa CRC: enabled, 2 bytes
*/