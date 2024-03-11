#include <ArduinoJson.h>
#include <FS.h>

#include "confstore.h"

bool setupFS()
{
    if (SPIFFS.begin())
    {
        Serial.println("*FS: Filesystem mounted.");
        return true;
    }
    else
    {
        Serial.println("*FS: Failed to mount filesystem...");
        return false;
    }
}

void loadSettings() {
}