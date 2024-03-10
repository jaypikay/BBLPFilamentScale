#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "wifi.h"
#include "nfc.h"
#include "scale.h"

void setup() {
  Serial.begin(115200);
  Serial.println("");

  setupWiFi();

  setupScale();
  setupNFC();

  Serial.println("Setup completed. Entering main loop...");
}

void loop() {
  handleOTA();
  handleNFC();
}