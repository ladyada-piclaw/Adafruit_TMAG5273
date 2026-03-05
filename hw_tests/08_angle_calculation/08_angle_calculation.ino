/*!
 * 08_angle_calculation.ino - Angle Calculation Test for TMAG5273
 *
 * Verifies:
 * - Angle calculation in XZ mode works (Z changes dramatically with servo)
 * - Angle values are valid (0-360°)
 * - Angles differ by at least 5° between 40° and 180° servo positions
 *   (40° is field minimum, 180° is field maximum based on calibration)
 *
 * Hardware: Metro Mini, TMAG5273A2 at 0x35, Servo on D4
 */

#include <Adafruit_TMAG5273.h>
#include <Servo.h>
#include <Wire.h>

#define SERVO_PIN 4
#define SENSOR_ADDR 0x35

// ReadType enum for medianRead
enum ReadType {
  READ_X,
  READ_Y,
  READ_Z,
  READ_TEMP,
  READ_MAG_X,
  READ_MAG_Y,
  READ_MAG_Z,
  READ_ANGLE,
  READ_MAGNITUDE
};

// Forward declarations
void testResult(bool pass);
void moveServo(int angle);
float medianRead(Adafruit_TMAG5273& sens, ReadType type, int samples = 5);

// Globals
Adafruit_TMAG5273 sensor;
Servo servo;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== hw_test_08_angle ==="));
  Serial.println(F("Testing: Angle calculation (40 vs 180 servo)"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Enable XZ channels and angle calculation (Z changes most with servo)
  sensor.setMagneticChannels(TMAG5273_MAG_CH_ZX);
  sensor.setAngleCalculation(TMAG5273_ANGLE_XZ);
  delay(50);

  bool allPass = true;

  // Test 1: Angle at 40° servo position (field minimum)
  Serial.println(F("\n1. Angle at servo 40 degrees:"));
  moveServo(40);

  float angle40 = medianRead(sensor, READ_ANGLE);
  Serial.print(F("   Calculated angle: "));
  Serial.print(angle40, 1);
  Serial.print(F(" deg - "));

  bool valid40 = (angle40 >= 0 && angle40 <= 360);
  if (valid40) {
    Serial.println(F("valid"));
  } else {
    Serial.println(F("INVALID"));
    allPass = false;
  }

  // Also print X/Z for reference
  float x40 = sensor.readMagneticX();
  float z40 = sensor.readMagneticZ();
  Serial.print(F("   X: "));
  Serial.print(x40, 0);
  Serial.print(F(" uT, Z: "));
  Serial.print(z40, 0);
  Serial.println(F(" uT"));

  // Test 2: Angle at 180° servo position (field maximum)
  Serial.println(F("\n2. Angle at servo 180 degrees:"));
  moveServo(180);

  float angle180 = medianRead(sensor, READ_ANGLE);
  Serial.print(F("   Calculated angle: "));
  Serial.print(angle180, 1);
  Serial.print(F(" deg - "));

  bool valid180 = (angle180 >= 0 && angle180 <= 360);
  if (valid180) {
    Serial.println(F("valid"));
  } else {
    Serial.println(F("INVALID"));
    allPass = false;
  }

  // Also print X/Z for reference
  float x180 = sensor.readMagneticX();
  float z180 = sensor.readMagneticZ();
  Serial.print(F("   X: "));
  Serial.print(x180, 0);
  Serial.print(F(" uT, Z: "));
  Serial.print(z180, 0);
  Serial.println(F(" uT"));

  // Test 3: Compare angles - should differ by at least 5°
  Serial.println(F("\n3. Angle comparison:"));
  Serial.print(F("   Angle at 40:  "));
  Serial.print(angle40, 1);
  Serial.println(F(" deg"));
  Serial.print(F("   Angle at 180: "));
  Serial.print(angle180, 1);
  Serial.println(F(" deg"));

  float diff = abs(angle40 - angle180);
  // Handle wrap-around (e.g., 350° vs 10° = 20° difference, not 340°)
  if (diff > 180) {
    diff = 360 - diff;
  }

  Serial.print(F("   Difference: "));
  Serial.print(diff, 1);
  Serial.print(F(" deg (need >=5): "));

  if (diff >= 5.0) {
    Serial.println(F("OK - angles differ"));
  } else {
    Serial.println(F("FAIL - angles too similar"));
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

// Helper: Median read for noise reduction
float medianRead(Adafruit_TMAG5273& sens, ReadType type, int samples) {
  float vals[11];
  if (samples > 11)
    samples = 11;
  for (int i = 0; i < samples; i++) {
    switch (type) {
      case READ_X:
        vals[i] = sens.readX();
        break;
      case READ_Y:
        vals[i] = sens.readY();
        break;
      case READ_Z:
        vals[i] = sens.readZ();
        break;
      case READ_TEMP:
        vals[i] = sens.getTemperature();
        break;
      case READ_MAG_X:
        vals[i] = sens.readMagneticX();
        break;
      case READ_MAG_Y:
        vals[i] = sens.readMagneticY();
        break;
      case READ_MAG_Z:
        vals[i] = sens.readMagneticZ();
        break;
      case READ_ANGLE:
        vals[i] = sens.readAngle();
        break;
      case READ_MAGNITUDE:
        vals[i] = (float)sens.readMagnitude();
        break;
    }
    delay(20);
  }
  // Bubble sort
  for (int i = 0; i < samples - 1; i++) {
    for (int j = i + 1; j < samples; j++) {
      if (vals[j] < vals[i]) {
        float t = vals[i];
        vals[i] = vals[j];
        vals[j] = t;
      }
    }
  }
  return vals[samples / 2];
}
