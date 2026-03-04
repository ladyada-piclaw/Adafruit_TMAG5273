/*!
 * 01_sensor_init.ino - Sensor Init Test for TMAG5273
 *
 * Verifies:
 * - begin() succeeds at 0x35
 * - Manufacturer ID == 0x5449
 * - Device ID bits[1:0] == 0x02 (x2 variant)
 *
 * Hardware: Metro Mini, TMAG5273A2 at 0x35, Servo on D4
 */

#include <Adafruit_TMAG5273.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define SENSOR_ADDR 0x35

// Forward declarations
void testResult(bool pass);

// Globals
Adafruit_TMAG5273 sensor;
Servo servo;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== hw_test_01_begin ==="));
  Serial.println(F("Testing: Sensor initialization"));

  // Attach and position servo
  servo.attach(SERVO_PIN);
  servo.write(180);
  delay(1000);
  servo.detach();

  Wire.begin();

  bool allPass = true;

  // Test 1: begin() succeeds
  Serial.print(F("1. begin() at 0x35: "));
  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("FAIL - begin() returned false"));
    allPass = false;
  } else {
    Serial.println(F("OK"));
  }

  // Test 2: Manufacturer ID
  Serial.print(F("2. Manufacturer ID: "));
  uint16_t mfgId = sensor.getManufacturerID();
  Serial.print(F("0x"));
  Serial.print(mfgId, HEX);
  if (mfgId != 0x5449) {
    Serial.println(F(" - FAIL (expected 0x5449)"));
    allPass = false;
  } else {
    Serial.println(F(" - OK"));
  }

  // Test 3: Device ID bits[1:0] == 0x02 for x2 variant
  Serial.print(F("3. Device ID: "));
  uint8_t devId = sensor.getDeviceID();
  Serial.print(F("0x"));
  Serial.print(devId, HEX);
  Serial.print(F(" (VER="));
  Serial.print((devId >> 2) & 0x03);
  Serial.print(F(", variant="));
  Serial.print(devId & 0x03);
  Serial.print(F("): "));
  if ((devId & 0x03) != 0x02) {
    Serial.println(F("FAIL (expected variant=2 for x2)"));
    allPass = false;
  } else {
    Serial.println(F("OK"));
  }

  // Final result
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
