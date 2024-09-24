#include <Adafruit_PN532.h>
#include <SPI.h>
#include <Wire.h>

#include "config.h"
#include "nfc.h"
#include "spoolman.h"
#include "utils.h"

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

bool has_nfc;
uint16_t current_spool;

bool setupNFC() {
  Serial.println("*PN532: Initializing...");
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata) {
    has_nfc = true;

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
    has_nfc = false;

    Serial.println("PN53x card not found!");

    return false;
  }
}

bool readMfuPage(uint8_t page, uint8_t *data) {
  Serial.println("*PN532: Reading page content...");
  bool success = nfc.ntag2xx_ReadPage(page, data);
  if (success) {
#ifdef DEBUG
    nfc.PrintHexChar(data, 4);
#endif /* DEBUG */
    return true;
  } else {
    Serial.println("*PN532: Error reading Mifare Ultralight TAG.");
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
    debug_println("*PN532: Found an ISO14443A card");

    if (uidLength >= 4 && uidLength <= 7) {
      switch (uidLength) {
      case 4:
        Serial.println("*PN532: Mifare Classic tag present.");
        break;
      case 7:
        Serial.println("*PN532: Mifare Ultralight tag present.");

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
          if (spoolId != 0) {
            current_spool = spoolId;
          }
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

void handleNFC() {
  uint32_t spoolId = readMifareTag();

  if (spoolId != INVALID_TAG) {
    // delay(500);
    // requestSpoolUpdate(spoolId);
  }
}
