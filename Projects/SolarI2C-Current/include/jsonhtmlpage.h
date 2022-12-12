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
"      \"batterycurrent\":\""+batterycurrent+"\",\n"
"      \"loadcurrent\":\""+loadcurrent+"\",\n"
"      \"solarcurrent\":\""+solarcurrent+"\",\n"
"      \"batteryvoltage\":\""+batteryvoltage+"\",\n"
"      \"loadvoltage\":\""+loadvoltage+"\",\n"
"      \"solarvoltage\":\""+solarvoltage+"\",\n"
"   }\n"
"}";

#endif