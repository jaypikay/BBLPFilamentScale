#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "utils.h"
#include "confstore.h"
#include "wifi.h"

WiFiManager wm;

int timeout = 120;
bool portalRunning = ENABLE_WEB_PORTAL;

struct URL
{
    String protocol = "";
    String host = "";
    String port = "";
    String path = "";
} url;

void parseURL(String urlString, struct URL *url);

WiFiManagerParameter custom_mqtt_host;
WiFiManagerParameter custom_mqtt_port;
WiFiManagerParameter custom_mqtt_username;
WiFiManagerParameter custom_mqtt_password;

WiFiManagerParameter custom_spoolman_url;
WiFiManagerParameter custom_spoolman_api_endpoint;

void setupWiFi()
{
    new (&custom_mqtt_host) WiFiManagerParameter("mqtt_host", "MQTT Server Host", "", 40);
    new (&custom_mqtt_port) WiFiManagerParameter("mqtt_port", "MQTT Server Port", "1883", 6, "type=\"number\"");
    new (&custom_mqtt_username) WiFiManagerParameter("mqtt_username", "MQTT Username", "", 20);
    new (&custom_mqtt_password) WiFiManagerParameter("mqtt_password", "MQTT Password", "", 32, "type=\"password\"");
    wm.addParameter(&custom_mqtt_host);
    wm.addParameter(&custom_mqtt_port);
    wm.addParameter(&custom_mqtt_username);
    wm.addParameter(&custom_mqtt_password);

    new (&custom_spoolman_url) WiFiManagerParameter("spoolman_host", "SpoolMan URL", "", 80);
    new (&custom_spoolman_api_endpoint) WiFiManagerParameter("spoolman_api_endpoint", "API endpoint", SPOOLMAN_API_ENDPOINT, 20);
    wm.addParameter(&custom_spoolman_url);
    wm.addParameter(&custom_spoolman_api_endpoint);

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
    if (strcasecmp(custom_mqtt_host.getValue(), mqtt_host.c_str()) != 0)
    {
        mqtt_host = String(custom_mqtt_host.getValue());
    }
    if (strcasecmp(custom_mqtt_port.getValue(), mqtt_host.c_str()) != 0)
    {
        mqtt_port = (uint16_t)atoi(custom_mqtt_port.getValue());
    }
    if (strcasecmp(custom_mqtt_username.getValue(), mqtt_username.c_str()) != 0)
    {
        mqtt_username = String(custom_mqtt_username.getValue());
    }
    if (strcasecmp(custom_mqtt_password.getValue(), mqtt_password.c_str()) != 0)
    {
        mqtt_password = String(custom_mqtt_password.getValue());
    }

    parseURL(String(custom_spoolman_url.getValue()), &url);

    saveConfig();
}

void parseURL(String urlString, struct URL *url)
{
    /* Source:
    https://gist.github.com/adoble/f1877b403e1c31584d96beab4553e4a7
    */
    // Assume a valid URL

    enum URLParseState
    {
        PROTOCOL,
        SEPERATOR,
        HOST,
        PORT,
        PATH
    } state = PROTOCOL;

    url->protocol = "";
    url->host = "";
    url->port = "";
    url->path = "/";

    for (unsigned int i = 0; i < urlString.length(); i++)
    {
        switch (state)
        {
        case PROTOCOL:
            if (urlString[i] == ':')
                state = SEPERATOR;
            else
                url->protocol += urlString[i];
            break;
        case SEPERATOR:
            if (urlString[i] != '/')
            {
                state = HOST;
                url->host += urlString[i];
            }
            break;
        case HOST:
            if (urlString[i] == ':')
                state = PORT;
            else
            {
                if (urlString[i] == '/')
                    state = PATH;
                else
                    url->host += urlString[i];
            }
            break;
        case PORT:
            if (urlString[i] == '/')
                state = PATH;
            else
                url->port += urlString[i];
            break;
        case PATH:
            url->path += urlString[i];
        }
    }
    url->protocol.toLowerCase();
    url->host.toLowerCase();
    url->port.toLowerCase();

    if (url->protocol.isEmpty())
    {
        url->protocol = "http";
        url->port = "80";
    }

    if (url->port.isEmpty())
    {
        if (url->protocol == "http")
        {
            url->port = "80";
        }
        else
        {
            url->port = "443";
        }
    }
}