#ifndef WLANCONNECTOR_h
#define WLANCONNECTOR_h
#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <WlanConnetor.h>

#include <EEPROMProvider.h>

class WlanConnector
{
  //******************************
  // Commands:
  // ==========
  //
  // uri/reload
  // uri/config?aliveblink=true|false
  // uri/config?alivemode=request|blink
  // uri/config?optionX=nnnnn
  // uri/save
  //
  //******************************

    #define DEBUG_WLAN

    #define defaultPWD "SchatziEngele42"
    #define defaultssid  "HP-675276"
    const char* ssidAccessPoint = "ESP-32";
    const char* passwordAccessPoint = NULL;
    const char* local_ipStr = "192.168.4.1";
    const char* gatewayStr = "192.168.4.1";
    const char* subnetStr = "255.255.255.0";

  public:                              
    typedef std::function<String(void)> TContentCallback;
    
    WlanConnector();
    WlanConnector( uint8_t accesPointPin, uint8_t statusLed, EEPROMProvider *eeprom);
    bool WlanConnected(void);
    
    void SetCallback(TContentCallback callback);
    void Process();
    bool BlinkAlive = true;

void enableWDT(uint32_t seconds);
    void resetWDT();

  private:
    TContentCallback contentCallback;
    int stdPort = 80;

    String DefaultHTML(void);
    typedef std::function<void(void)> THandlerFunction;
    
    #define maxOptions 10
    bool AliveModeBlink = false;

    int  options[maxOptions];
    bool SetOption(String opt, String val);
    bool SetAliveMode(String opt, String val);
    
    String  LoginHTMLWeb(void);
    String  DefaultHTMLWeb(void);
    void handle_OnAccessPoint(void);
    void handle_OnAccessSave(void);
    void handle_OnAccessWeb(void);
    void handle_OnNotFound(void);
    void handle_OnConfigWeb(void);
    void handle_OnAutoRefreshPage(void);
    
    String getContent(void);

    WebServer* webserver;    

    EEPROMProvider* eeprom;
    uint8_t ssidWlanID;
    uint8_t passwordWlanID;

    String ssidWlan;
    String  passwordWlan;
    
    volatile const unsigned long maxMillisTimer = 1000;
    volatile unsigned long lastMillis;
    
    uint8_t statusLED;
    uint8_t accesPointPin;
    volatile bool IsInAccessPointMode;
    volatile bool IsInWebserverMode;

    IPAddress local_ip;
    IPAddress gateway;
    IPAddress subnet;

    bool AccessPointMode();
    void  ResetWiFi();
    void CheckReconnect();

    void ProcessAccessPoint();
    void ProcessWebserver();
};

#endif