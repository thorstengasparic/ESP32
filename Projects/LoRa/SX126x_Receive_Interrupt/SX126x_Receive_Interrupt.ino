/*   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <WiFi.h>

#include <RadioLib.h>
#include <Wire.h>
#include "SSD1306Wire.h"  // legacy: #include "SSD1306.h"
#include <pins_arduino.h>
#include <PubSubClient.h>


const char* ssid = "HP-675276";
const char* password = "SchatziEngele42";

// Add your MQTT Broker address, example:
const char* mqtt_server = "mqtt://192.168.178.201:31883";
// MQTT Broker
const char *mqtt_broker = "192.168.178.201";
const char *topic = "lora01";
const char *mqtt_username = "iot";
const char *mqtt_password = "1iot2";
const int mqtt_port = 31883;

char* outTopic = "lora/esp01/";
String tempTopic = String(outTopic) + "temperature";
String humTopic = String(outTopic) + "humidity";
bool isValidMessage(String msg);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

WiFiClient espClient;
PubSubClient client(espClient);

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);  // ADDRESS, SDA, SCL  -  SDA_OLED and SCL_OLED are the pins from pins_arduino.h where the Heltec connects the OLED display
SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);

void DisplayPrint(char* text);
void DisplayPrint(String text);

long validCount = 0;
long ErrorCount = 0;
long CRCErrorCount = 0;


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  InitDisplay();
  setup_wifi();
  InitRadio();

 client.setServer(mqtt_broker, mqtt_port);
client.setCallback(callback);

  if (client.connect(topic , mqtt_username, mqtt_password)) {
    //client.publish((const char*)tempTopic.c_str(),"0");
    //client.publish((const char*)humTopic.c_str(),"0");
  } else {
      DisplayPrint(String("failed, rc=")+String(client.state()));
      delay(5000);
    }
}
void setup_wifi() {
  delay(10);
  
  DisplayPrint(String(ssid) + ": Connecting *");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    DisplayPrint(String(ssid) + ": Connecting");
    delay(250);
    DisplayPrint(String(ssid) + ": Connecting *");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP().toString());
  DisplayPrint(String("IP: ") + String(WiFi.localIP().toString()));
  delay(3000);
}

volatile bool receivedFlag = false;
ICACHE_RAM_ATTR
void setFlag(void) {
  receivedFlag = true;
}

long lastmillis = millis();
void loop() {

  if ( (millis()-lastmillis) > (1000 *30))
  {
    ESP.restart();
  } 

  if (receivedFlag) {
    lastmillis = millis();
    receivedFlag = false;

    String str;
    int state = radio.readData(str);
    if (state == RADIOLIB_ERR_NONE) {
      validCount++;
      
      DisplayPrint(str);
      if (isValidMessage(str)) {
        double temp = getTemp(str);
        double hum = getHum(str);

        DisplayPrint(String(temp)+ "° "+String(hum)+"%" );
        client.publish((const char*)tempTopic.c_str(),(const char*)String(temp).c_str());
        client.publish((const char*)humTopic.c_str(),(const char*)String(hum).c_str());
      }
    }
    else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      CRCErrorCount++;
      Serial.println(F("CRC error!"));
      DisplayPrint("CRC error!");
    } else {
      ErrorCount++;
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

  int state = radio.begin(868.1, 125.0, 12, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 21, 8, 1.6, false);
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

void DisplayPrint(String msg)
{
   DisplayPrint((char*)msg.c_str());
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

   s = "Messages :" + String(validCount) +"/"+String(CRCErrorCount)+"/"+String(ErrorCount);
   DisplayText(4,  (char*)s.c_str());      
  
  display.display();
}

void DisplayText(byte line, char* text)
{
    display.setFont(ArialMT_Plain_10);
    display.drawString( 0, line*10, text);  
    Serial.println(text);
}

bool isValidMessage(String msg)
{
  //23.79 °C / 52.57 % re
  int br = msg.lastIndexOf('/');
  if (br< 5) return false;
  return true;
}

double getTemp(String msg)
{
  if (!isValidMessage(msg)) return 0.0;
  //23.79 °C / 52.57 % re
  int br = msg.lastIndexOf('/');
  String tempStr = msg.substring(0,br-1);
 return tempStr.toDouble();
}

double getHum(String msg)
{
  //23.79 °C / 52.57 % re
  if (!isValidMessage(msg)) return 0.0;

  int br = msg.lastIndexOf('/')+1;
  String tempStr = msg.substring(br,msg.length()-1);
  return tempStr.toDouble();
}

