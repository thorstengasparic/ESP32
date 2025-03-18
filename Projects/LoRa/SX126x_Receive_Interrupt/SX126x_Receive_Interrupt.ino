/*   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>
#include <Wire.h>
#include "SSD1306Wire.h"  // legacy: #include "SSD1306.h"
#include <pins_arduino.h>

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);  // ADDRESS, SDA, SCL  -  SDA_OLED and SCL_OLED are the pins from pins_arduino.h where the Heltec connects the OLED display
SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);

void DisplayPrint(char* text);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  InitDisplay();
  InitRadio();

}

volatile bool receivedFlag = false;
ICACHE_RAM_ATTR
void setFlag(void) {
  receivedFlag = true;
}

void loop() {

  if (receivedFlag) {
    receivedFlag = false;

    String str;
    int state = radio.readData(str);
    if (state == RADIOLIB_ERR_NONE) {
      DisplayPrint((char*)str.c_str());

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      Serial.println(F("CRC error!"));
      DisplayPrint("CRC error!");
    } else {
      String err = "failed, code " + String(state);
      DisplayPrint((char*)err.c_str());
    }
  }
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

void InitDisplay()
{
  VextON();
  displayReset();
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
}

void InitRadio(void)
{
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
  // initialize SX1262 with default settings
   DisplayPrint("[SX1262] Initializing ... ");

  int state = radio.begin(868.1, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 18, 8, 1.6, false);
  if (state == RADIOLIB_ERR_NONE) {
    DisplayPrint("radio begin success");
  } else {
    String s= "failed, code: "+ String(state);
    DisplayPrint((char*)s.c_str());
    while (true);
  }
  radio.setCurrentLimit(60.0);
  radio.setDio2AsRfSwitch(true);
  radio.explicitHeader();
  radio.setCRC(2);
  radio.setPacketSentAction(setFlag);
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    DisplayPrint("radio start success");
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void DisplayPrint(char* text)
{
   display.clear();
   DisplayText(0,  text);
   String s;
   s = "RSSI:\t\t" + String(radio.getRSSI()) + " dBm";
   DisplayText(1,  (char*)s.c_str());
   
   s = "SNR:\t\t" + String(radio.getSNR());
   DisplayText(2,  (char*)s.c_str());
     
   s = "Frequency error:\t" + String(radio.getFrequencyError()) + " Hz";
   DisplayText(3,  (char*)s.c_str());      
  
  display.display();
}

void DisplayText(byte line, char* text)
{
    display.setFont(ArialMT_Plain_10);
    display.drawString( 0, line*10, text);  
    Serial.println(text);
}