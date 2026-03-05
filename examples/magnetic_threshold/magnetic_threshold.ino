/*!
 * @file magnetic_threshold.ino
 * @brief Z-axis magnetic threshold detection using hardware interrupt
 *
 * Sets a fixed Z-axis threshold (~10 mT) and uses the INT pin to
 * detect when a strong magnet is brought nearby.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 * MIT license
 */

#include <Adafruit_TMAG5273.h>

#define INT_PIN 2
#define THRESHOLD_MT 10.0 // Trigger at ~10 mT on Z axis

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

  // Determine range from variant (begin() sets wide range by default)
  bool is_x2 = (tmag.getDeviceID() & 0x03) == 0x02;
  float range_mT = is_x2 ? 266.0 : 80.0;
  Serial.print(F("Variant: "));
  Serial.print(is_x2 ? F("x2") : F("x1"));
  Serial.print(F(", Z range: +/-"));
  Serial.print(range_mT, 0);
  Serial.println(F(" mT"));

  // Convert mT threshold to register value: val = mT * 128 / range
  int8_t threshVal = (int8_t)(THRESHOLD_MT * 128.0 / range_mT);
  if (threshVal < 1)
    threshVal = 1;

  // Set X and Y thresholds to max so only Z triggers
  tmag.setXThreshold(127);
  tmag.setYThreshold(127);
  tmag.setZThreshold(threshVal);

  float actual_mT = threshVal * range_mT / 128.0;
  Serial.print(F("Z threshold: "));
  Serial.print(actual_mT, 1);
  Serial.print(F(" mT (register: "));
  Serial.print(threshVal);
  Serial.println(F(")"));

  // Disable data ready interrupt, enable threshold with pulsed mode
  tmag.enableResultInterrupt(false);
  tmag.enableThresholdInterrupt(true);
  tmag.setInterruptPulsed(true);

  // Set up D2 as interrupt input (INT pin is active low, open-drain)
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), onThreshold, FALLING);

  Serial.println(F("\nBring a magnet close to trigger!\n"));
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
