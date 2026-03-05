/*!
 * 10_i2c_crc.ino - CRC Mode Test for TMAG5273
 *
 * Note: Place a magnet near the sensor for best results
 *
 * Verifies:
 * - Reads work with CRC enabled
 * - Reads work with CRC disabled
 * - No corruption in either mode
 *
 * Hardware: Metro Mini, TMAG5273A2 at 0x35
 */

#include <Adafruit_TMAG5273.h>
#include <Wire.h>

#define SENSOR_ADDR 0x35

// Forward declarations
void testResult(bool pass);
bool isValidReading(float val);

// Globals
Adafruit_TMAG5273 sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== hw_test_10_crc ==="));
  Serial.println(F("Testing: CRC mode"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Enable all channels and temperature
  sensor.setMagneticChannels(TMAG5273_MAG_CH_XYZ);
  sensor.enableTemperature(true);

  bool allPass = true;

  // Test 1: Enable CRC
  Serial.println(F("\n1. CRC enabled:"));
  sensor.enableCRC(true);
  delay(50);

  bool crcEnabled = sensor.crcEnabled();
  Serial.print(F("   CRC status: "));
  Serial.println(crcEnabled ? F("enabled") : F("disabled"));

  float x = sensor.readMagneticX();
  float y = sensor.readMagneticY();
  float z = sensor.readMagneticZ();
  float temp = sensor.getTemperature();

  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Y: "));
  Serial.print(y, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Z: "));
  Serial.print(z, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Temp: "));
  Serial.print(temp, 1);
  Serial.println(F(" C"));

  bool crcOnOk = isValidReading(x) && isValidReading(y) && isValidReading(z) &&
                 (temp >= 10 && temp <= 60);
  Serial.print(F("   Status: "));
  if (crcOnOk) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
    allPass = false;
  }

  // Test 2: Disable CRC
  Serial.println(F("\n2. CRC disabled:"));
  sensor.enableCRC(false);
  delay(50);

  crcEnabled = sensor.crcEnabled();
  Serial.print(F("   CRC status: "));
  Serial.println(crcEnabled ? F("enabled") : F("disabled"));

  x = sensor.readMagneticX();
  y = sensor.readMagneticY();
  z = sensor.readMagneticZ();
  temp = sensor.getTemperature();

  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Y: "));
  Serial.print(y, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Z: "));
  Serial.print(z, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Temp: "));
  Serial.print(temp, 1);
  Serial.println(F(" C"));

  bool crcOffOk = isValidReading(x) && isValidReading(y) && isValidReading(z) &&
                  (temp >= 10 && temp <= 60);
  Serial.print(F("   Status: "));
  if (crcOffOk) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
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

// Helper: Check if reading is valid
bool isValidReading(float val) {
  if (isnan(val))
    return false;
  if (val < -150000 || val > 150000)
    return false;
  return true;
}
