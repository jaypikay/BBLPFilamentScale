#ifndef CONFIG_H
#define CONFIG_H 1

#define TRIGGER_ENABLE_WEB_PORTAL D5
#define ENABLE_WEB_PORTAL false

#define HOSTNAME "bbplss"

// Interval setup
#define MQTT_PUBLISH_INTERVAL 60000       // 60 seconds
#define MQTT_RECONNECT_INTERVAL 5 * 60000 // 5 minutes
#define TASKS_UPDATE_INTERVAL 1000        // 100 millis

/* HX711 Amplifier for Load Cell*/
const uint8_t LOADCELL_DOUT_PIN = D7;
const uint8_t LOADCELL_SCK_PIN = D8;

/*
 * Set LOADCELL_CALIBRATION_REFERENCE to a known weight (e.g. empty spool)
 * Use a scale to measure the weight of the known weight.
 */
#define LOADCELL_CALIBRATION_REFERENCE 253.0

#define LOADCELL_CALIBRATION_OFFSET 54857
#define LOADCELL_CALIBRATION_RATIO 1102.63
#define LOADCELL_READ_SAMPLES 15

/* NFC Module */
const uint8_t PN532_IRQ = D2;
const uint8_t PN532_RESET = D3;

const uint16_t PN532_READ_TIMEOUT = 50U;

/* EEPROM */
#define EEPROM_SIZE 512

#endif /* CONFIG_H */
