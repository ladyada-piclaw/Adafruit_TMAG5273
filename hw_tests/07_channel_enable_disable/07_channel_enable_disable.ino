/*!
 * 07_channel_enable_disable.ino - Channel Enable Test for TMAG5273
 *
 * Verifies:
 * - X only: readX() returns valid data
 * - XY: both X and Y return valid data
 * - XYZ: all three return valid data
 * - Z only: readZ() works
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
void moveServo(int angle);
bool isValidReading(float val);

// Globals
Adafruit_TMAG5273 sensor;
Servo servo;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== hw_test_07_channels ==="));
  Serial.println(F("Testing: Channel enable configurations"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Position servo at moderate field
  Serial.println(F("\nPositioning servo at 120..."));
  moveServo(120);

  bool allPass = true;

  // Test 1: X only
  Serial.println(F("\n1. X channel only:"));
  sensor.setMagneticChannels(TMAG5273_MAG_CH_X);
  delay(50);

  float x = sensor.readMagneticX();
  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.print(F(" uT - "));
  if (isValidReading(x)) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
    allPass = false;
  }

  // Test 2: XY
  Serial.println(F("\n2. XY channels:"));
  sensor.setMagneticChannels(TMAG5273_MAG_CH_XY);
  delay(50);

  x = sensor.readMagneticX();
  float y = sensor.readMagneticY();
  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.print(F(" uT - "));
  Serial.println(isValidReading(x) ? F("OK") : F("FAIL"));
  Serial.print(F("   Y: "));
  Serial.print(y, 1);
  Serial.print(F(" uT - "));
  Serial.println(isValidReading(y) ? F("OK") : F("FAIL"));

  if (!isValidReading(x) || !isValidReading(y)) {
    allPass = false;
  }

  // Test 3: XYZ
  Serial.println(F("\n3. XYZ channels:"));
  sensor.setMagneticChannels(TMAG5273_MAG_CH_XYZ);
  delay(50);

  x = sensor.readMagneticX();
  y = sensor.readMagneticY();
  float z = sensor.readMagneticZ();
  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.print(F(" uT - "));
  Serial.println(isValidReading(x) ? F("OK") : F("FAIL"));
  Serial.print(F("   Y: "));
  Serial.print(y, 1);
  Serial.print(F(" uT - "));
  Serial.println(isValidReading(y) ? F("OK") : F("FAIL"));
  Serial.print(F("   Z: "));
  Serial.print(z, 1);
  Serial.print(F(" uT - "));
  Serial.println(isValidReading(z) ? F("OK") : F("FAIL"));

  if (!isValidReading(x) || !isValidReading(y) || !isValidReading(z)) {
    allPass = false;
  }

  // Test 4: Z only
  Serial.println(F("\n4. Z channel only:"));
  sensor.setMagneticChannels(TMAG5273_MAG_CH_Z);
  delay(50);

  z = sensor.readMagneticZ();
  Serial.print(F("   Z: "));
  Serial.print(z, 1);
  Serial.print(F(" uT - "));
  if (isValidReading(z)) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
    allPass = false;
  }

  // Return servo
  moveServo(180);

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
  servo.detach();
}

// Helper: Move servo and wait for settle
void moveServo(int angle) {
  servo.attach(SERVO_PIN);
  servo.write(angle);
  delay(1500);
  servo.detach();
}

// Helper: Check if reading is valid (not NaN, not extreme)
bool isValidReading(float val) {
  if (isnan(val))
    return false;
  // Valid range for 133mT = ±133000 uT
  if (val < -150000 || val > 150000)
    return false;
  return true;
}
