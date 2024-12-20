#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "confstore.h"
#include "mqtt.h"
#include "nfc.h"
#include "scale.h"
#include "utils.h"
#include "wifi.h"

// Timing variables
unsigned long previousMqttMillis = 0;
unsigned long lastTaskMillis = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("");

  pinMode(TRIGGER_ENABLE_WEB_PORTAL, INPUT_PULLUP);

#ifdef ENABLE_DEBUG
  debug_println("*** DEBUG prints enabled ***");
#endif /* DEBUG */

  setupConfigFS();

  setupWiFi();
  setupMqtt();

  hasScale = setupScale();
  hasNFC = setupNFC();

  Serial.println("Setup completed. Entering main loop...");
}

void loop() {
  unsigned long currentMillis = millis();

  bool publishMqtt = true;
  bool scaleUpdated = false;
  bool nfcUpdated = false;

  // Prioritize OTA and WiFi handling
  handleOTA();
  handleWiFi();

  delay(100);

  // Handle scale and NFC in a cyclic manner
  if (currentMillis - lastTaskMillis >= TASKS_UPDATE_INTERVAL) {
    lastTaskMillis = currentMillis;

    nfcUpdated = handleNFC();
    scaleUpdated = handleScale();
  }

  if (nfcUpdated || scaleUpdated) {
    publishMqtt = true;
  }

  // Handle MQTT every 60 seconds or handle if requested
  // TODO: handle force update request
  if (currentMillis - previousMqttMillis >= MQTT_PUBLISH_INTERVAL) {
    previousMqttMillis = currentMillis;

    handleMqtt();
  }
}
