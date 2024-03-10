#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "nfc.h"
#include "scale.h"
#include "utils.h"
#include "wifi.h"

void setup() {
  Serial.begin(115200);
  Serial.println("");

#ifdef DEBUG
  debug_println("*** DEBUG prints enabled ***");
#endif /* DEBUG */

  setupWiFi();

  setupScale();
  setupNFC();

  Serial.println("Setup completed. Entering main loop...");
}

void loop() {
  handleOTA();
  handleNFC();
}