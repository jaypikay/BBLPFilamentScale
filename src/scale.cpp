#include <EEPROM.h>
#include <HX711.h>

#include "config.h"
#include "scale.h"
#include "utils.h"

HX711 loadcell;

bool hasScale;
byte readSamples = LOADCELL_READ_SAMPLES;

long previousWeight = -1;
long currentWeight = 0;
long scaleOffset = LOADCELL_CALIBRATION_OFFSET;

void calibrateScale();

bool setupScale() {
  Serial.println("\033[1;34m*HX711\033[0m: Initializing...");
  loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  if (loadcell.wait_ready_timeout(500)) {
    Serial.println("\033[1;34m*HX711\033[0m: Load cell ready.");

    calibrateScale();

    // debug_print("\033[1;34m*HX711\033[0m: Load cell offset: ");
    // debug_println(LOADCELL_CALIBRATION_OFFSET);

    return true;
  } else {
    Serial.println("\033[1;34m*HX711\033[0m: Load cell not found...");
    return false;
  }
}

bool handleScale() {
  if (loadcell.wait_ready_timeout(500)) {
    loadcell.set_offset(scaleOffset);

    long reading = loadcell.read_average(readSamples);
    currentWeight = (reading - loadcell.get_offset()) / loadcell.get_scale();

    debug_print("\033[1;34m*HX711\033[0m: reading=");
    debug_print(reading);
    debug_print(" ::: [ CAL_OFFSET: ");
    debug_print(loadcell.get_offset());
    debug_print(", CAL_RATIO: ");
    debug_print(loadcell.get_scale());
    debug_print(", Adjusted Value: ");
    debug_print(currentWeight);
    debug_println(" ]");

    if (previousWeight != currentWeight) {
      previousWeight = currentWeight;
      return true;
    }
  }

  return false;
}

void calibrateScale() {
  Serial.println("\033[1;34m*HX711\033[0m: Calibrating...");

  loadcell.set_scale();
  loadcell.tare();

#ifdef MANUAL_CALIBRATION
  float reading = loadcell.get_units(read_samples);
  Serial.print("\033[1;34m*HX711\033[0m: Empty scale reading: ");
  Serial.println(reading, 2);

  Serial.println("!!! Place known weight on scale !!!");
  Serial.println("--- PRESS RETURN TO CONTINUE ---");
  Serial.flush();
  while (!Serial.available())
    ;

  float weight = loadcell.get_units(read_samples);
  Serial.print("\033[1;34m*HX711\033[0m: Known weight reading: ");
  Serial.println(weight, 2);

  Serial.print("\033[1;34m*HX711\033[0m: set_scale() value -> ");
  Serial.println(weight / LOADCELL_CALIBRATION_REFERENCE);

  loadcell.set_scale(weight / LOADCELL_CALIBRATION_REFERENCE);
#else
  loadcell.set_scale(LOADCELL_CALIBRATION_RATIO);
  loadcell.set_offset(scaleOffset);
#endif
}
