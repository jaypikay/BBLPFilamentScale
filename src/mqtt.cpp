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

uint32_t reconnectCounter = 0;

long lastTimeStatusSent = 0;
long lastTimeStateSent = 0;

uint32_t readMifareTag();
void sendStatusUpdate();

int attemptConnectMqtt() {
  if (mqttClient.connected()) {
    return 1;
  }

  int status = mqttClient.connect(mqtt_host.c_str(), mqtt_port);
  if (!status) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
  }
  return status;
}

void setupMqtt() {
  debug_println("\033[1;32m*MQTT\033[0m: Initializing...");
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

    attemptConnectMqtt();
  }
}

void handleMqtt() {
  if (millis() - lastTimeStatusSent > 2 * 1000) {
    debug_println("\033[1;32m*MQTT\033[0m: Sending message '" + topicPrefix +
                  topic_status + "'...");
    mqttClient.beginMessage(topicPrefix + topic_status);
    mqttClient.print("ONLINE");
    mqttClient.endMessage();
    lastTimeStatusSent = millis();
  }
  if (millis() - lastTimeStateSent > 10 * 1000) {
    sendStatusUpdate();
    lastTimeStateSent = millis();
  }

  reconnectCounter++;
  if ((reconnectCounter % 300) == 0) {
    Serial.println("\033[1;32m*MQTT\033[0m: Attempting reconnect...");
    attemptConnectMqtt();
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
