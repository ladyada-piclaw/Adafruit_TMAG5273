/*!
 * @file magnetic_threshold.ino
 * @brief Z-axis magnetic threshold detection using hardware interrupt
 *
 * Reads ambient Z field, sets threshold 20% above it, and uses D2
 * interrupt pin to detect when a magnet crosses the threshold.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 * MIT license
 */

#include <Adafruit_TMAG5273.h>

#define INT_PIN 2

Adafruit_TMAG5273 tmag;
volatile bool thresholdTriggered = false;

void onThreshold() {
  thresholdTriggered = true;
}

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

  // Read ambient Z field (average 10 samples)
  Serial.println(F("Calibrating ambient Z field..."));
  float ambientZ = 0;
  for (int i = 0; i < 10; i++) {
    ambientZ += tmag.readMagneticZ();
    delay(20);
  }
  ambientZ /= 10.0;
  Serial.print(F("Ambient Z: "));
  Serial.print(ambientZ, 1);
  Serial.println(F(" uT"));

  // Convert ambient to threshold value and add 20% margin
  // Threshold register is int8_t: threshold_mT = value * range_mT / 128
  // For x2 variant normal range: range = 133 mT
  // For x1 variant normal range: range = 40 mT
  bool is_x2 = (tmag.getDeviceID() & 0x03) == 0x02;
  float range_mT = is_x2 ? 133.0 : 40.0;
  float ambient_mT = ambientZ / 1000.0;
  int8_t threshVal = (int8_t)(ambient_mT * 1.2 * 128.0 / range_mT);

  // Clamp to valid range
  if (threshVal > 127)
    threshVal = 127;
  if (threshVal < -128)
    threshVal = -128;

  tmag.setZThreshold(threshVal);
  float thresh_mT = threshVal * range_mT / 128.0;
  Serial.print(F("Z threshold set to: "));
  Serial.print(thresh_mT, 1);
  Serial.print(F(" mT (register value: "));
  Serial.print(threshVal);
  Serial.println(F(")"));

  // Disable data ready interrupt (enabled by begin()) so INT only fires
  // on threshold crossing, not every conversion
  tmag.enableResultInterrupt(false);

  // Enable threshold interrupt on INT pin
  tmag.enableThresholdInterrupt(true);
  Serial.println(F("Threshold interrupt enabled on INT pin"));

  // Set up D2 as interrupt input (INT pin is active low, open-drain)
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), onThreshold, FALLING);

  Serial.println(F("\nMove magnet closer to trigger threshold!\n"));
}

void loop() {
  float x = tmag.readMagneticX();
  float y = tmag.readMagneticY();
  float z = tmag.readMagneticZ();

  Serial.print(F("X: "));
  Serial.print(x, 1);
  Serial.print(F(" uT\tY: "));
  Serial.print(y, 1);
  Serial.print(F(" uT\tZ: "));
  Serial.print(z, 1);
  Serial.print(F(" uT"));

  if (thresholdTriggered) {
    Serial.print(F("\t*** THRESHOLD ***"));
    thresholdTriggered = false;
  }

  Serial.println();
  delay(100);
}
