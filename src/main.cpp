#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "utils.h"
#include "wifi.h"
#include "nfc.h"
#include "scale.h"
#include "spoolman.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("");

    pinMode(TRIGGER_ENABLE_WEB_PORTAL, INPUT_PULLUP);

#ifdef DEBUG
    debug_println("*** DEBUG prints enabled ***");
#endif /* DEBUG */

    setupWiFi();

    has_scale = setupScale();
    has_nfc = setupNFC();

    Serial.println("Setup completed. Entering main loop...");
}

void loop()
{
    handleOTA();

    handleWiFi();

    handleScale();
    handleNFC();
}