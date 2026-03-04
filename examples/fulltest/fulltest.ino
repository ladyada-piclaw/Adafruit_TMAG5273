/*!
 * @file fulltest.ino
 * @brief Configurable demo of TMAG5273 3-axis Hall-effect sensor
 *
 * Demonstrates all configuration options — edit the values below to
 * customize behavior. After setup, continuously prints magnetic field,
 * temperature, angle, and magnitude readings.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 * MIT license
 */

#include <Adafruit_TMAG5273.h>

Adafruit_TMAG5273 tmag;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("\n=== TMAG5273 Full Test ===\n"));

  // === Initialization ===
  Serial.println(F("--- Initialization ---"));
  if (!tmag.begin()) {
    Serial.println(F("Failed to find TMAG5273!"));
    while (1)
      delay(10);
  }
  Serial.print(F("Manufacturer ID: 0x"));
  Serial.println(tmag.getManufacturerID(), HEX);
  Serial.print(F("Device ID: 0x"));
  Serial.println(tmag.getDeviceID(), HEX);
  bool is_x2 = (tmag.getDeviceID() & 0x03) == 0x02;
  Serial.print(F("Variant: "));
  Serial.println(is_x2 ? F("x2 (+/-133/266 mT)") : F("x1 (+/-40/80 mT)"));

  // === Magnetic Channels ===
  Serial.println(F("\n--- Magnetic Channels ---"));
  tmag.setMagneticChannels(TMAG5273_MAG_CH_XYZ);
  // Options: TMAG5273_MAG_CH_OFF, _X, _Y, _XY, _Z, _ZX, _YZ, _XYZ
  //          _XYX, _YXY, _YZY, _XZX (pseudo-simultaneous modes)
  Serial.print(F("Channels: "));
  switch (tmag.getMagneticChannels()) {
    case TMAG5273_MAG_CH_OFF:
      Serial.println(F("OFF"));
      break;
    case TMAG5273_MAG_CH_X:
      Serial.println(F("X"));
      break;
    case TMAG5273_MAG_CH_Y:
      Serial.println(F("Y"));
      break;
    case TMAG5273_MAG_CH_XY:
      Serial.println(F("XY"));
      break;
    case TMAG5273_MAG_CH_Z:
      Serial.println(F("Z"));
      break;
    case TMAG5273_MAG_CH_ZX:
      Serial.println(F("ZX"));
      break;
    case TMAG5273_MAG_CH_YZ:
      Serial.println(F("YZ"));
      break;
    case TMAG5273_MAG_CH_XYZ:
      Serial.println(F("XYZ"));
      break;
    case TMAG5273_MAG_CH_XYX:
      Serial.println(F("XYX"));
      break;
    case TMAG5273_MAG_CH_YXY:
      Serial.println(F("YXY"));
      break;
    case TMAG5273_MAG_CH_YZY:
      Serial.println(F("YZY"));
      break;
    case TMAG5273_MAG_CH_XZX:
      Serial.println(F("XZX"));
      break;
  }

  // === Conversion Averaging ===
  Serial.println(F("\n--- Conversion Averaging ---"));
  tmag.setConversionAverage(TMAG5273_CONV_AVG_32X);
  // Options: TMAG5273_CONV_AVG_1X, _2X, _4X, _8X, _16X, _32X
  Serial.print(F("Averaging: "));
  switch (tmag.getConversionAverage()) {
    case TMAG5273_CONV_AVG_1X:
      Serial.println(F("1X"));
      break;
    case TMAG5273_CONV_AVG_2X:
      Serial.println(F("2X"));
      break;
    case TMAG5273_CONV_AVG_4X:
      Serial.println(F("4X"));
      break;
    case TMAG5273_CONV_AVG_8X:
      Serial.println(F("8X"));
      break;
    case TMAG5273_CONV_AVG_16X:
      Serial.println(F("16X"));
      break;
    case TMAG5273_CONV_AVG_32X:
      Serial.println(F("32X"));
      break;
  }

  // === XY Range ===
  Serial.println(F("\n--- XY Range ---"));
  tmag.setXYRangeWide(false);
  // false = normal (+/-40mT for x1 / +/-133mT for x2)
  // true = wide (+/-80mT for x1 / +/-266mT for x2)
  Serial.print(F("XY Range: "));
  if (is_x2) {
    Serial.println(tmag.getXYRangeWide() ? F("+/-266 mT") : F("+/-133 mT"));
  } else {
    Serial.println(tmag.getXYRangeWide() ? F("+/-80 mT") : F("+/-40 mT"));
  }

  // === Z Range ===
  Serial.println(F("\n--- Z Range ---"));
  tmag.setZRangeWide(false);
  // false = normal (+/-40mT for x1 / +/-133mT for x2)
  // true = wide (+/-80mT for x1 / +/-266mT for x2)
  Serial.print(F("Z Range: "));
  if (is_x2) {
    Serial.println(tmag.getZRangeWide() ? F("+/-266 mT") : F("+/-133 mT"));
  } else {
    Serial.println(tmag.getZRangeWide() ? F("+/-80 mT") : F("+/-40 mT"));
  }

  // === Operating Mode ===
  Serial.println(F("\n--- Operating Mode ---"));
  tmag.setOperatingMode(TMAG5273_MODE_CONTINUOUS);
  // Options: TMAG5273_MODE_STANDBY (trigger mode)
  //          TMAG5273_MODE_SLEEP (ultra-low power)
  //          TMAG5273_MODE_CONTINUOUS (continuous sampling)
  //          TMAG5273_MODE_WAKEUP_SLEEP (periodic wake/sleep)
  Serial.print(F("Operating Mode: "));
  switch (tmag.getOperatingMode()) {
    case TMAG5273_MODE_STANDBY:
      Serial.println(F("Standby"));
      break;
    case TMAG5273_MODE_SLEEP:
      Serial.println(F("Sleep"));
      break;
    case TMAG5273_MODE_CONTINUOUS:
      Serial.println(F("Continuous"));
      break;
    case TMAG5273_MODE_WAKEUP_SLEEP:
      Serial.println(F("Wakeup/Sleep"));
      break;
  }

  // === Low Noise Mode ===
  Serial.println(F("\n--- Low Noise Mode ---"));
  tmag.setLowNoiseMode(true);
  // true = lower noise, higher power consumption
  // false = default noise/power
  Serial.print(F("Low Noise: "));
  Serial.println(tmag.getLowNoiseMode() ? F("Enabled") : F("Disabled"));

  // === Temperature Channel ===
  Serial.println(F("\n--- Temperature ---"));
  tmag.enableTemperature(true);
  // true = enable temperature measurement
  // false = disable temperature measurement
  Serial.print(F("Temperature: "));
  Serial.println(tmag.temperatureEnabled() ? F("Enabled") : F("Disabled"));

  // === Temperature Compensation ===
  Serial.println(F("\n--- Temperature Compensation ---"));
  tmag.setMagTempComp(TMAG5273_TEMPCO_NDFEB);
  // Options: TMAG5273_TEMPCO_NONE (no compensation)
  //          TMAG5273_TEMPCO_NDFEB (0.12%/C, rare earth magnets)
  //          TMAG5273_TEMPCO_CERAMIC (0.2%/C, ceramic/ferrite magnets)
  Serial.print(F("Temp Compensation: "));
  switch (tmag.getMagTempComp()) {
    case TMAG5273_TEMPCO_NONE:
      Serial.println(F("None"));
      break;
    case TMAG5273_TEMPCO_NDFEB:
      Serial.println(F("NdFeB"));
      break;
    case TMAG5273_TEMPCO_CERAMIC:
      Serial.println(F("Ceramic"));
      break;
  }

  // === Angle Calculation ===
  Serial.println(F("\n--- Angle Calculation ---"));
  tmag.setAngleCalculation(TMAG5273_ANGLE_XY);
  // Options: TMAG5273_ANGLE_OFF (no angle calculation)
  //          TMAG5273_ANGLE_XY (X first, Y second)
  //          TMAG5273_ANGLE_YZ (Y first, Z second)
  //          TMAG5273_ANGLE_XZ (X first, Z second)
  Serial.print(F("Angle Calculation: "));
  switch (tmag.getAngleCalculation()) {
    case TMAG5273_ANGLE_OFF:
      Serial.println(F("Off"));
      break;
    case TMAG5273_ANGLE_XY:
      Serial.println(F("XY"));
      break;
    case TMAG5273_ANGLE_YZ:
      Serial.println(F("YZ"));
      break;
    case TMAG5273_ANGLE_XZ:
      Serial.println(F("XZ"));
      break;
  }

  // === Interrupt Mode ===
  Serial.println(F("\n--- Interrupt Mode ---"));
  tmag.setInterruptMode(TMAG5273_INT_THROUGH_INT);
  // Options: TMAG5273_INT_NONE (no interrupt)
  //          TMAG5273_INT_THROUGH_INT (INT pin)
  //          TMAG5273_INT_INT_EXCEPT_I2C (INT except during I2C)
  //          TMAG5273_INT_THROUGH_SCL (SCL pin)
  //          TMAG5273_INT_SCL_EXCEPT_I2C (SCL except during I2C)
  // Also available: enableThresholdInterrupt() for magnetic threshold alerts

  tmag.enableResultInterrupt(true);
  // true = INT pin asserts on conversion complete (data ready)
  // false = no data ready interrupt
  Serial.print(F("Data Ready INT: "));
  Serial.println(F("Enabled"));

  Serial.print(F("Interrupt Mode: "));
  switch (tmag.getInterruptMode()) {
    case TMAG5273_INT_NONE:
      Serial.println(F("None"));
      break;
    case TMAG5273_INT_THROUGH_INT:
      Serial.println(F("INT pin"));
      break;
    case TMAG5273_INT_INT_EXCEPT_I2C:
      Serial.println(F("INT (not I2C)"));
      break;
    case TMAG5273_INT_THROUGH_SCL:
      Serial.println(F("SCL"));
      break;
    case TMAG5273_INT_SCL_EXCEPT_I2C:
      Serial.println(F("SCL (not I2C)"));
      break;
  }

  Serial.println(F("\n--- Starting Continuous Readings ---\n"));
}

void loop() {
  float x = tmag.readMagneticX();
  float y = tmag.readMagneticY();
  float z = tmag.readMagneticZ();
  float temp = tmag.getTemperature();
  float angle = tmag.readAngle();
  float mag = tmag.readMagnitudeMT();

  Serial.print(F("X: "));
  Serial.print(x, 1);
  Serial.print(F(" uT\tY: "));
  Serial.print(y, 1);
  Serial.print(F(" uT\tZ: "));
  Serial.print(z, 1);
  Serial.print(F(" uT\tT: "));
  Serial.print(temp, 1);
  Serial.print(F(" C\tAng: "));
  Serial.print(angle, 1);
  Serial.print(F(" deg\tMag: "));
  Serial.print(mag, 1);
  Serial.println(F(" mT"));

  delay(100);
}
