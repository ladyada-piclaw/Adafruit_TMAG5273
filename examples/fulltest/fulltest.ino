/*!
 * @file fulltest.ino
 * @brief Comprehensive demo of TMAG5273 configuration options
 *
 * This sketch demonstrates all configuration options for the TMAG5273
 * Hall-effect sensor using switch-case to show all enum values.
 * After configuration demo, it continuously reads magnetic field data.
 */

#include <Adafruit_TMAG5273.h>

Adafruit_TMAG5273 tmag;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("\n=== TMAG5273 Full Test ===\n"));

  if (!tmag.begin()) {
    Serial.println(F("Failed to find TMAG5273!"));
    while (1)
      delay(10);
  }

  Serial.print(F("Manufacturer ID: 0x"));
  Serial.println(tmag.getManufacturerID(), HEX);
  Serial.print(F("Device ID: 0x"));
  Serial.println(tmag.getDeviceID(), HEX);

  // === Magnetic Channels Demo ===
  Serial.println(F("\n--- Magnetic Channels ---"));
  tmag5273_mag_ch_en_t channels[] = {
      TMAG5273_MAG_CH_OFF, TMAG5273_MAG_CH_X,   TMAG5273_MAG_CH_Y,
      TMAG5273_MAG_CH_XY,  TMAG5273_MAG_CH_Z,   TMAG5273_MAG_CH_ZX,
      TMAG5273_MAG_CH_YZ,  TMAG5273_MAG_CH_XYZ, TMAG5273_MAG_CH_XYX,
      TMAG5273_MAG_CH_YXY, TMAG5273_MAG_CH_YZY, TMAG5273_MAG_CH_XZX};
  for (uint8_t i = 0; i < 12; i++) {
    tmag.setMagneticChannels(channels[i]);
    tmag5273_mag_ch_en_t ch = tmag.getMagneticChannels();
    Serial.print(F("Set "));
    printMagChannel(ch);
    Serial.print(F(" -> Read: "));
    printMagChannel(ch);
    Serial.println();
  }

  // === Conversion Averaging Demo ===
  Serial.println(F("\n--- Conversion Averaging ---"));
  tmag5273_conv_avg_t avgs[] = {TMAG5273_CONV_AVG_1X,  TMAG5273_CONV_AVG_2X,
                                TMAG5273_CONV_AVG_4X,  TMAG5273_CONV_AVG_8X,
                                TMAG5273_CONV_AVG_16X, TMAG5273_CONV_AVG_32X};
  for (uint8_t i = 0; i < 6; i++) {
    tmag.setConversionAverage(avgs[i]);
    tmag5273_conv_avg_t avg = tmag.getConversionAverage();
    Serial.print(F("Set "));
    printConvAvg(avg);
    Serial.print(F(" -> Read: "));
    printConvAvg(avg);
    Serial.println();
  }

  // === XY Range Demo ===
  Serial.println(F("\n--- XY Range ---"));
  tmag.setXYRange(false);
  Serial.print(F("Set normal -> Read: "));
  Serial.println(tmag.getXYRange() ? F("wide") : F("normal"));
  tmag.setXYRange(true);
  Serial.print(F("Set wide -> Read: "));
  Serial.println(tmag.getXYRange() ? F("wide") : F("normal"));

  // === Z Range Demo ===
  Serial.println(F("\n--- Z Range ---"));
  tmag.setZRange(false);
  Serial.print(F("Set normal -> Read: "));
  Serial.println(tmag.getZRange() ? F("wide") : F("normal"));
  tmag.setZRange(true);
  Serial.print(F("Set wide -> Read: "));
  Serial.println(tmag.getZRange() ? F("wide") : F("normal"));

  // === Operating Mode Demo ===
  Serial.println(F("\n--- Operating Mode ---"));
  tmag5273_operating_mode_t modes[] = {
      TMAG5273_MODE_STANDBY, TMAG5273_MODE_SLEEP, TMAG5273_MODE_CONTINUOUS,
      TMAG5273_MODE_WAKEUP_SLEEP};
  for (uint8_t i = 0; i < 4; i++) {
    tmag.setOperatingMode(modes[i]);
    tmag5273_operating_mode_t mode = tmag.getOperatingMode();
    Serial.print(F("Set "));
    printOpMode(mode);
    Serial.print(F(" -> Read: "));
    printOpMode(mode);
    Serial.println();
  }

  // === Sleep Time Demo ===
  Serial.println(F("\n--- Sleep Time (showing a few values) ---"));
  tmag5273_sleeptime_t sleeps[] = {TMAG5273_SLEEP_1MS, TMAG5273_SLEEP_10MS,
                                   TMAG5273_SLEEP_100MS, TMAG5273_SLEEP_1000MS};
  for (uint8_t i = 0; i < 4; i++) {
    tmag.setSleepTime(sleeps[i]);
    tmag5273_sleeptime_t slp = tmag.getSleepTime();
    Serial.print(F("Set "));
    printSleepTime(slp);
    Serial.print(F(" -> Read: "));
    printSleepTime(slp);
    Serial.println();
  }

  // === Low Noise Mode Demo ===
  Serial.println(F("\n--- Low Noise Mode ---"));
  tmag.setLowNoiseMode(false);
  Serial.print(F("Set disabled -> Read: "));
  Serial.println(tmag.getLowNoiseMode() ? F("enabled") : F("disabled"));
  tmag.setLowNoiseMode(true);
  Serial.print(F("Set enabled -> Read: "));
  Serial.println(tmag.getLowNoiseMode() ? F("enabled") : F("disabled"));

  // === Temperature Compensation Demo ===
  Serial.println(F("\n--- Temperature Compensation ---"));
  tmag5273_mag_tempco_t tempcos[] = {
      TMAG5273_TEMPCO_NONE, TMAG5273_TEMPCO_NDFEB, TMAG5273_TEMPCO_CERAMIC};
  for (uint8_t i = 0; i < 3; i++) {
    tmag.setMagTempComp(tempcos[i]);
    tmag5273_mag_tempco_t tc = tmag.getMagTempComp();
    Serial.print(F("Set "));
    printTempComp(tc);
    Serial.print(F(" -> Read: "));
    printTempComp(tc);
    Serial.println();
  }

  // === Angle Calculation Demo ===
  Serial.println(F("\n--- Angle Calculation ---"));
  tmag5273_angle_en_t angles[] = {TMAG5273_ANGLE_OFF, TMAG5273_ANGLE_XY,
                                  TMAG5273_ANGLE_YZ, TMAG5273_ANGLE_XZ};
  for (uint8_t i = 0; i < 4; i++) {
    tmag.setAngleCalculation(angles[i]);
    tmag5273_angle_en_t ang = tmag.getAngleCalculation();
    Serial.print(F("Set "));
    printAngleCalc(ang);
    Serial.print(F(" -> Read: "));
    printAngleCalc(ang);
    Serial.println();
  }

  // === Interrupt Mode Demo ===
  Serial.println(F("\n--- Interrupt Mode ---"));
  tmag5273_int_mode_t intmodes[] = {
      TMAG5273_INT_NONE, TMAG5273_INT_THROUGH_INT, TMAG5273_INT_INT_EXCEPT_I2C,
      TMAG5273_INT_THROUGH_SCL, TMAG5273_INT_SCL_EXCEPT_I2C};
  for (uint8_t i = 0; i < 5; i++) {
    tmag.setInterruptMode(intmodes[i]);
    tmag5273_int_mode_t im = tmag.getInterruptMode();
    Serial.print(F("Set "));
    printIntMode(im);
    Serial.print(F(" -> Read: "));
    printIntMode(im);
    Serial.println();
  }

  // === CRC Enable Demo ===
  Serial.println(F("\n--- CRC ---"));
  tmag.enableCRC(false);
  Serial.print(F("Set disabled -> Read: "));
  Serial.println(tmag.crcEnabled() ? F("enabled") : F("disabled"));
  tmag.enableCRC(true);
  Serial.print(F("Set enabled -> Read: "));
  Serial.println(tmag.crcEnabled() ? F("enabled") : F("disabled"));
  tmag.enableCRC(false); // Disable for normal operation

  // === Configure for continuous reading ===
  Serial.println(F("\n--- Final Configuration ---"));
  tmag.setMagneticChannels(TMAG5273_MAG_CH_XYZ);
  tmag.setConversionAverage(TMAG5273_CONV_AVG_32X);
  tmag.setOperatingMode(TMAG5273_MODE_CONTINUOUS);
  tmag.enableTemperature(true);
  tmag.setAngleCalculation(TMAG5273_ANGLE_XY);
  Serial.println(
      F("Set: XYZ channels, 32x avg, continuous, temp on, angle XY"));

  Serial.println(F("\n--- Starting continuous readings ---\n"));
}

void loop() {
  float x = tmag.readMagneticX();
  float y = tmag.readMagneticY();
  float z = tmag.readMagneticZ();
  float temp = tmag.getTemperature();
  float angle = tmag.readAngle();
  uint8_t mag = tmag.readMagnitude();

  Serial.print(F("X: "));
  Serial.print(x, 2);
  Serial.print(F(" uT  Y: "));
  Serial.print(y, 2);
  Serial.print(F(" uT  Z: "));
  Serial.print(z, 2);
  Serial.print(F(" uT  Temp: "));
  Serial.print(temp, 1);
  Serial.print(F(" C  Angle: "));
  Serial.print(angle, 1);
  Serial.print(F(" deg  Mag: "));
  Serial.println(mag);

  delay(100);
}

// === Helper functions to print enum values ===

void printMagChannel(tmag5273_mag_ch_en_t ch) {
  switch (ch) {
    case TMAG5273_MAG_CH_OFF:
      Serial.print(F("OFF"));
      break;
    case TMAG5273_MAG_CH_X:
      Serial.print(F("X"));
      break;
    case TMAG5273_MAG_CH_Y:
      Serial.print(F("Y"));
      break;
    case TMAG5273_MAG_CH_XY:
      Serial.print(F("XY"));
      break;
    case TMAG5273_MAG_CH_Z:
      Serial.print(F("Z"));
      break;
    case TMAG5273_MAG_CH_ZX:
      Serial.print(F("ZX"));
      break;
    case TMAG5273_MAG_CH_YZ:
      Serial.print(F("YZ"));
      break;
    case TMAG5273_MAG_CH_XYZ:
      Serial.print(F("XYZ"));
      break;
    case TMAG5273_MAG_CH_XYX:
      Serial.print(F("XYX"));
      break;
    case TMAG5273_MAG_CH_YXY:
      Serial.print(F("YXY"));
      break;
    case TMAG5273_MAG_CH_YZY:
      Serial.print(F("YZY"));
      break;
    case TMAG5273_MAG_CH_XZX:
      Serial.print(F("XZX"));
      break;
  }
}

void printConvAvg(tmag5273_conv_avg_t avg) {
  switch (avg) {
    case TMAG5273_CONV_AVG_1X:
      Serial.print(F("1X"));
      break;
    case TMAG5273_CONV_AVG_2X:
      Serial.print(F("2X"));
      break;
    case TMAG5273_CONV_AVG_4X:
      Serial.print(F("4X"));
      break;
    case TMAG5273_CONV_AVG_8X:
      Serial.print(F("8X"));
      break;
    case TMAG5273_CONV_AVG_16X:
      Serial.print(F("16X"));
      break;
    case TMAG5273_CONV_AVG_32X:
      Serial.print(F("32X"));
      break;
  }
}

void printOpMode(tmag5273_operating_mode_t mode) {
  switch (mode) {
    case TMAG5273_MODE_STANDBY:
      Serial.print(F("STANDBY"));
      break;
    case TMAG5273_MODE_SLEEP:
      Serial.print(F("SLEEP"));
      break;
    case TMAG5273_MODE_CONTINUOUS:
      Serial.print(F("CONTINUOUS"));
      break;
    case TMAG5273_MODE_WAKEUP_SLEEP:
      Serial.print(F("WAKEUP_SLEEP"));
      break;
  }
}

void printSleepTime(tmag5273_sleeptime_t slp) {
  switch (slp) {
    case TMAG5273_SLEEP_1MS:
      Serial.print(F("1ms"));
      break;
    case TMAG5273_SLEEP_5MS:
      Serial.print(F("5ms"));
      break;
    case TMAG5273_SLEEP_10MS:
      Serial.print(F("10ms"));
      break;
    case TMAG5273_SLEEP_15MS:
      Serial.print(F("15ms"));
      break;
    case TMAG5273_SLEEP_20MS:
      Serial.print(F("20ms"));
      break;
    case TMAG5273_SLEEP_30MS:
      Serial.print(F("30ms"));
      break;
    case TMAG5273_SLEEP_50MS:
      Serial.print(F("50ms"));
      break;
    case TMAG5273_SLEEP_100MS:
      Serial.print(F("100ms"));
      break;
    case TMAG5273_SLEEP_500MS:
      Serial.print(F("500ms"));
      break;
    case TMAG5273_SLEEP_1000MS:
      Serial.print(F("1000ms"));
      break;
    case TMAG5273_SLEEP_2000MS:
      Serial.print(F("2000ms"));
      break;
    case TMAG5273_SLEEP_5000MS:
      Serial.print(F("5000ms"));
      break;
    case TMAG5273_SLEEP_20000MS:
      Serial.print(F("20000ms"));
      break;
  }
}

void printTempComp(tmag5273_mag_tempco_t tc) {
  switch (tc) {
    case TMAG5273_TEMPCO_NONE:
      Serial.print(F("NONE"));
      break;
    case TMAG5273_TEMPCO_NDFEB:
      Serial.print(F("NdFeB"));
      break;
    case TMAG5273_TEMPCO_CERAMIC:
      Serial.print(F("CERAMIC"));
      break;
  }
}

void printAngleCalc(tmag5273_angle_en_t ang) {
  switch (ang) {
    case TMAG5273_ANGLE_OFF:
      Serial.print(F("OFF"));
      break;
    case TMAG5273_ANGLE_XY:
      Serial.print(F("XY"));
      break;
    case TMAG5273_ANGLE_YZ:
      Serial.print(F("YZ"));
      break;
    case TMAG5273_ANGLE_XZ:
      Serial.print(F("XZ"));
      break;
  }
}

void printIntMode(tmag5273_int_mode_t im) {
  switch (im) {
    case TMAG5273_INT_NONE:
      Serial.print(F("NONE"));
      break;
    case TMAG5273_INT_THROUGH_INT:
      Serial.print(F("INT_PIN"));
      break;
    case TMAG5273_INT_INT_EXCEPT_I2C:
      Serial.print(F("INT_EXCEPT_I2C"));
      break;
    case TMAG5273_INT_THROUGH_SCL:
      Serial.print(F("SCL"));
      break;
    case TMAG5273_INT_SCL_EXCEPT_I2C:
      Serial.print(F("SCL_EXCEPT_I2C"));
      break;
  }
}
