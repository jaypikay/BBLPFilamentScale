#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "config.h"
#include "utils.h"
#include "confstore.h"

String mqtt_host;
uint16_t mqtt_port;
String mqtt_username;
String mqtt_password;

void setupConfigFS()
{
    if (!LittleFS.begin())
    {
        Serial.println("Failed to mount file system");
    }

    loadConfig();
}

bool loadConfig()
{
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println("Failed to open config file");
        return false;
    }

    StaticJsonDocument<200> doc;
    auto error = deserializeJson(doc, configFile);
    if (error)
    {
        Serial.println("Failed to parse config file");
        return false;
    }

    mqtt_host = String(doc["mqtt_host"]);
    mqtt_port = doc["mqtt_port"];
    mqtt_username = String(doc["mqtt_username"]);
    mqtt_password = String(doc["mqtt_password"]);
    
    return true;
}

bool saveConfig()
{
    StaticJsonDocument<200> doc;

    doc["mqtt_host"] = mqtt_host.c_str();
    doc["mqtt_port"] = mqtt_port;
    doc["mqtt_username"] = mqtt_username.c_str();
    doc["mqtt_password"] = mqtt_password.c_str();

    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    serializeJson(doc, configFile);
    return true;
}