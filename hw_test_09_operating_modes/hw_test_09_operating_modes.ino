/*!
 * hw_test_09_operating_modes.ino - Operating Modes Test for TMAG5273
 *
 * Verifies:
 * - Continuous mode: reads work
 * - Standby mode: trigger conversion, verify read works
 * - Mode switching doesn't break reads
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

  Serial.println(F("=== hw_test_09_operating_modes ==="));
  Serial.println(F("Testing: Operating modes"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Enable all channels
  sensor.setMagneticChannels(TMAG5273_MAG_CH_XYZ);

  // Position servo
  Serial.println(F("\nPositioning servo at 120..."));
  moveServo(120);

  bool allPass = true;

  // Test 1: Continuous mode
  Serial.println(F("\n1. Continuous mode:"));
  sensor.setOperatingMode(TMAG5273_MODE_CONTINUOUS);
  delay(100);

  float x = sensor.readMagneticX();
  float y = sensor.readMagneticY();
  float z = sensor.readMagneticZ();

  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Y: "));
  Serial.print(y, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Z: "));
  Serial.print(z, 1);
  Serial.println(F(" uT"));

  bool contOk = isValidReading(x) && isValidReading(y) && isValidReading(z);
  Serial.print(F("   Status: "));
  if (contOk) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
    allPass = false;
  }

  // Test 2: Standby mode with trigger
  Serial.println(F("\n2. Standby mode (triggered):"));
  sensor.setOperatingMode(TMAG5273_MODE_STANDBY);
  delay(50);

  // Trigger a conversion
  sensor.triggerConversion();
  delay(50);

  x = sensor.readMagneticX();
  y = sensor.readMagneticY();
  z = sensor.readMagneticZ();

  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Y: "));
  Serial.print(y, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Z: "));
  Serial.print(z, 1);
  Serial.println(F(" uT"));

  bool standbyOk = isValidReading(x) && isValidReading(y) && isValidReading(z);
  Serial.print(F("   Status: "));
  if (standbyOk) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
    allPass = false;
  }

  // Test 3: Switch back to continuous
  Serial.println(F("\n3. Back to continuous:"));
  sensor.setOperatingMode(TMAG5273_MODE_CONTINUOUS);
  delay(100);

  x = sensor.readMagneticX();
  y = sensor.readMagneticY();
  z = sensor.readMagneticZ();

  Serial.print(F("   X: "));
  Serial.print(x, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Y: "));
  Serial.print(y, 1);
  Serial.println(F(" uT"));
  Serial.print(F("   Z: "));
  Serial.print(z, 1);
  Serial.println(F(" uT"));

  bool backOk = isValidReading(x) && isValidReading(y) && isValidReading(z);
  Serial.print(F("   Status: "));
  if (backOk) {
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

// Helper: Check if reading is valid
bool isValidReading(float val) {
  if (isnan(val))
    return false;
  if (val < -150000 || val > 150000)
    return false;
  return true;
}
