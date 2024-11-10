/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
//  #include <Wire.h>
//  #include <SPI.h>
//  #include <Adafruit_Sensor.h>

// #include <Wire.h>
 
// void setup() {
//   Wire.begin();
//   Serial.begin(115200);
//   Serial.println("\nI2C Scanner");
// }
 
// void loop() {
//   byte error, address;
//   int nDevices;
//   Serial.println("Scanning...");
//   nDevices = 0;
//   for(address = 1; address < 127; address++ ) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
//     if (error == 0) {
//       Serial.print("I2C device found at address 0x");
//       if (address<16) {
//         Serial.print("0");
//       }
//       Serial.println(address,HEX);
//       nDevices++;
//     }
//     else if (error==4) {
//       Serial.print("Unknow error at address 0x");
//       if (address<16) {
//         Serial.print("0");
//       }
//       Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0) {
//     Serial.println("No I2C devices found\n");
//   }
//   else {
//     Serial.println("done\n");
//   }
//   delay(5000);          
// }

// /***************************************************************************
//   This is a library for the BMP280 humidity, temperature & pressure sensor

//   Designed specifically to work with the Adafruit BMP280 Breakout
//   ----> http://www.adafruit.com/products/2651

//   These sensors use I2C or SPI to communicate, 2 or 4 pins are required
//   to interface.

//   Adafruit invests time and resources providing this open source code,
//   please support Adafruit andopen-source hardware by purchasing products
//   from Adafruit!

//   Written by Limor Fried & Kevin Townsend for Adafruit Industries.
//   BSD license, all text above must be included in any redistribution
//  ***************************************************************************/

#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>


#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
Adafruit_AHTX0 aht;
void setup1() ;
void loop1() ;

void setup1() {
  Serial.begin(115200);  
  Serial.println(F("BMP280 Forced Mode Test."));

  //if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
                   setup1() ;
}

void loop1() {
  // must call this to wake sensor up and get new measurement data
  // it blocks until measurement is complete
  if (bmp.takeForcedMeasurement()) {
    // can now print out the new measurements
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1020.7)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
    delay(2000);
  } else {
    Serial.println("Forced measurement failed!");
  }
    loop1();

}




void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit AHT10/AHT20 demo!");

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  delay(500);
}