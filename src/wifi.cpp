#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "wifi.h"

WiFiManager wm;

void setupWiFi()
{
    wm.setConfigPortalTimeout(60);
    if (wm.autoConnect("Filament Scale AP"))
    {
        Serial.println("WiFi connection successful.");
        ArduinoOTA.begin();
    }
    else
    {
        Serial.println("Configuration Portal running...");
    }
}

void handleOTA()
{
    ArduinoOTA.handle();
}