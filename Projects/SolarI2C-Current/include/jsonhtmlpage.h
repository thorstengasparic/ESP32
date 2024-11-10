#ifndef JSONHTMLPAGE_h
#define JSONHTMLPAGE_h

#include <Arduino.h>
#include "textreplacement.h"

String jsonHtmlPage = "{\n"
"   \"solar\":{\n"
"      \"batterycurrent\":\""+batterycurrent+"\",\n"
"      \"loadcurrent\":\""+loadcurrent+"\",\n"
"      \"solarcurrent\":\""+solarcurrent+"\",\n"
"      \"batteryvoltage\":\""+batteryvoltage+"\",\n"
"      \"loadvoltage\":\""+loadvoltage+"\",\n"
"      \"solarvoltage\":\""+solarvoltage+"\",\n"
"      \"batterypower\":\""+batterypower+"\",\n"
"      \"loadvpower\":\""+loadpower+"\",\n"
"      \"solarpower\":\""+solarpower+"\"\n"
"   }\n"
"}";

#endif