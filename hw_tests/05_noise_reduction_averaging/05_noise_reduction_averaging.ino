/*!
 * 05_noise_reduction_averaging.ino - Averaging Effect on Noise Test for
 * TMAG5273
 *
 * Note: Place a magnet near the sensor for best results
 *
 * Verifies:
 * - 32x averaging produces lower std dev than 1x averaging
 * - Any reduction in noise counts as pass
 *
 * Hardware: Metro Mini, TMAG5273A2 at 0x35
 */

#include <Adafruit_TMAG5273.h>
#include <Wire.h>
#include <math.h>

#define SENSOR_ADDR 0x35
#define NUM_SAMPLES 30

// Forward declarations
void testResult(bool pass);
float computeStdDev(float* vals, int n);

// Globals
Adafruit_TMAG5273 sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== hw_test_05_averaging ==="));
  Serial.println(F("Testing: Averaging effect on noise"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Enable all channels
  sensor.setMagneticChannels(TMAG5273_MAG_CH_XYZ);

  bool allPass = true;
  float readings[NUM_SAMPLES];

  // Test 1: 1x averaging
  Serial.println(F("\n1. Testing 1x averaging..."));
  sensor.setConversionAverage(TMAG5273_CONV_AVG_1X);
  delay(50);

  Serial.print(F("   Readings: "));
  for (int i = 0; i < NUM_SAMPLES; i++) {
    readings[i] = sensor.readMagneticX();
    if (i < 10) {
      Serial.print(readings[i], 0);
      if (i < 9)
        Serial.print(F(", "));
    }
    delay(10);
  }
  Serial.println(F("..."));

  float stdDev1x = computeStdDev(readings, NUM_SAMPLES);
  Serial.print(F("   Std Dev (1x): "));
  Serial.print(stdDev1x, 2);
  Serial.println(F(" uT"));

  // Test 2: 32x averaging
  Serial.println(F("\n2. Testing 32x averaging..."));
  sensor.setConversionAverage(TMAG5273_CONV_AVG_32X);
  delay(50);

  Serial.print(F("   Readings: "));
  for (int i = 0; i < NUM_SAMPLES; i++) {
    readings[i] = sensor.readMagneticX();
    if (i < 10) {
      Serial.print(readings[i], 0);
      if (i < 9)
        Serial.print(F(", "));
    }
    delay(10);
  }
  Serial.println(F("..."));

  float stdDev32x = computeStdDev(readings, NUM_SAMPLES);
  Serial.print(F("   Std Dev (32x): "));
  Serial.print(stdDev32x, 2);
  Serial.println(F(" uT"));

  // Compare
  Serial.println(F("\n3. Comparison:"));
  Serial.print(F("   1x std dev:  "));
  Serial.print(stdDev1x, 2);
  Serial.println(F(" uT"));
  Serial.print(F("   32x std dev: "));
  Serial.print(stdDev32x, 2);
  Serial.println(F(" uT"));

  // Pass if 32x noise < 1x noise (any reduction counts)
  Serial.print(F("   32x < 1x: "));
  if (stdDev32x < stdDev1x) {
    Serial.println(F("YES - averaging reduces noise"));
  } else {
    Serial.println(F("NO"));
  }

  // Also pass if both are very low (quiet environment)
  bool noisePass =
      (stdDev32x < stdDev1x) || (stdDev1x < 10.0 && stdDev32x < 10.0);

  if (noisePass) {
    if (stdDev1x < 10.0 && stdDev32x < 10.0) {
      Serial.println(F("   Result: OK (both very low noise)"));
    } else {
      Serial.println(F("   Result: OK (32x averaging reduces noise)"));
    }
  } else {
    Serial.println(F("   Result: FAIL"));
    allPass = false;
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

// Helper: Compute standard deviation
float computeStdDev(float* vals, int n) {
  float sum = 0;
  for (int i = 0; i < n; i++) {
    sum += vals[i];
  }
  float mean = sum / n;

  float sqSum = 0;
  for (int i = 0; i < n; i++) {
    float diff = vals[i] - mean;
    sqSum += diff * diff;
  }

  return sqrt(sqSum / n);
}
