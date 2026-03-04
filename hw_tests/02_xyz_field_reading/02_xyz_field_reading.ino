/*!
 * 02_xyz_field_reading.ino - Basic Magnetic Reads Test for TMAG5273
 *
 * Verifies:
 * - Read X/Y/Z in µT at 0° position
 * - Read X/Y/Z in µT at 180° position
 * - At least one axis changed >200 µT
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

  Serial.println(F("=== hw_test_02_magnetic_reads ==="));
  Serial.println(F("Testing: Basic magnetic field reads"));

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

  // Move to 0° position (highest X, lowest |Y|)
  Serial.println(F("\n1. Moving servo to 0 degrees..."));
  moveServo(0);

  float x0 = medianRead(sensor, READ_MAG_X);
  float y0 = medianRead(sensor, READ_MAG_Y);
  float z0 = medianRead(sensor, READ_MAG_Z);

  Serial.print(F("   X at 0: "));
  Serial.print(x0);
  Serial.println(F(" uT"));
  Serial.print(F("   Y at 0: "));
  Serial.print(y0);
  Serial.println(F(" uT"));
  Serial.print(F("   Z at 0: "));
  Serial.print(z0);
  Serial.println(F(" uT"));

  // Check for reasonable values (not all zeros)
  if (x0 == 0 && y0 == 0 && z0 == 0) {
    Serial.println(F("   WARNING: All zeros at 0 position"));
  }

  // Move to 180° position (lowest X, highest |Y|)
  Serial.println(F("\n2. Moving servo to 180 degrees..."));
  moveServo(180);

  float x180 = medianRead(sensor, READ_MAG_X);
  float y180 = medianRead(sensor, READ_MAG_Y);
  float z180 = medianRead(sensor, READ_MAG_Z);

  Serial.print(F("   X at 180: "));
  Serial.print(x180);
  Serial.println(F(" uT"));
  Serial.print(F("   Y at 180: "));
  Serial.print(y180);
  Serial.println(F(" uT"));
  Serial.print(F("   Z at 180: "));
  Serial.print(z180);
  Serial.println(F(" uT"));

  // Calculate differences
  float diffX = abs(x0 - x180);
  float diffY = abs(y0 - y180);
  float diffZ = abs(z0 - z180);

  Serial.println(F("\n3. Differences (0 vs 180):"));
  Serial.print(F("   Delta X: "));
  Serial.print(diffX);
  Serial.println(F(" uT"));
  Serial.print(F("   Delta Y: "));
  Serial.print(diffY);
  Serial.println(F(" uT"));
  Serial.print(F("   Delta Z: "));
  Serial.print(diffZ);
  Serial.println(F(" uT"));

  // Check if at least one axis changed >200 µT
  float maxDiff = max(diffX, max(diffY, diffZ));
  Serial.print(F("\n4. Max difference: "));
  Serial.print(maxDiff);
  Serial.print(F(" uT (need >200): "));

  if (maxDiff > 200) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAIL"));
    allPass = false;
  }

  // Return servo to 180
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

// Helper: Move servo and wait for settle (keep attached!)
void moveServo(int angle) {
  servo.attach(SERVO_PIN);
  servo.write(angle);
  delay(2000);
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
