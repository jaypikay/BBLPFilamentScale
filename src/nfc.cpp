#include <Adafruit_PN532.h>
#include <SPI.h>
#include <Wire.h>

#include "config.h"
#include "nfc.h"
#include "spoolman.h"
#include "utils.h"

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

bool hasNFC;

uint16_t previousSpool = 0;
uint16_t currentSpool;

bool setupNFC() {
  Serial.println("\033[1;33m*PN532\033[0m: Initializing...");
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata) {
    hasNFC = true;

    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware version: ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    // nfc.setPassiveActivationRetries(0xFF);
    // nfc.SAMConfig();
    return true;
  } else {
    hasNFC = false;

    Serial.println("PN53x card not found!");

    return false;
  }
}

bool readMfuPage(uint8_t page, uint8_t *data) {
  Serial.println("\033[1;33m*PN532\033[0m: Reading page content...");
  bool success = nfc.ntag2xx_ReadPage(page, data);
  if (success) {
#ifdef DEBUG
    nfc.PrintHexChar(data, 4);
#endif /* DEBUG */
    return true;
  } else {
    Serial.println(
        "\033[1;33m*PN532\033[0m: Error reading Mifare Ultralight TAG.");
  }

  return false;
}

uint32_t readMifareTag() {
  uint32_t spoolId;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;
  uint8_t data[32] = {0};

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength,
                              PN532_READ_TIMEOUT)) {
    debug_println("\033[1;33m*PN532\033[0m: Found an ISO14443A card");

    if (uidLength >= 4 && uidLength <= 7) {
      switch (uidLength) {
      case 4:
        Serial.println("\033[1;33m*PN532\033[0m: Mifare Classic tag present.");
        break;
      case 7:
        Serial.println(
            "\033[1;33m*PN532\033[0m: Mifare Ultralight tag present.");

        if (!readMfuPage(MFU_PAGE_SPOOL_ID, data))
          goto error;
        spoolId = (static_cast<uint32_t>(data[0]) << 24) |
                  (static_cast<uint32_t>(data[1]) << 16) |
                  (static_cast<uint32_t>(data[2]) << 8) |
                  (static_cast<uint32_t>(data[3]));
        debug_print("Spool ID# ");
        debug_println(spoolId);

        if (!readMfuPage(MFU_PAGE_SPOOL_MAN_TAG, data))
          goto error;
        if (strncmp((char *)data, "SPOO", 4) == 0) {
          debug_println("SpoolManager tag found.");
          return spoolId;
        }
      default:
        break;
      }
    }
  }

error:
  return INVALID_TAG;
}

bool handleNFC() {
  uint32_t spoolId = readMifareTag();
  if (spoolId != 0) {
    currentSpool = spoolId;
  }

  if (previousSpool != currentSpool) {
    debug_println(
        "\033[1;33m*PN532\033[0m: new tag detected, requesting mqtt update.");
    previousSpool = currentSpool;
    return true;
  }

  if (spoolId != INVALID_TAG) {
    // delay(500);
    // requestSpoolUpdate(spoolId);
  }

  return false;
}
