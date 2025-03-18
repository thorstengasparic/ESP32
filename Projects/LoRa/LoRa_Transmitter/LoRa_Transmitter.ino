
#define LORA_RX 44
#define LORA_TX 43

#define LORA_VEXT 36
#define LORA_LED 35
#define LORA_RST_OLED 21
#define LORA_SCL 42
#define LORA_SDA 41


// #include "BH1750.h"
// #include "Wire.h"

// BH1750 bh1750_b;

// void setup() {
//   Serial.begin(115200);
//   //Wire1.begin(10,11);
//   Wire.begin(20,19);
//   bh1750_b.begin(BH1750::CONTINUOUS_HIGH_RES_MODE,0x23, &Wire);
//   Serial.println("go");
// }

// void loop() {

//   float light_level_b;
//   if (bh1750_b.measurementReady()) {
//     light_level_b = bh1750_b.readLightLevel();
//   }
//   I2C_ScannerWire(Wire);
//   //I2C_ScannerWire(Wire1);
//   Serial.printf(" %.0f \n", light_level_b);
//   delay(1000);
// }

// void I2C_ScannerWire(TwoWire &tw)
// {
//   byte error, address;
//   int nDevices;

//   nDevices = 0;
//   for(address = 1; address < 127; address++ )
//   {
//     tw.beginTransmission(address);
//     error = tw.endTransmission();

//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address<16)
//         Serial.print("0");
//       Serial.print(address,HEX);
//       Serial.println("  !");

//       nDevices++;
//     }
//     else if (error==4)
//     {
//       //Serial.print("Unknown error at address 0x");
//       //if (address<16)
//      //   Serial.print("0");
//      // Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0)
//     Serial.println("No I2C devices found\n");
// }
#include <SPI.h>
#include <LoRa.h>

#define ss 18
#define rst 14
#define dio0 26

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(8 /* CS */, 12 /* RST */, 14 /* DIO0 */);
  
  if (!LoRa.begin(868E6)) {
    Serial.println("Start of LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);
  // send packet
  LoRa.beginPacket();
  LoRa.print("Nr of Tx: ");
  LoRa.print(counter);
  LoRa.endPacket();
  counter++;
  delay(5000);
}
