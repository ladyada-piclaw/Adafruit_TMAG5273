/*!
 * 06_range_switching.ino - Range Switching Test for TMAG5273
 *
 * Note: Place a magnet near the sensor for best results
 *
 * Verifies:
 * - Same field reads different RAW values at different ranges
 * - Wide range raw value is smaller (less sensitivity)
 * - Ratio between 1.3 and 3.0
 *
 * Hardware: Metro Mini, TMAG5273A2 at 0x35
 */

#include <Adafruit_TMAG5273.h>
#include <Wire.h>
#include <math.h>

#define SENSOR_ADDR 0x35

// Forward declarations
void testResult(bool pass);

// Globals
Adafruit_TMAG5273 sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== hw_test_06_range ==="));
  Serial.println(F("Testing: Range switching (RAW values)"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Enable all channels
  sensor.setMagneticChannels(TMAG5273_MAG_CH_XYZ);

  bool allPass = true;

  // Test 1: Narrow range (±40mT for A1, ±133mT for A2)
  Serial.println(F("\n1. Narrow range:"));
  sensor.setXYRangeWide(false); // false = narrow
  sensor.setZRangeWide(false);
  delay(50);

  // Take multiple RAW readings and average
  long narrowRawSum = 0;
  for (int i = 0; i < 5; i++) {
    narrowRawSum += sensor.readX(); // RAW read
    delay(20);
  }
  int16_t narrowRaw = narrowRawSum / 5;

  Serial.print(F("   X RAW (narrow): "));
  Serial.println(narrowRaw);

  // Also show converted for reference
  float narrowUT = sensor.readMagneticX();
  Serial.print(F("   X uT (narrow):  "));
  Serial.print(narrowUT, 1);
  Serial.println(F(" uT"));

  // Test 2: Wide range (±80mT for A1, ±266mT for A2)
  Serial.println(F("\n2. Wide range:"));
  sensor.setXYRangeWide(true); // true = wide
  sensor.setZRangeWide(true);
  delay(50);

  long wideRawSum = 0;
  for (int i = 0; i < 5; i++) {
    wideRawSum += sensor.readX(); // RAW read
    delay(20);
  }
  int16_t wideRaw = wideRawSum / 5;

  Serial.print(F("   X RAW (wide): "));
  Serial.println(wideRaw);

  // Also show converted for reference
  float wideUT = sensor.readMagneticX();
  Serial.print(F("   X uT (wide):  "));
  Serial.print(wideUT, 1);
  Serial.println(F(" uT"));

  // Compare RAW values
  // At wide range, sensitivity is halved, so same field gives ~half the raw
  // value
  Serial.println(F("\n3. RAW value comparison:"));
  Serial.print(F("   Narrow RAW: "));
  Serial.println(narrowRaw);
  Serial.print(F("   Wide RAW:   "));
  Serial.println(wideRaw);

  // Calculate ratio using absolute values (field could be negative)
  float ratio = 0;
  if (abs(wideRaw) > 0) {
    ratio = (float)abs(narrowRaw) / (float)abs(wideRaw);
  }

  Serial.print(F("   Ratio (narrow/wide): "));
  Serial.print(ratio, 2);
  Serial.print(F(" (need 1.3-3.0): "));

  // Wide range has 2x the range, so ~half sensitivity
  // Raw value at narrow should be ~2x raw at wide
  // Allow 1.3 to 3.0 for tolerance
  if (ratio >= 1.3 && ratio <= 3.0) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
    allPass = false;
  }

  // Also verify converted uT values are similar (within 30%)
  Serial.println(F("\n4. Converted uT comparison (should be similar):"));
  Serial.print(F("   Narrow: "));
  Serial.print(narrowUT, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Wide:   "));
  Serial.print(wideUT, 1);
  Serial.println(F(" uT"));

  float utDiff = abs(narrowUT - wideUT);
  float utAvg = (abs(narrowUT) + abs(wideUT)) / 2;
  float percentDiff = 0;
  if (utAvg > 0) {
    percentDiff = (utDiff / utAvg) * 100;
  }

  Serial.print(F("   Difference: "));
  Serial.print(percentDiff, 1);
  Serial.print(F("% (expect <30%): "));

  if (percentDiff <= 30) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("marginal"));
    // Don't fail on this, just informational
  }

  Serial.println();
  testResult(allPass);
}

void loop() {
  // Nothing to do
}

// Helper: Print result
void testResult(bool pass) {
  if (pass) {
    Serial.println(F("=== PASS ==="));
  } else {
    Serial.println(F("=== FAIL ==="));
  }
}
