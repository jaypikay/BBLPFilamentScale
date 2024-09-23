#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "utils.h"
#include "confstore.h"
#include "spoolman.h"
#include "wifi.h"

WiFiManager wm;

int timeout = 120;
bool portalRunning = ENABLE_WEB_PORTAL;

WiFiManagerParameter custom_mqtt_host;
WiFiManagerParameter custom_mqtt_port;
WiFiManagerParameter custom_mqtt_username;
WiFiManagerParameter custom_mqtt_password;

void setupWiFi()
{
    wm.setHostname(HOSTNAME);

    new (&custom_mqtt_host) WiFiManagerParameter("mqtt_host", "MQTT Server Host", "", 40);
    new (&custom_mqtt_port) WiFiManagerParameter("mqtt_port", "MQTT Server Port", "1883", 6, "type=\"number\"");
    new (&custom_mqtt_username) WiFiManagerParameter("mqtt_username", "MQTT Username", "", 20);
    new (&custom_mqtt_password) WiFiManagerParameter("mqtt_password", "MQTT Password", "", 32, "type=\"password\"");
    wm.addParameter(&custom_mqtt_host);
    wm.addParameter(&custom_mqtt_port);
    wm.addParameter(&custom_mqtt_username);
    wm.addParameter(&custom_mqtt_password);

    wm.setSaveParamsCallback(saveConfigCallback);

    std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
    wm.setMenu(menu);
    wm.setClass("invert");

    wm.setConfigPortalTimeout(timeout);
    if (wm.autoConnect("Filament Scale AP"))
    {
        Serial.println("WiFi connection successful.");
        ArduinoOTA.begin();
        if (ENABLE_WEB_PORTAL)
        {
            debug_println("*WiFi: Starting web portal...");
            wm.startWebPortal();
        }
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

void handleWiFi()
{
    if (portalRunning)
    {
        wm.process();
    }

    if (digitalRead(TRIGGER_ENABLE_WEB_PORTAL) == LOW)
    {
        delay(50);
        if (digitalRead(TRIGGER_ENABLE_WEB_PORTAL) == LOW)
        {
            if (!portalRunning)
            {
                debug_println("*WiFi: Starting web portal...");
                wm.startWebPortal();
            }
            else
            {
                debug_println("*WiFi: Stopping web portal...");
                wm.stopWebPortal();
            }
            portalRunning = !portalRunning;
            spoolmanStatus.webPortal = portalRunning;

            /*delay(10000); // reset delay hold
            if (digitalRead(TRIGGER_ENABLE_WEB_PORTAL) == LOW)
            {
                Serial.println("Button Held");
                Serial.println("Erasing Config, restarting");
                wm.resetSettings();
                ESP.restart();
            }*/
        }
    }
}

void saveConfigCallback()
{
    if (!String(custom_mqtt_host.getValue()).isEmpty() && strcasecmp(custom_mqtt_host.getValue(), mqtt_host.c_str()) != 0)
    {
        debug_println("*CONF: mqtt_host has changed...");
        mqtt_host = String(custom_mqtt_host.getValue());
    }
    if (strcasecmp(custom_mqtt_port.getValue(), String(mqtt_port).c_str()) != 0)
    {
        debug_println("*CONF: mqtt_port has changed...");
        mqtt_port = (uint16_t)atoi(custom_mqtt_port.getValue());
    }
    if (!String(custom_mqtt_username.getValue()).isEmpty() && strcasecmp(custom_mqtt_username.getValue(), mqtt_username.c_str()) != 0)
    {
        debug_println("*CONF: mqtt_username has changed...");
        mqtt_username = String(custom_mqtt_username.getValue());
    }
    if (!String(custom_mqtt_password.getValue()).isEmpty() && strcasecmp(custom_mqtt_password.getValue(), mqtt_password.c_str()) != 0)
    {
        debug_println("*CONF: mqtt_password has changed...");
        mqtt_password = String(custom_mqtt_password.getValue());
    }

    saveConfig();
}
