/*!
 * @file magnetic_threshold.ino
 * @brief Demonstrates X-axis magnetic threshold detection with interrupt
 *
 * This sketch configures the TMAG5273 to trigger a threshold interrupt
 * when the X-axis magnetic field exceeds a set value. The threshold is
 * set to ~52 mT (threshold value 50 * 133/128 mT).
 */

#include <Adafruit_TMAG5273.h>

Adafruit_TMAG5273 tmag;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("\n=== TMAG5273 Magnetic Threshold Demo ===\n"));

  if (!tmag.begin()) {
    Serial.println(F("Failed to find TMAG5273!"));
    while (1)
      delay(10);
  }

  Serial.println(F("TMAG5273 found!"));

  // Configure for X channel readings
  tmag.setMagneticChannels(TMAG5273_MAG_CH_XYZ);
  tmag.setOperatingMode(TMAG5273_MODE_CONTINUOUS);
  tmag.setConversionAverage(TMAG5273_CONV_AVG_8X);

  // Set X-axis threshold to 50 (maps to ~52 mT: 50 * 133/128)
  // Threshold is absolute - triggers on +/- 52 mT
  int8_t xThreshold = 50;
  tmag.setXThreshold(xThreshold);
  Serial.print(F("X threshold set to: "));
  Serial.print(tmag.getXThreshold());
  Serial.println(F(" (approx 52 mT)"));

  // Enable interrupt via INT pin
  tmag.setInterruptMode(TMAG5273_INT_THROUGH_INT);
  Serial.println(F("Interrupt mode: INT pin"));

  // Enable threshold interrupt
  tmag.enableThresholdInterrupt(true);
  Serial.println(F("Threshold interrupt enabled"));

  Serial.println(F("\nBring a magnet close to trigger threshold!"));
  Serial.println(F("----------------------------------------------\n"));
}

void loop() {
  // Read magnetic field values
  float x = tmag.readMagneticX();
  float y = tmag.readMagneticY();
  float z = tmag.readMagneticZ();

  // Check conversion status for threshold flag
  uint8_t status = tmag.getConversionStatus();

  // Bit 2 (0x04) is threshold crossed flag
  bool thresholdCrossed = (status & 0x04) != 0;

  Serial.print(F("X: "));
  Serial.print(x, 1);
  Serial.print(F(" uT  Y: "));
  Serial.print(y, 1);
  Serial.print(F(" uT  Z: "));
  Serial.print(z, 1);
  Serial.print(F(" uT"));

  if (thresholdCrossed) {
    Serial.print(F("  *** THRESHOLD CROSSED ***"));
  }

  Serial.println();

  delay(100);
}
