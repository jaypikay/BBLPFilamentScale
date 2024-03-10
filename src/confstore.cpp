#include <FS.h>
#include <ArduinoJson.h>

#include "confstore.h"

bool setupFS() {
    if (SPIFFS.begin()) {
        Serial.println("*FS: Filesystem mounted.");
        return true;
    } else {
        Serial.println("*FS: Failed to mount filesystem...");
        return false;
    }
}