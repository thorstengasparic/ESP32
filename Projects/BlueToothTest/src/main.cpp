#include <Arduino.h>
#include <Bluetooth.h>

// Bluetooth-Objekt erstellen
Bluetooth bluetooth;

void setup() {
  // Bluetooth-Modul initialisieren
  bluetooth.begin();
}

void loop() {
  // Eingehende Nachrichten lesen
  if (bluetooth.available()) {
    String message = bluetooth.readString();
    Serial.println("Empfangene Nachricht: " + message);
  }
}
