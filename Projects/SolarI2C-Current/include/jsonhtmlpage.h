#ifndef JSONHTMLPAGE_h
#define JSONHTMLPAGE_h

#include <Arduino.h>
const String batteryvoltage = "$batteryvoltage$";
const String solarvoltage = "$solarvoltage$";
const String loadvoltage = "$loadvoltage$";
const String solarcurrent = "$solarcurrent$";
const String batterycurrent = "$batterycurrent$";
const String loadcurrent = "$loadcurrent$";


String jsonHtmlPage = "{\n"
"   \"solar\":{\n"
"      \"batteryvoltage\":\""+batteryvoltage+"\",\n"
"      \"batterycurrent\":\""+batterycurrent+"\",\n"
"      \"solarvoltage\":\""+solarvoltage+"\",\n"
"      \"solarcurrent\":\""+solarcurrent+"\",\n"
"      \"loadvoltage\":\""+loadvoltage+"\",\n"
"      \"loadcurrent\":\""+loadcurrent+"\",\n"

"   }\n"
"}";

#endif