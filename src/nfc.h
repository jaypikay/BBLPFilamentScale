#ifndef _NFC
#define _NFC 1

#define INVALID_TAG 0xFFFFFFFF

extern bool has_nfc;
extern uint16_t current_spool;

const uint8_t MFU_PAGE_SPOOL_ID = 5;
const uint8_t MFU_PAGE_SPOOL_MAN_TAG = 6;

bool setupNFC();

void handleNFC();

#endif /* _NFC */
