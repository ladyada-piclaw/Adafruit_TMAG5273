/*!
 * 04_servo_proximity_sweep.ino - Magnetic Field vs Servo Position for TMAG5273
 *
 * Verifies:
 * - Sweep servo 0 to 180 in 10 degree steps
 * - Total field magnitude at 180 > magnitude at 0 by at least 2x
 * - At least one axis changes by > 1000 uT across the sweep
 *
 * Hardware: Metro Mini, TMAG5273A2 at 0x35, Servo on D4
 */

#include <Adafruit_TMAG5273.h>
#include <Servo.h>
#include <Wire.h>
#include <math.h>

#define SERVO_PIN 4
#define SENSOR_ADDR 0x35
#define NUM_STEPS 19

// Forward declarations
void testResult(bool pass);
void moveServo(int angle);

// Globals
Adafruit_TMAG5273 sensor;
Servo servo;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== hw_test_04_servo_sweep ==="));
  Serial.println(F("Testing: Magnetic field vs servo position"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  sensor.setMagneticChannels(TMAG5273_MAG_CH_XYZ);
  delay(50);

  servo.attach(SERVO_PIN);

  float mag0 = 0, mag180 = 0;
  float xMin = 999999, xMax = -999999;
  float yMin = 999999, yMax = -999999;
  float zMin = 999999, zMax = -999999;

  Serial.println(F("Angle | X(uT)   | Y(uT)   | Z(uT)   | Mag(uT)"));
  Serial.println(F("------+---------+---------+---------+---------"));

  for (int angle = 0; angle <= 180; angle += 10) {
    servo.write(angle);
    delay(1500);

    float x = sensor.readMagneticX();
    float y = sensor.readMagneticY();
    float z = sensor.readMagneticZ();
    float mag = sqrt(x * x + y * y + z * z);

    // Track min/max per axis
    if (x < xMin)
      xMin = x;
    if (x > xMax)
      xMax = x;
    if (y < yMin)
      yMin = y;
    if (y > yMax)
      yMax = y;
    if (z < zMin)
      zMin = z;
    if (z > zMax)
      zMax = z;

    // Print row
    if (angle < 100)
      Serial.print(F(" "));
    if (angle < 10)
      Serial.print(F(" "));
    Serial.print(angle);
    Serial.print(F("   | "));
    Serial.print(x, 0);
    Serial.print(F("\t| "));
    Serial.print(y, 0);
    Serial.print(F("\t| "));
    Serial.print(z, 0);
    Serial.print(F("\t| "));
    Serial.println(mag, 0);

    if (angle == 0)
      mag0 = mag;
    if (angle == 180)
      mag180 = mag;
  }

  servo.detach();

  // Check 1: magnitude at 180 should be > 2x magnitude at 0
  Serial.println();
  Serial.print(F("Magnitude at 0: "));
  Serial.print(mag0, 0);
  Serial.println(F(" uT"));
  Serial.print(F("Magnitude at 180: "));
  Serial.print(mag180, 0);
  Serial.println(F(" uT"));

  float ratio = mag180 / mag0;
  Serial.print(F("Ratio (180/0): "));
  Serial.print(ratio, 2);
  Serial.print(F(" (need >2.0): "));

  bool ratioPass = (ratio > 2.0);
  Serial.println(ratioPass ? F("OK") : F("FAIL"));

  // Check 2: at least one axis range > 1000 uT
  float xRange = xMax - xMin;
  float yRange = yMax - yMin;
  float zRange = zMax - zMin;
  float maxRange = max(xRange, max(yRange, zRange));

  Serial.print(F("X range: "));
  Serial.print(xRange, 0);
  Serial.println(F(" uT"));
  Serial.print(F("Y range: "));
  Serial.print(yRange, 0);
  Serial.println(F(" uT"));
  Serial.print(F("Z range: "));
  Serial.print(zRange, 0);
  Serial.println(F(" uT"));
  Serial.print(F("Max axis range: "));
  Serial.print(maxRange, 0);
  Serial.print(F(" uT (need >1000): "));

  bool rangePass = (maxRange > 1000);
  Serial.println(rangePass ? F("OK") : F("FAIL"));

  Serial.println();
  testResult(ratioPass && rangePass);
}

void loop() {}

void testResult(bool pass) {
  if (pass) {
    Serial.println(F("=== PASS ==="));
  } else {
    Serial.println(F("=== FAIL ==="));
  }
}

void moveServo(int angle) {
  servo.attach(SERVO_PIN);
  servo.write(angle);
  delay(2000);
}
