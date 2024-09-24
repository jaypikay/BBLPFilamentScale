#include <ArduinoJson.h>
#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "config.h"
#include "confstore.h"
#include "mqtt.h"
#include "nfc.h"
#include "scale.h"
#include "spoolman.h"
#include "utils.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

String clientID = "Spoolman-";
String topicPrefix = "spoolman/";
String topic_status = "status";
String topicSpoolState = "spool/state";

long lastTimeStatusSent = 0;
long lastTimeStateSent = 0;

uint32_t readMifareTag();
void sendStatusUpdate();

void setupMqtt() {
  debug_println("*MQTT: Initializing...");
  if (!mqtt_host.isEmpty()) {
    byte mac[6];
    WiFi.macAddress(mac);
    for (int i = 0; i < 3; i++) {
      clientID += String(mac[i], HEX);
    }
    mqttClient.setId(clientID);
    strncpy(spoolmanStatus.deviceId, clientID.c_str(), 16);

    if (!mqtt_username.isEmpty() && !mqtt_password.isEmpty()) {
      mqttClient.setUsernamePassword(mqtt_username, mqtt_password);
    }

    if (!mqttClient.connect(mqtt_host.c_str(), mqtt_port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
    }
  }
}

void handleMqtt() {
  if (millis() - lastTimeStatusSent > 2 * 1000) {
    debug_println("MQTT: Sending message '" + topicPrefix + topic_status +
                  "'...");
    mqttClient.beginMessage(topicPrefix + topic_status);
    mqttClient.print("ONLINE");
    mqttClient.endMessage();
    lastTimeStatusSent = millis();
  }
  if (millis() - lastTimeStateSent > 10 * 1000) {
    sendStatusUpdate();
    lastTimeStateSent = millis();
  }
}

void sendStatusUpdate() {
  if (mqttClient.connected()) {
    JsonDocument doc;

    doc["id"] = current_spool;
    doc["weight"] = current_weight;

    mqttClient.beginMessage(topicPrefix + topicSpoolState);
    serializeJson(doc, mqttClient);
    mqttClient.endMessage();
  }
}
