#ifndef EEPROMPROVIDER_h
#define EEPROMPROVIDER_h
#include <Arduino.h>
#include <EEPROM.h>

class EEPROMProvider
{

  #define _DEBUG_EEPROM

  #define maxEEPROM_SZIE 128
  #define maxregister 16
  #define MAGIG_LEN 3

  public:
    EEPROMProvider();
    EEPROMProvider(String version );

    bool IsInitialized();
    void ArmVersionNumber();
    uint8_t Register(uint8_t maxBufferLength, String defaultValue);
    void Save(uint8_t regID,String buffer);
    String Load(uint8_t regID);

  private:
    String curversion;
    int eepromSize;
    void WriteMagig();
    uint8_t GetRegLen(uint8_t regID);
    unsigned int GetRegOffset(uint8_t regID);
    uint8_t reg[maxregister];
};

#endif