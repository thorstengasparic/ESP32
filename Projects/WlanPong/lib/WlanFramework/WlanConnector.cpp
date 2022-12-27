#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WlanConnetor.h>

#include <WlanConnetor.h>
#include <wifiCredHtml.h>
#include <esp_task_wdt.h>


WlanConnector::WlanConnector()
{
};

WlanConnector::WlanConnector(uint8_t accesPointPinIn, uint8_t statusLed, EEPROMProvider* eepromIn)
{
  Serial.println("1");
  //webserver = new WebServer(stdPort);
  Serial.println("2");
  contentCallback = NULL;

  for (int i = 0; i< maxOptions; i++) options[i] =0;

  statusLED = statusLed;
  pinMode(statusLED, OUTPUT);

  accesPointPin = accesPointPinIn;
  pinMode(accesPointPin, INPUT);

  IsInAccessPointMode = false;
  IsInWebserverMode = false;
  eeprom = eepromIn;
  lastMillis = 0;

  #define maxbufferlen 32
  if (eeprom != NULL)
  {
    passwordWlanID = eeprom->Register(maxbufferlen, defaultPWD);
    passwordWlan =   eeprom->Load(passwordWlanID);
    ssidWlanID = eeprom->Register(maxbufferlen, defaultssid );
    ssidWlan = eeprom->Load(ssidWlanID);
  }
  else
  {
    ssidWlan=  defaultssid;
    passwordWlan = defaultPWD;
  }
  Serial.println(passwordWlan);
  Serial.println(ssidWlan);
return;
  webserver->on("/", std::bind(&WlanConnector::handle_OnAccessWeb, this));
  webserver->on("/config", std::bind(&WlanConnector::handle_OnConfigWeb, this));
  webserver->on("/reload", std::bind(&WlanConnector::handle_OnAutoRefreshPage, this));
  webserver->on("/save", std::bind(&WlanConnector::handle_OnAccessSave, this));
  webserver->onNotFound(std::bind(&WlanConnector::handle_OnNotFound, this));
};

bool  WlanConnector::AccessPointMode()
{
   return (digitalRead(accesPointPin ) == HIGH);
}

void  WlanConnector::ResetWiFi()
{
   #ifdef DEBUG_WLAN
   Serial.println("Switch WIFI off");
   #endif
   IsInAccessPointMode = false;
   IsInWebserverMode = false;
   WiFi.disconnect(true);
   webserver->close();
}
void WlanConnector::enableWDT(uint32_t seconds)
{
    esp_task_wdt_init(seconds, true); //enable panic so ESP32 restarts
    esp_task_wdt_add(NULL); //add current thread to WDT watch

}
void WlanConnector::resetWDT()
{
  esp_task_wdt_reset();
}
void  WlanConnector::ProcessAccessPoint()
{
    this->resetWDT();
    if ( !IsInAccessPointMode )
    {
      ResetWiFi();
      Serial.println("Switch to accesspoint mode");
      
      WiFi.mode(WIFI_AP);
      WiFi.softAP( ssidAccessPoint, passwordAccessPoint);
      delay(100);

      local_ip.fromString(local_ipStr);
      gateway.fromString(gatewayStr);
      subnet.fromString(subnetStr);
    
      Serial.println(local_ip);Serial.println(gatewayStr);Serial.println(subnet);
      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);

      webserver->begin();      
      
      IsInAccessPointMode = true;
    }

    (digitalRead(statusLED) == HIGH) ? digitalWrite(statusLED, LOW) : digitalWrite(statusLED, HIGH);

    Serial.println("Accesspoint alive");
}

bool WlanConnector::WlanConnected(void)
{
  return ( !IsInAccessPointMode ) && (WiFi.status() == WL_CONNECTED);
}

void  WlanConnector::ProcessWebserver()
{
    if ( !IsInWebserverMode )
    {
      ResetWiFi();
      
      Serial.println("Switch to WebserverMode");
      WiFi.mode(WIFI_STA);
      IsInWebserverMode = true;
      digitalWrite(statusLED, LOW);
    }
    CheckReconnect();
    if (BlinkAlive&& (AliveModeBlink))
       (digitalRead(statusLED) == HIGH) ? digitalWrite(statusLED, LOW) : digitalWrite(statusLED, HIGH);  
    Serial.println("Web server alive");
}


void WlanConnector::CheckReconnect()
{
  if (WiFi.status() != WL_CONNECTED) 
  {
      webserver->close();
      WiFi.disconnect(true);

      Serial.println("Try connect to WiFi...");
      WiFi.begin( ssidWlan.c_str(), passwordWlan.c_str());
      while (WiFi.status() != WL_CONNECTED) 
      {
        Serial.print(".");
        digitalWrite(statusLED, LOW);
        delay(1000);
        digitalWrite(statusLED, HIGH);
        delay(10);
        digitalWrite(statusLED, LOW);
        if (AccessPointMode()) return;
      }
      Serial.println("");
      Serial.println("Connecting to WiFi...");
      Serial.println(WiFi.localIP());
      
      webserver->begin();

  }
};

void WlanConnector::SetCallback(TContentCallback callback)
{
    contentCallback = callback;
}

void WlanConnector::Process()
{
  webserver->handleClient();

  unsigned long currentMillis = millis();
  if ((currentMillis - lastMillis) > maxMillisTimer)
  {
    if (AccessPointMode())
    {
      ProcessAccessPoint();
    }
    else
    {
      ProcessWebserver();
    }
    lastMillis = currentMillis;
  }
}

void WlanConnector::handle_OnAccessPoint(void)
{
  webserver->send(200, "text/html", LoginHTMLWeb() );
};

bool WlanConnector::SetOption(String opt, String val)
{
      opt.replace("option","");
      int idx = atoi(opt.c_str());
      if (idx < 0) return false;
      if (idx >= maxOptions) return false;
      options[idx] = atoi(val.c_str());
      Serial.print(idx);
      Serial.print(": ");
      Serial.println(options[idx]);
      return true;
}

bool WlanConnector::SetAliveMode(String opt, String val)
{
      opt.replace("alive","");

      if (opt == "blink")
      {
        if (val == "true") {BlinkAlive = true; return true;}
        if (val == "false") {BlinkAlive = false; return true;}
      }

      if (opt == "mode")
      {
        if (val == "request") {AliveModeBlink = false; return true;}
        if (val == "blink") {AliveModeBlink = true; return true;}
      }
      return false;
}

void WlanConnector::handle_OnAutoRefreshPage(void)
{
  #ifdef DEBUG_WLAN
    Serial.println("handle_OnAutoRefreshPage");
  #endif
  this->resetWDT();
  String page = htmlValuePage;
  String content = getContent();
  page.replace(VALUETOKEN, content.c_str() );
  webserver->send(200, "text/html", page );
}

void  WlanConnector::handle_OnConfigWeb(void)
{
  #ifdef DEBUG_WLAN
    Serial.println("handle_OnConfigWeb");
  #endif
  this->resetWDT();

  int c = webserver->args();
  bool invalidCommand= false;
  for (int i = 0; i < c; i++)
  {
    if (webserver->argName(i).startsWith("option"))
    {
      if (SetOption(webserver->argName(i), webserver->arg(i)))
        continue;
    }
    if (webserver->argName(i).startsWith("alive"))
    {
      if (SetAliveMode(webserver->argName(i), webserver->arg(i)))
        continue;
    }
     invalidCommand= true;
  }
  if (invalidCommand)
  {
    webserver->send(400, "text/html", "Invalid command" );
  }
  else{
     webserver->send(200, "text/html", "OK" );
  }
};

void WlanConnector::handle_OnAccessSave(void)
{
  #ifdef DEBUG_WLAN
    Serial.println("handle_OnAccessSave");
  #endif
  this->resetWDT();

  int i = webserver->args();
  if (i ==2)
  {
      
      Serial.print(webserver->arg(0));
      Serial.print(" - ");
      Serial.println(webserver->arg(1));
      if (eeprom != NULL)
      {
        eeprom->Save(ssidWlanID, webserver->arg(0));
        eeprom->Save(passwordWlanID, webserver->arg(1));
        passwordWlan =   eeprom->Load(passwordWlanID);
        ssidWlan = eeprom->Load(ssidWlanID);
      }
     #ifdef DEBUG_WLAN
      Serial.println(ssidWlan);
      Serial.println(passwordWlan);
      #endif
  }
  webserver->send(200, "text/html", LoginHTMLWeb() );
};

void WlanConnector::handle_OnAccessWeb(void)
{
    #ifdef DEBUG_WLAN
    Serial.println("handle_OnAccessWeb");
  #endif
  this->resetWDT();
  
  if (BlinkAlive&& (!AliveModeBlink))
       (digitalRead(statusLED) == HIGH) ? digitalWrite(statusLED, LOW) : digitalWrite(statusLED, HIGH);
    if ( IsInWebserverMode )
      webserver->send(200, "text/html", DefaultHTMLWeb() );
    else
      webserver->send(200, "text/html", LoginHTMLWeb() );
};

void WlanConnector::handle_OnNotFound(void)
{
  Serial.println(webserver->uri());
  webserver->send(404, "text/plain", "Not found");
};

String  WlanConnector::getContent(void)
{
  if (contentCallback != NULL)
    {
      String content = (contentCallback)();
      #ifdef DEBUG_WLAN
      Serial.println(content);
      #endif
      return content;
    } 
    return ""; 
}

String  WlanConnector::DefaultHTMLWeb(void)
{
    String content = getContent();
    if (content != "")
    {
      return content;
    }

    String ptr = "<!DOCTYPE html> <html>\n";
    ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr +="<title>ESP32</title>\n";
    ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
    ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
    ptr +="</style>\n";
    ptr +="</head>\n";
    ptr +="<body>\n";
    ptr +="<div id=\"webpage\">\n";
    ptr +="<h1>Webserver</h1>\n";
    ptr +="</div>\n";
    ptr +="</body>\n";
    ptr +="</html>\n";
    return ptr;    
};
    
String  WlanConnector::LoginHTMLWeb(void)
{
      String page = htmlWifiPage;
      page.replace(SSIDTOKEN, ssidWlan);
      page.replace(PWDTOKEN , passwordWlan);
      return page;
}


