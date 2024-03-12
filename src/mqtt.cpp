#include <ArduinoMqttClient.h>
#include <WiFiClient.h>

#include "config.h"
#include "utils.h"
#include "confstore.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setupMqtt()
{
    debug_println("*MQTT: Initializing...");
    if (!mqtt_host.isEmpty())
    {
        if (!mqtt_username.isEmpty() && !mqtt_password.isEmpty())
        {
            mqttClient.setUsernamePassword(mqtt_username, mqtt_password);
        }

        if (!mqttClient.connect(mqtt_host.c_str(), mqtt_port))
        {
            Serial.print("MQTT connection failed! Error code = ");
            Serial.println(mqttClient.connectError());
        }
    }
}

void handleMqtt()
{
    mqttClient.poll();

    if (digitalRead(D5) == LOW)
    {

        debug_println("Sending MQTT message...");

        mqttClient.beginMessage("device/spoolman/hello");
        mqttClient.print("Hello World!");
        mqttClient.print("Hello");
        mqttClient.endMessage();
    }
}