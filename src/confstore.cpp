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

String spoolman_host;
uint16_t spoolman_port;
String spoolman_protocol;
String spoolman_api_endpoint;

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

    configFile.close();

#ifdef ENABLE_DEBUG
    dumpConfigFile();
#endif

    /* MQTT Settings */
    mqtt_host = String(doc["mqtt_host"]);
    mqtt_port = doc["mqtt_port"];
    mqtt_username = String(doc["mqtt_username"]);
    mqtt_password = String(doc["mqtt_password"]);

    /* Spoolman Settings */
    spoolman_host = String(doc["spoolman_host"]);
    spoolman_port = doc["spoolman_port"];
    spoolman_protocol = String(doc["spoolman_protocol"]);
    spoolman_api_endpoint = String(doc["spoolman_api_endpoint"]);

    return true;
}

#ifdef ENABLE_DEBUG
void dumpConfigFile()
{
    debug_println("*CONF: Dumping config file content...");
    File configFile = LittleFS.open("/config.json", "r");
    if (configFile)
    {
        uint8_t buf[512] = {0};
        configFile.read(buf, 512);
        debug_println(String((char *)buf));
        configFile.close();
    }
    else
    {
        Serial.println("Failed to open config file");
    }
}
#endif

bool saveConfig()
{
    StaticJsonDocument<200> doc;

    doc["mqtt_host"] = mqtt_host.c_str();
    doc["mqtt_port"] = mqtt_port;
    doc["mqtt_username"] = mqtt_username.c_str();
    doc["mqtt_password"] = mqtt_password.c_str();

    doc["spoolman_host"] = spoolman_host.c_str();
    debug_println(spoolman_port);
    debug_println(String(doc["spoolman_port"]));
    doc["spoolman_port"] = spoolman_port;
    debug_println(String(doc["spoolman_port"]));
    doc["spoolman_protocol"] = spoolman_protocol.c_str();
    doc["spoolman_api_endpoint"] = spoolman_api_endpoint.c_str();

    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    serializeJson(doc, configFile);

    configFile.close();

#ifdef ENABLE_DEBUG
    dumpConfigFile();
#endif

    return true;
}