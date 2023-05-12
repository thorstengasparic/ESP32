#ifndef jsonpage_h
#define jsonpage_h

#include <Arduino.h>
#include "textreplacement.h"


String jsonpage = "\"controller\": {"
"\"count\": \""+RPLC_INFO+"\""
"},\n"
"\"unit\": [\n"
"{\n"
"\"name\": \"Solar\",\n"
"\"current\": \""+RPLC_SOLARCURRENT+"\",\n"
"\"voltage\": \""+RPLC_SOLARVOLTAGE+"\",\n"
"\"load\": \""+RPLC_SOLARPOWER+"\"\n"
"},\n"
"{\n"
"\"name\": \"Battery\",\n"
"\"current\": \""+RPLC_BATTCURRENT+"\",\n"
"\"voltage\": \""+RPLC_BATTVOLTAGE+"\",\n"
"\"load\": \""+RPLC_BATTPOWER+"\"\n"
"},\n"
"{\n"
"\"name\": \"Load\",\n"
"\"current\": \""+RPLC_LOADCURRENT+"\",\n"
"\"voltage\": \""+RPLC_LOADVOLTAGE+"\",\n"
"\"load\": \""+RPLC_LOADPOWER+"\"\n"
"}\n"
"]\n"
"}";
#endif