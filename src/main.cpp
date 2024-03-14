#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "utils.h"
#include "wifi.h"
#include "confstore.h"
#include "nfc.h"
#include "scale.h"
#include "mqtt.h"
#include "spoolman.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("");

    pinMode(TRIGGER_ENABLE_WEB_PORTAL, INPUT_PULLUP);

#ifdef ENABLE_DEBUG
    debug_println("*** DEBUG prints enabled ***");
#endif /* DEBUG */

    setupConfigFS();

    setupWiFi();
    setupMqtt();

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
    handleMqtt();
}