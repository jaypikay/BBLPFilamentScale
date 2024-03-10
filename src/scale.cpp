#include <HX711.h>

#include "config.h"
#include "scale.h"

HX711 loadcell;

bool setupScale() {
    Serial.println("*HX711: Initializing...");
    loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    if (loadcell.is_ready()) {
        Serial.println("*HX711: Load cell ready.");
        return true;
    } else {
        Serial.println("*HX711: Load cell not found...");
        return false;
    }
}

void handleScale() {
    
}