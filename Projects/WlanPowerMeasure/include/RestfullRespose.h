#ifndef jsonpage_h
#define jsonpage_h

#include <Arduino.h>
#include "textreplacement.h"


String jsonpage = 
"{"
    "\"info\": "
        "{\n"
        "\"count\": \""+RPLC_INFO+"\"\n"
        "},\n"
    "\"solar\": "
        "{\n"
        "\"current\": \""+RPLC_SOLARCURRENT+"\",\n"
        "\"voltage\": \""+RPLC_SOLARVOLTAGE+"\",\n"
        "\"load\": \""+RPLC_SOLARPOWER+"\"\n"
        "},\n"
    "\"batterie\": "
        "{\n"
        "\"current\": \""+RPLC_BATTCURRENT+"\",\n"
        "\"voltage\": \""+RPLC_BATTVOLTAGE+"\",\n"
        "\"load\": \""+RPLC_BATTPOWER+"\"\n"
        "},\n"
    "\"load\": "
        "{\n"
        "\"current\": \""+RPLC_LOADCURRENT+"\",\n"
        "\"voltage\": \""+RPLC_LOADVOLTAGE+"\",\n"
        "\"load\": \""+RPLC_LOADPOWER+"\"\n"
        "}\n"
"}";
#endif