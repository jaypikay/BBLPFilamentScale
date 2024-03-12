#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "utils.h"
#include "wifi.h"

WiFiManager wm;

int timeout = 120;
bool portalRunning = false;
int customFieldLength = 40;

WiFiManagerParameter custom_mqtt_server_addr;
WiFiManagerParameter custom_mqtt_server_port;
WiFiManagerParameter custom_mqtt_username;
WiFiManagerParameter custom_mqtt_password;

void setupWiFi()
{
    new (&custom_mqtt_server_addr) WiFiManagerParameter("mqtt_host", "MQTT Server Host", "", customFieldLength, "placeholder=\"MQTT Server Address\"");
    new (&custom_mqtt_server_port) WiFiManagerParameter("mqtt_port", "MQTT Server Port", "1883", 5, "placeholder=\"MQTT Server Address\" type=\"number\"");
    new (&custom_mqtt_username) WiFiManagerParameter("mqtt_username", "MQTT Username", "", customFieldLength, "placeholder=\"MQTT Server Address\"");
    new (&custom_mqtt_password) WiFiManagerParameter("mqtt_password", "MQTT Password", "", customFieldLength, "placeholder=\"MQTT Server Address\" type=\"password\"");

    wm.addParameter(&custom_mqtt_server_addr);
    wm.addParameter(&custom_mqtt_server_port);
    wm.addParameter(&custom_mqtt_username);
    wm.addParameter(&custom_mqtt_password);

    std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
    wm.setMenu(menu);
    wm.setClass("invert");

    wm.setConfigPortalTimeout(timeout);
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

            delay(10000); // reset delay hold
            if( digitalRead(TRIGGER_ENABLE_WEB_PORTAL) == LOW ){
                Serial.println("Button Held");
                Serial.println("Erasing Config, restarting");
                wm.resetSettings();
                ESP.restart();
            }

        }
    }
}

String getConfigParam(String name)
{
    String value;
    if (wm.server->hasArg(name))
    {
        value = wm.server->arg(name);
    }
    return value;
}