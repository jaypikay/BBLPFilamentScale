#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include "config.h"
#include "nfc.h"

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

bool setupNFC() {
  Serial.println("*PN532: Initializing...");
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata) {
    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware version: ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    //nfc.setPassiveActivationRetries(0xFF);
    //nfc.SAMConfig();
    return true;
  } else {
    Serial.println("PN53x card not found!");
   
    return false;
  }
}

bool readMfuPage(uint8_t page, uint8_t *data) {
  Serial.println("*PN532: Reading page content...");
  bool success = nfc.ntag2xx_ReadPage(page, data);
  if (success) {
    nfc.PrintHexChar(data, 4);
    return true;
  } else {
    Serial.println("*PN532: Error reading Mifare Ultralight TAG.");
  }

  return false;
}

uint32_t readMifareClassicTag() {
  uint32_t spoolId;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  uint8_t data[32] = { 0 };

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, PN532_READ_TIMEOUT)) {
    Serial.println("*PN532: Found an ISO14443A card");
    /*Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);*/

    if (uidLength >= 4 && uidLength <= 7) {
      switch (uidLength) {
      case 4:
        Serial.print("*PN532: Mifare Classic tag present.");
        break;
      case 7:
        Serial.print("*PN532: Mifare Ultralight tag present.");
        readMfuPage(MFU_PAGE_SPOOL_ID, data);
        spoolId = (static_cast<uint32_t>(data[0]) << 24)
          | (static_cast<uint32_t>(data[1]) << 16)
          | (static_cast<uint32_t>(data[2]) << 8)
          | (static_cast<uint32_t>(data[3]));
        Serial.print("Spool ID# ");
        Serial.println(spoolId);
        readMfuPage(MFU_PAGE_SPOOL_MAN_TAG, data);
        if (strncmp((char *)data, "SPOO", 4) == 0) {
          Serial.println("SpoolManager tag found.");
          return spoolId;
        }
      default:
        break;
      }
    }
    return INVALID_TAG;
  } else {
    return INVALID_TAG;
  }
}

void handleNFC() {
  readMifareClassicTag();
}