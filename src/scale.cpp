#include <EEPROM.h>
#include <HX711.h>

#include "config.h"
#include "scale.h"
#include "utils.h"

HX711 loadcell;

bool has_scale;
byte read_samples = LOADCELL_READ_SAMPLES;

long current_weight = 0;
long scale_offset = LOADCELL_CALIBRATION_OFFSET;

void calibrateScale();

bool setupScale() {
  Serial.println("*HX711: Initializing...");
  loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  if (loadcell.wait_ready_timeout(500)) {
    Serial.println("*HX711: Load cell ready.");

    calibrateScale();

    // debug_print("*HX711: Load cell offset: ");
    // debug_println(LOADCELL_CALIBRATION_OFFSET);

    return true;
  } else {
    Serial.println("*HX711: Load cell not found...");
    return false;
  }
}

void handleScale() {
  if (loadcell.wait_ready_timeout(500)) {
    loadcell.set_offset(scale_offset);

    long reading = loadcell.read_average(read_samples);
    current_weight = (reading - loadcell.get_offset()) / loadcell.get_scale();

    debug_print("*HX711 reading: ");
    debug_print(reading);
    debug_print(" ::: [ CAL_OFFSET: ");
    debug_print(loadcell.get_offset());
    debug_print(", CAL_RATIO: ");
    debug_print(loadcell.get_scale());
    debug_print(", Adjusted Value: ");
    debug_print(current_weight);
    debug_println(" ]");

    delay(100);
  }
}

void calibrateScale() {
  Serial.println("*HX711: Calibrating...");

  loadcell.set_scale();
  loadcell.tare();

#ifdef MANUAL_CALIBRATION
  float reading = loadcell.get_units(read_samples);
  Serial.print("*HX711: Empty scale reading: ");
  Serial.println(reading, 2);

  Serial.println("!!! Place known weight on scale !!!");
  Serial.println("--- PRESS RETURN TO CONTINUE ---");
  Serial.flush();
  while (!Serial.available())
    ;

  float weight = loadcell.get_units(read_samples);
  Serial.print("*HX711: Known weight reading: ");
  Serial.println(weight, 2);

  Serial.print("*HX711: set_scale() value -> ");
  Serial.println(weight / LOADCELL_CALIBRATION_REFERENCE);

  loadcell.set_scale(weight / LOADCELL_CALIBRATION_REFERENCE);
#else
  loadcell.set_scale(LOADCELL_CALIBRATION_RATIO);
  loadcell.set_offset(scale_offset);
#endif
}
