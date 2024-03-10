#ifndef _CONFIG
#define _CONFIG 1

/* HX711 Amplifier for Load Cell*/
const uint8_t LOADCELL_DOUT_PIN = D6;
const uint8_t LOADCELL_SCK_PIN = D7;
const uint8_t LOADCELL_GAIN = 128U;

/* NFC Module */
const uint8_t PN532_IRQ = D2;
const uint8_t PN532_RESET = D3;

const uint16_t PN532_READ_TIMEOUT = 50U;

#endif /* _CONFIG */