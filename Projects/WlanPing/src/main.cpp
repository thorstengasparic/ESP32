 #include <Arduino.h>

#include <EEPROM.h>

#include <EEPROMProvider.h>
#include <WlanConnetor.h>
#include <HTTPClient.h>

#define VERSION "1.x"
#define STATUS_LED 2
#define beepPin     23
#define wifiModePin 5
#define echoPin     19
#define trigPin     18   
void httpRequest(String url);
String HttpContentFunction();
void ICACHE_RAM_ATTR ultrasonicEchoISR();
String url = "http://192.168.178.231";
bool doRequest = false;

WlanConnector *wlanConnector = NULL;

void setup() 
{
  Serial.begin(115200); // Starts the serial communication
  Serial.println("Init...");
  wlanConnector = new WlanConnector(wifiModePin, STATUS_LED, VERSION, true);
  wlanConnector->enableWDT(30);
} 

void loop() 
{
  wlanConnector->Process();
  if (wlanConnector->WlanConnected())
  {
    httpRequest(url);
  }
  
  delay(50);
}
HTTPClient http;

void httpRequest(String url)
{
  
  //http.setTimeout(1000);
  
  http.begin(url);
  Serial.println(url);
  digitalWrite(STATUS_LED, HIGH);
  int httpResponseCode = http.GET();
  digitalWrite(STATUS_LED, LOW);
  if (httpResponseCode > 0) {
    wlanConnector->resetWDT();
    Serial.print("HTTP ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println();
    //Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  
}

// #include <Arduino.h>
// #include <pitches.h>

// const int BUZZZER_PIN = D8; // GIOP18 pin connected to piezo buzzer

// // notes in the melody:
// int melody[] = {
//   NOTE_E5, NOTE_E5, NOTE_E5,
//   NOTE_E5, NOTE_E5, NOTE_E5,
//   NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
//   NOTE_E5,
//   NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
//   NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
//   NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
//   NOTE_D5, NOTE_G5
// };

// // note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
// int noteDurations[] = {
//   8, 8, 4,
//   8, 8, 4,
//   8, 8, 8, 8,
//   2,
//   8, 8, 8, 8,
//   8, 8, 8, 16, 16,
//   8, 8, 8, 8,
//   4, 4
// };

// void setup() {
//   // iterate over the notes of the melody:
//   int size = sizeof(noteDurations) / sizeof(int);

//   for (int thisNote = 0; thisNote < size; thisNote++) {

//     // to calculate the note duration, take one second divided by the note type.
//     //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//     int noteDuration = 1000 / noteDurations[thisNote];
//     tone(BUZZZER_PIN, melody[thisNote], noteDuration);

//     // to distinguish the notes, set a minimum time between them.
//     // the note's duration + 30% seems to work well:
//     int pauseBetweenNotes = noteDuration * 1.30;
//     delay(pauseBetweenNotes);
//     // stop the tone playing:
//     noTone(BUZZZER_PIN);
//   }
// }

// void loop() {
//   // no need to repeat the melody.
// }