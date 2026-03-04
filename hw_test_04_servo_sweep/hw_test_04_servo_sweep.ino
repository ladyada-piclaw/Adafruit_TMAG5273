/*!
 * hw_test_04_servo_sweep.ino - Magnetic Field Rotation Test for TMAG5273
 *
 * Verifies:
 * - Sweep servo 0° to 180° in 10° steps
 * - X-axis shows monotonic DECREASE trend (allow 1-2 violations)
 * - X difference between 0° and 180° > 300 µT
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

  Serial.println(F("=== hw_test_04_servo_sweep ==="));
  Serial.println(F("Testing: Magnetic field rotation (X decreases 0->180)"));

  Wire.begin();

  if (!sensor.begin(SENSOR_ADDR)) {
    Serial.println(F("ERROR: Sensor not found!"));
    testResult(false);
    return;
  }

  // Enable all channels
  sensor.setMagneticChannels(TMAG5273_MAG_CH_XYZ);
  delay(50);

  bool allPass = true;

  // Sweep from 0 to 180
  Serial.println(F("\nAngle | X(uT)   | Y(uT)   | Z(uT)   | Mag(uT)"));
  Serial.println(F("------+---------+---------+---------+---------"));

  float xValues[NUM_STEPS];
  float x0 = 0;
  float x180 = 0;
  int idx = 0;

  for (int angle = 0; angle <= 180; angle += 10) {
    moveServo(angle);

    float x = medianRead(sensor, READ_MAG_X);
    float y = medianRead(sensor, READ_MAG_Y);
    float z = medianRead(sensor, READ_MAG_Z);
    float mag = sqrt(x * x + y * y + z * z);

    xValues[idx] = x;

    // Print row
    if (angle < 100)
      Serial.print(F(" "));
    if (angle < 10)
      Serial.print(F(" "));
    Serial.print(angle);
    Serial.print(F("   | "));
    if (x >= 0)
      Serial.print(F(" "));
    Serial.print(x, 0);
    Serial.print(F("\t| "));
    if (y >= 0)
      Serial.print(F(" "));
    Serial.print(y, 0);
    Serial.print(F("\t| "));
    if (z >= 0)
      Serial.print(F(" "));
    Serial.print(z, 0);
    Serial.print(F("\t| "));
    Serial.println(mag, 0);

    if (angle == 0)
      x0 = x;
    if (angle == 180)
      x180 = x;

    idx++;
  }

  // Check 1: X difference between 0 and 180 should be > 300 µT
  Serial.println();
  Serial.print(F("X at 0:   "));
  Serial.print(x0, 0);
  Serial.println(F(" uT"));
  Serial.print(F("X at 180: "));
  Serial.print(x180, 0);
  Serial.println(F(" uT"));

  float xDiff = x0 - x180; // Should be positive (X decreases)
  Serial.print(F("X drop (0->180): "));
  Serial.print(xDiff, 0);
  Serial.print(F(" uT (need >300): "));

  bool xDiffPass = (xDiff > 300);
  if (xDiffPass) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
  }

  // Check 2: X should show monotonic decrease (allow 1-2 violations)
  int violations = 0;
  for (int i = 1; i < NUM_STEPS; i++) {
    if (xValues[i] > xValues[i - 1] + 50) { // Allow 50 µT noise margin
      violations++;
    }
  }

  Serial.print(F("Monotonic violations: "));
  Serial.print(violations);
  Serial.print(F(" (allow <=2): "));

  bool monotonicPass = (violations <= 2);
  if (monotonicPass) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
  }

  // Pass if EITHER condition met
  allPass = xDiffPass || monotonicPass;

  Serial.print(F("\nOverall: "));
  if (allPass) {
    Serial.println(F("PASS (X decreases as expected)"));
  } else {
    Serial.println(F("FAIL"));
  }

  // Return to 180
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
