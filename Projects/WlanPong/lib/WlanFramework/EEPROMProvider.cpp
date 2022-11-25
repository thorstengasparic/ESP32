#include <Arduino.h>
#include <EEPROM.h>
#include <EEPROMProvider.h>

EEPROMProvider::EEPROMProvider()
{
};

EEPROMProvider::EEPROMProvider(String version )
{
  curversion = version;
  eepromSize = 0;
  for (int i = 0; i< maxregister; i++)
  {
      reg[i] =0;
  }
  EEPROM.begin(maxEEPROM_SZIE);
};

void  EEPROMProvider::ArmVersionNumber()
{
  if (!IsInitialized()) 
  {
    #ifdef DEBUG_EEPROM
    Serial.println("ArmVersionNumber: WriteMagig");
    #endif
    WriteMagig();
  }
}

bool EEPROMProvider::IsInitialized()
{
    char buf[4];
    buf[0] = EEPROM.read(0);
    buf[1] = EEPROM.read(1);
    buf[2] = EEPROM.read(2);
    buf[3] = 0;
    #ifdef DEBUG_EEPROM
    Serial.print("Magig value: ");
    Serial.println(buf);
    #endif
    return curversion == buf;
    
};
void EEPROMProvider::WriteMagig()
{
    #ifdef DEBUG_EEPROM
    Serial.println("WriteMagig");
    #endif
    EEPROM.write(0, curversion[0]);
    EEPROM.commit();
    EEPROM.write(1, curversion[1]);
    EEPROM.commit();
    EEPROM.write(2, curversion[2]);
    EEPROM.commit();
};

uint8_t EEPROMProvider::Register(uint8_t maxBufferLength, String defaultbuffer)
{
  if (eepromSize != 0)
    return 0;

  #ifdef DEBUG_EEPROM
  Serial.println("Register");
  #endif
  for (int i = 1; i< maxregister; i++)
  {
      if(reg[i] ==0)
      {
        reg[i] = maxBufferLength;
        #ifdef DEBUG_EEPROM
        Serial.print("reg: ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(maxBufferLength);
        #endif

        if(!IsInitialized())
        {
          Save(i,defaultbuffer);
          Serial.print("Write default: ");
          Serial.println(defaultbuffer);
        }
        #ifdef DEBUG_EEPROM
        Serial.print("registered");
        #endif
        return i;
      }
  }
  return 0;
};
 

uint8_t EEPROMProvider::GetRegLen(uint8_t regID)
{
  #ifdef DEBUG_EEPROM
  Serial.print("GetRegLen: ");
  Serial.print(" ");
  Serial.print(regID);
  Serial.print(" ");
  Serial.println(reg[regID]);
  #endif
  return reg[regID];
};

unsigned int EEPROMProvider::GetRegOffset(uint8_t regID)
{
  #ifdef DEBUG_EEPROM
  Serial.print("GetRegOffset: ");
  Serial.print(regID);
  #endif
  int offset = MAGIG_LEN;
  for (int i = 1; i< regID; i++)
  {
     offset+=(unsigned int)reg[i];
  }
  #ifdef DEBUG_EEPROM
  Serial.print(" Offset: ");
  Serial.println(offset);
  #endif
  return offset;
};

void EEPROMProvider::Save(uint8_t regID, String buffer)
{
  #ifdef DEBUG_EEPROM
  Serial.print("Save ");
  Serial.print( regID);
  Serial.print(" ");
  Serial.println(buffer);
  
  Serial.print("reg[regID] ");
  Serial.println(reg[regID]);
  #endif
  uint8_t offset = GetRegOffset(regID);
  uint8_t len =  GetRegLen(regID);

  #ifdef DEBUG_EEPROM
  Serial.print("Off ");
  Serial.print( offset);
  Serial.print(" ");
  Serial.println(len);
  #endif

  for (int i =0; i< len; i++)
  {
    #ifdef DEBUG_EEPROM
    Serial.print("write: ");
    Serial.print(offset);
    Serial.print(" ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(buffer[i]);
    #endif
    EEPROM.write(offset+i, buffer[i]);
    EEPROM.commit();
  }
  #ifdef DEBUG_EEPROM
  Serial.print("Save eeprom: ");
  Serial.println(buffer);
  #endif
};

String EEPROMProvider::Load(uint8_t regID)
{
  uint8_t offset = GetRegOffset(regID);
  uint8_t len =  GetRegLen(regID);
  #ifdef DEBUG_EEPROM
  Serial.print("OffLoad " );
  Serial.print(regID);
  Serial.print(" ");
  Serial.print(offset);
  Serial.print(" ");
  Serial.println(len);
  #endif
  String buffer = "";
  for (int i =0; i< len; i++)
  {
    #ifdef DEBUG_EEPROM
    Serial.print("read: ");
    Serial.print(offset);
    Serial.print(" ");
    Serial.print(i);
    Serial.print(" ");
    #endif
    char val = (char)EEPROM.read(offset+i); 
    #ifdef DEBUG_EEPROM
    Serial.println(val);
    #endif
    if (val==0) break;
    buffer += (char)EEPROM.read(offset+i);
  }
  #ifdef DEBUG_EEPROM
  Serial.println("");
  Serial.print("Load eeprom: ");
  Serial.println(buffer);
  #endif
  return buffer;
};
