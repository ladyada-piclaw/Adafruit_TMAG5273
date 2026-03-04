/*!
 * 03_temperature_reading.ino - Temperature Read Test for TMAG5273
 *
 * Verifies:
 * - Temperature in reasonable range (10°C to 60°C)
 * - Multiple reads are stable (within ±2°C)
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

  Serial.println(F("=== hw_test_03_temperature ==="));
  Serial.println(F("Testing: Temperature reads"));

  // Position servo
  servo.attach(SERVO_PIN);
  servo.write(180);
  delay(1000);
  servo.detach();

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Enable temperature channel
  sensor.enableTemperature(true);
  delay(50);

  bool allPass = true;

  // Test 1: Read temperature and check range
  Serial.println(F("\n1. Reading temperature..."));
  float temp = sensor.getTemperature();
  Serial.print(F("   Temperature: "));
  Serial.print(temp);
  Serial.print(F(" C - "));

  if (temp >= 10.0 && temp <= 60.0) {
    Serial.println(F("OK (in range 10-60C)"));
  } else {
    Serial.println(F("FAIL (out of range)"));
    allPass = false;
  }

  // Test 2: Multiple reads for stability
  Serial.println(F("\n2. Stability test (10 reads)..."));
  float minTemp = 1000.0;
  float maxTemp = -1000.0;
  float sumTemp = 0.0;

  for (int i = 0; i < 10; i++) {
    float t = sensor.getTemperature();
    Serial.print(F("   Read "));
    Serial.print(i + 1);
    Serial.print(F(": "));
    Serial.print(t);
    Serial.println(F(" C"));

    if (t < minTemp)
      minTemp = t;
    if (t > maxTemp)
      maxTemp = t;
    sumTemp += t;
    delay(100);
  }

  float avgTemp = sumTemp / 10.0;
  float range = maxTemp - minTemp;

  Serial.println(F("\n3. Statistics:"));
  Serial.print(F("   Min: "));
  Serial.print(minTemp);
  Serial.println(F(" C"));
  Serial.print(F("   Max: "));
  Serial.print(maxTemp);
  Serial.println(F(" C"));
  Serial.print(F("   Avg: "));
  Serial.print(avgTemp);
  Serial.println(F(" C"));
  Serial.print(F("   Range: "));
  Serial.print(range);
  Serial.print(F(" C (need <4C): "));

  if (range <= 4.0) {
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
