#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "wifi.h"

AsyncWebServer server(80);
DNSServer dns;

AsyncWiFiManager wm(&server, &dns);

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