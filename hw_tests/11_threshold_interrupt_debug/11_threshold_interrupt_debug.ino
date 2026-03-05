/*!
 * @file 11_threshold_interrupt_debug.ino
 * @brief Debug test for TMAG5273 threshold interrupt with servo magnet
 *
 * Hardware setup:
 * - TMAG5273 on I2C (SDA/SCL)
 * - INT pin connected to D2 (INPUT_PULLUP)
 * - Servo with magnet on D4
 */

#include <Adafruit_TMAG5273.h>
#include <Servo.h>
#include <Wire.h>

Adafruit_TMAG5273 tmag;
Servo servo;

#define SERVO_PIN 4
#define INT_PIN 2

volatile uint16_t isrCount = 0;
bool intEverLow = false;
bool exceededThreshold = false;
float maxZ_mT = 0;
float thresholdMT = 0;

void isr() {
  isrCount++;
}

uint8_t readReg(uint8_t regAddr) {
  Wire.beginTransmission(0x35);
  Wire.write(regAddr);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)0x35, (uint8_t)1);
  return Wire.read();
}

void dumpRegisters() {
  Serial.println(F("\n=== Register Dump ==="));
  for (uint8_t reg = 0x00; reg <= 0x08; reg++) {
    Serial.print(F("0x"));
    if (reg < 0x10)
      Serial.print(F("0"));
    Serial.print(reg, HEX);
    Serial.print(F(": 0x"));
    uint8_t val = readReg(reg);
    if (val < 0x10)
      Serial.print(F("0"));
    Serial.println(val, HEX);
  }
  Serial.print(F("0x18: 0x"));
  uint8_t v18 = readReg(0x18);
  if (v18 < 0x10)
    Serial.print(F("0"));
  Serial.println(v18, HEX);
  Serial.print(F("0x1C: 0x"));
  uint8_t v1c = readReg(0x1C);
  if (v1c < 0x10)
    Serial.print(F("0"));
  Serial.println(v1c, HEX);
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("\n=== TMAG5273 Threshold Interrupt Debug Test ==="));

  pinMode(INT_PIN, INPUT_PULLUP);
  servo.attach(SERVO_PIN);
  servo.write(40); // Start far from sensor

  if (!tmag.begin()) {
    Serial.println(F("FAILED to init TMAG5273!"));
    while (1)
      delay(100);
  }
  Serial.println(F("TMAG5273 initialized"));

  // Print variant and range info
  uint8_t devId = tmag.getDeviceID();
  uint8_t variant = devId & 0x03;
  Serial.print(F("Device ID: 0x"));
  Serial.print(devId, HEX);
  Serial.print(F(" - Variant: "));
  if (variant == 0x02) {
    Serial.println(F("x2 (133/266 mT)"));
  } else {
    Serial.println(F("x1 (40/80 mT)"));
  }

  Serial.print(F("XY Range Wide: "));
  Serial.println(tmag.getXYRangeWide() ? F("Yes") : F("No"));
  Serial.print(F("Z Range Wide: "));
  Serial.println(tmag.getZRangeWide() ? F("Yes") : F("No"));

  // Determine range for threshold calculation (x2 wide = 266mT)
  float rangeMT = 266.0; // x2 variant, wide range

  dumpRegisters();

  // ====== Configure for Z-only threshold interrupt ======
  Serial.println(F("\n=== Configuring Threshold Interrupt ==="));

  // Set X and Y thresholds to max (127) so they never trigger
  tmag.setXThreshold(127);
  tmag.setYThreshold(127);
  Serial.println(F("X/Y thresholds set to 127 (disabled)"));

  // Read ambient Z (average 10 samples)
  delay(100);
  float sumZ = 0;
  for (int i = 0; i < 10; i++) {
    sumZ += tmag.readMagneticZ();
    delay(20);
  }
  float ambientZ_uT = sumZ / 10.0;
  float ambientZ_mT = ambientZ_uT / 1000.0;
  Serial.print(F("Ambient Z: "));
  Serial.print(ambientZ_uT, 1);
  Serial.print(F(" uT ("));
  Serial.print(ambientZ_mT, 3);
  Serial.println(F(" mT)"));

  // Calculate threshold: 1.5x ambient
  float targetMT = ambientZ_mT * 1.5;
  if (targetMT < 0)
    targetMT = -targetMT; // Use absolute value
  if (targetMT < 1.0)
    targetMT = 1.0; // Minimum 1mT

  // Convert to register value: threshVal = target_mT * 128.0 / range_mT
  int8_t threshVal = (int8_t)(targetMT * 128.0 / rangeMT);
  if (threshVal < 1)
    threshVal = 1;

  thresholdMT = (float)threshVal * rangeMT / 128.0;

  Serial.print(F("Z Threshold target: "));
  Serial.print(targetMT, 2);
  Serial.print(F(" mT -> reg value: "));
  Serial.print(threshVal);
  Serial.print(F(" -> actual: "));
  Serial.print(thresholdMT, 2);
  Serial.println(F(" mT"));

  tmag.setZThreshold(threshVal);

  // Configure interrupt
  tmag.enableResultInterrupt(false);
  Serial.println(F("Result interrupt: DISABLED"));

  tmag.enableThresholdInterrupt(true);
  Serial.println(F("Threshold interrupt: ENABLED"));

  tmag.setInterruptPulsed(true);
  Serial.print(F("Interrupt pulsed mode: "));
  Serial.println(tmag.getInterruptPulsed() ? F("YES") : F("NO"));

  // Dump INT_CONFIG_1 again
  Serial.print(F("INT_CONFIG_1 (0x08): 0x"));
  uint8_t intConfig = readReg(0x08);
  if (intConfig < 0x10)
    Serial.print(F("0"));
  Serial.println(intConfig, HEX);

  // Attach interrupt
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);
  Serial.println(F("Interrupt attached to D2 (FALLING)"));

  Serial.println(F("\n=== Starting Servo Sweep ==="));
  Serial.println(F("Angle\tZ(uT)\tZ(mT)\tThresh\tINT\tISR\tShould"));

  delay(500);
}

void loop() {
  static int angle = 40;
  static bool done = false;

  if (done)
    return;

  // Read Z
  float z_uT = tmag.readMagneticZ();
  float z_mT = z_uT / 1000.0;
  float absZ_mT = (z_mT < 0) ? -z_mT : z_mT;

  // Check if INT pin is low
  int intState = digitalRead(INT_PIN);
  if (intState == LOW)
    intEverLow = true;

  // Check if threshold exceeded
  bool shouldFire = (absZ_mT > thresholdMT);
  if (shouldFire)
    exceededThreshold = true;

  // Track max Z
  if (absZ_mT > maxZ_mT)
    maxZ_mT = absZ_mT;

  // Print status
  Serial.print(angle);
  Serial.print(F("\t"));
  Serial.print(z_uT, 0);
  Serial.print(F("\t"));
  Serial.print(z_mT, 2);
  Serial.print(F("\t"));
  Serial.print(thresholdMT, 2);
  Serial.print(F("\t"));
  Serial.print(intState == LOW ? F("LOW") : F("HIGH"));
  Serial.print(F("\t"));
  Serial.print(isrCount);
  Serial.print(F("\t"));
  Serial.println(shouldFire ? F("YES") : F("no"));

  // Next angle
  angle += 10;
  if (angle <= 180) {
    servo.write(angle);
    delay(1500);
  } else {
    // Summary
    Serial.println(F("\n=== SUMMARY ==="));
    Serial.print(F("ISR count: "));
    Serial.println(isrCount);
    Serial.print(F("INT ever LOW: "));
    Serial.println(intEverLow ? F("YES") : F("NO"));
    Serial.print(F("Z exceeded threshold: "));
    Serial.println(exceededThreshold ? F("YES") : F("NO"));
    Serial.print(F("Max |Z|: "));
    Serial.print(maxZ_mT, 2);
    Serial.print(F(" mT (threshold: "));
    Serial.print(thresholdMT, 2);
    Serial.println(F(" mT)"));

    // PASS/FAIL
    Serial.println();
    if (isrCount > 0 && exceededThreshold) {
      Serial.println(
          F("*** PASS: Threshold interrupt fired when Z > threshold ***"));
    } else if (exceededThreshold && isrCount == 0) {
      Serial.println(
          F("*** FAIL: Z exceeded threshold but ISR never fired ***"));
    } else if (!exceededThreshold) {
      Serial.println(
          F("*** FAIL: Z never exceeded threshold (move magnet closer) ***"));
    } else {
      Serial.println(F("*** FAIL: Unknown condition ***"));
    }

    // Return servo to start
    servo.write(40);
    done = true;
  }
}
