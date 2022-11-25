#ifndef JSONHTMLPAGE_h
#define JSONHTMLPAGE_h

#include <Arduino.h>
const String batteryvoltage = "$batteryvoltage$";
const String solarcurrent = "$solarcurrent$";
const String batterycurrent = "$batterycurrent$";
const String loadycurrent = "$loadycurrent$";

String jsonHtmlPage = "{\n"
"   \"solar\":{\n"
"      \"batteryvoltage\":\"$batteryvoltage$\",\n"
"      \"solarcurrent\":\"$solarcurrent$\",\n"
"      \"batterycurrent\":\"$batterycurrent$\",\n"
"      \"loadycurrent\":\"$loadycurrent$\"\n"
"   }\n"
"}";

#endif