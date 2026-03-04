/*!
 * @file Adafruit_TMAG5273.cpp
 *
 * @mainpage Adafruit TMAG5273 3-axis Hall-effect sensor library
 *
 * @section intro_sec Introduction
 *
 * This is a library for the TI TMAG5273 low-power linear 3D Hall-effect
 * magnetic sensor with I2C interface. Supports all TMAG5273 variants.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * MIT license, all text above must be included in any redistribution
 */

#include "Adafruit_TMAG5273.h"

// Temperature conversion constants from datasheet
#define TMAG5273_TEMP_ADC_T0 17508 ///< ADC code at 25°C reference
#define TMAG5273_TEMP_SENS_T0 25.0 ///< Reference temperature in °C
#define TMAG5273_TEMP_ADC_RES 60.1 ///< Temperature ADC resolution (LSB/°C)

/*!
 * @brief Constructor for the TMAG5273 sensor class
 */
Adafruit_TMAG5273::Adafruit_TMAG5273() {
  i2c_dev = NULL;
  _is_x2_variant = true; // Default, will be detected in begin()
  _sensorID = 5273;
  _range_xy = 133.0;
  _range_z = 133.0;
}

/*!
 * @brief Destructor for the TMAG5273 sensor class
 */
Adafruit_TMAG5273::~Adafruit_TMAG5273() {
  if (i2c_dev) {
    delete i2c_dev;
  }
}

/*!
 * @brief Initialize the sensor
 * @param addr I2C address (default 0x35)
 * @param theWire Pointer to Wire instance
 * @return True if initialization successful
 */
bool Adafruit_TMAG5273::begin(uint8_t addr, TwoWire* theWire) {
  if (i2c_dev) {
    delete i2c_dev;
  }
  i2c_dev = new Adafruit_I2CDevice(addr, theWire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // Wait for power-up (270us typical)
  delay(1);

  // Verify manufacturer ID (should be 0x5449 = "TI")
  if (getManufacturerID() != 0x5449) {
    return false;
  }

  // Detect variant from DEVICE_ID register bits[1:0]
  uint8_t ver = getDeviceID() & 0x03;
  _is_x2_variant = (ver == 0x02);

  // Set default ranges based on variant
  if (_is_x2_variant) {
    _range_xy = 133.0;
    _range_z = 133.0;
  } else {
    _range_xy = 40.0;
    _range_z = 40.0;
  }

  // Default configuration: max performance, all features enabled
  if (!setMagneticChannels(TMAG5273_MAG_CH_XYZ)) {
    return false;
  }
  if (!setConversionAverage(TMAG5273_CONV_AVG_32X)) {
    return false;
  }
  if (!setOperatingMode(TMAG5273_MODE_CONTINUOUS)) {
    return false;
  }
  if (!enableTemperature(true)) {
    return false;
  }
  if (!setLowNoiseMode(true)) {
    return false;
  }
  if (!setMagTempComp(TMAG5273_TEMPCO_NDFEB)) {
    return false;
  }
  if (!setAngleCalculation(TMAG5273_ANGLE_XY)) {
    return false;
  }
  if (!setInterruptMode(TMAG5273_INT_THROUGH_INT)) {
    return false;
  }

  return true;
}

/*!
 * @brief Get the manufacturer ID
 * @return Manufacturer ID (0x5449 = "TI")
 */
uint16_t Adafruit_TMAG5273::getManufacturerID() {
  Adafruit_BusIO_Register reg_lsb =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MANUFACTURER_ID_LSB, 1);
  Adafruit_BusIO_Register reg_msb =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MANUFACTURER_ID_MSB, 1);

  uint8_t lsb = reg_lsb.read();
  uint8_t msb = reg_msb.read();
  return ((uint16_t)msb << 8) | lsb;
}

/*!
 * @brief Get the device ID
 * @return Device ID (version bits: 1=x1 variant, 2=x2 variant)
 */
uint8_t Adafruit_TMAG5273::getDeviceID() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_ID, 1);
  return reg.read();
}

/*!
 * @brief Set which magnetic channels are enabled
 * @param channels Channel configuration
 * @return True on success
 */
bool Adafruit_TMAG5273::setMagneticChannels(tmag5273_mag_ch_en_t channels) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits mag_ch_en =
      Adafruit_BusIO_RegisterBits(&reg, 4, 4);
  return mag_ch_en.write(channels);
}

/*!
 * @brief Get which magnetic channels are enabled
 * @return Current channel configuration
 */
tmag5273_mag_ch_en_t Adafruit_TMAG5273::getMagneticChannels() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits mag_ch_en =
      Adafruit_BusIO_RegisterBits(&reg, 4, 4);
  return (tmag5273_mag_ch_en_t)mag_ch_en.read();
}

/*!
 * @brief Set the XY axis range
 * @param wide True for wide range (80mT/266mT), false for narrow (40mT/133mT)
 * @return True on success
 */
bool Adafruit_TMAG5273::setXYRange(bool wide) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits x_y_range =
      Adafruit_BusIO_RegisterBits(&reg, 1, 1);

  if (!x_y_range.write(wide ? 1 : 0)) {
    return false;
  }

  // Update stored range
  if (_is_x2_variant) {
    _range_xy = wide ? 266.0 : 133.0;
  } else {
    _range_xy = wide ? 80.0 : 40.0;
  }
  return true;
}

/*!
 * @brief Get the XY axis range setting
 * @return True if wide range
 */
bool Adafruit_TMAG5273::getXYRange() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits x_y_range =
      Adafruit_BusIO_RegisterBits(&reg, 1, 1);
  return x_y_range.read() == 1;
}

/*!
 * @brief Set the Z axis range
 * @param wide True for wide range (80mT/266mT), false for narrow (40mT/133mT)
 * @return True on success
 */
bool Adafruit_TMAG5273::setZRange(bool wide) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits z_range = Adafruit_BusIO_RegisterBits(&reg, 1, 0);

  if (!z_range.write(wide ? 1 : 0)) {
    return false;
  }

  // Update stored range
  if (_is_x2_variant) {
    _range_z = wide ? 266.0 : 133.0;
  } else {
    _range_z = wide ? 80.0 : 40.0;
  }
  return true;
}

/*!
 * @brief Get the Z axis range setting
 * @return True if wide range
 */
bool Adafruit_TMAG5273::getZRange() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits z_range = Adafruit_BusIO_RegisterBits(&reg, 1, 0);
  return z_range.read() == 1;
}

/*!
 * @brief Set the conversion averaging
 * @param avg Averaging setting (1x to 32x)
 * @return True on success
 */
bool Adafruit_TMAG5273::setConversionAverage(tmag5273_conv_avg_t avg) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits conv_avg =
      Adafruit_BusIO_RegisterBits(&reg, 3, 2);
  return conv_avg.write(avg);
}

/*!
 * @brief Get the conversion averaging setting
 * @return Current averaging setting
 */
tmag5273_conv_avg_t Adafruit_TMAG5273::getConversionAverage() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits conv_avg =
      Adafruit_BusIO_RegisterBits(&reg, 3, 2);
  return (tmag5273_conv_avg_t)conv_avg.read();
}

/*!
 * @brief Set the operating mode
 * @param mode Operating mode
 * @return True on success
 */
bool Adafruit_TMAG5273::setOperatingMode(tmag5273_operating_mode_t mode) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits operating_mode =
      Adafruit_BusIO_RegisterBits(&reg, 2, 0);
  return operating_mode.write(mode);
}

/*!
 * @brief Get the operating mode
 * @return Current operating mode
 */
tmag5273_operating_mode_t Adafruit_TMAG5273::getOperatingMode() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits operating_mode =
      Adafruit_BusIO_RegisterBits(&reg, 2, 0);
  return (tmag5273_operating_mode_t)operating_mode.read();
}

/*!
 * @brief Set the sleep time for wake-up and sleep mode
 * @param time Sleep time setting
 * @return True on success
 */
bool Adafruit_TMAG5273::setSleepTime(tmag5273_sleeptime_t time) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits sleeptime =
      Adafruit_BusIO_RegisterBits(&reg, 4, 0);
  return sleeptime.write(time);
}

/*!
 * @brief Get the sleep time setting
 * @return Current sleep time
 */
tmag5273_sleeptime_t Adafruit_TMAG5273::getSleepTime() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits sleeptime =
      Adafruit_BusIO_RegisterBits(&reg, 4, 0);
  return (tmag5273_sleeptime_t)sleeptime.read();
}

/*!
 * @brief Set low noise mode
 * @param enable True for low noise, false for low power
 * @return True on success
 */
bool Adafruit_TMAG5273::setLowNoiseMode(bool enable) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits lp_ln = Adafruit_BusIO_RegisterBits(&reg, 1, 4);
  return lp_ln.write(enable ? 1 : 0);
}

/*!
 * @brief Get low noise mode setting
 * @return True if low noise mode enabled
 */
bool Adafruit_TMAG5273::getLowNoiseMode() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits lp_ln = Adafruit_BusIO_RegisterBits(&reg, 1, 4);
  return lp_ln.read() == 1;
}

/*!
 * @brief Set the magnet temperature compensation
 * @param tempco Temperature coefficient setting
 * @return True on success
 */
bool Adafruit_TMAG5273::setMagTempComp(tmag5273_mag_tempco_t tempco) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits mag_tempco =
      Adafruit_BusIO_RegisterBits(&reg, 2, 5);
  return mag_tempco.write(tempco);
}

/*!
 * @brief Get the magnet temperature compensation setting
 * @return Current temperature compensation
 */
tmag5273_mag_tempco_t Adafruit_TMAG5273::getMagTempComp() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits mag_tempco =
      Adafruit_BusIO_RegisterBits(&reg, 2, 5);
  return (tmag5273_mag_tempco_t)mag_tempco.read();
}

/*!
 * @brief Set the angle calculation axis pair
 * @param angle Angle calculation setting
 * @return True on success
 */
bool Adafruit_TMAG5273::setAngleCalculation(tmag5273_angle_en_t angle) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits angle_en =
      Adafruit_BusIO_RegisterBits(&reg, 2, 2);
  return angle_en.write(angle);
}

/*!
 * @brief Get the angle calculation axis pair
 * @return Current angle calculation setting
 */
tmag5273_angle_en_t Adafruit_TMAG5273::getAngleCalculation() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_SENSOR_CONFIG_2, 1);
  Adafruit_BusIO_RegisterBits angle_en =
      Adafruit_BusIO_RegisterBits(&reg, 2, 2);
  return (tmag5273_angle_en_t)angle_en.read();
}

/*!
 * @brief Enable or disable the temperature channel
 * @param enable True to enable temperature measurement
 * @return True on success
 */
bool Adafruit_TMAG5273::enableTemperature(bool enable) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_T_CONFIG, 1);
  Adafruit_BusIO_RegisterBits t_ch_en = Adafruit_BusIO_RegisterBits(&reg, 1, 0);
  return t_ch_en.write(enable ? 1 : 0);
}

/*!
 * @brief Check if temperature channel is enabled
 * @return True if enabled
 */
bool Adafruit_TMAG5273::temperatureEnabled() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_T_CONFIG, 1);
  Adafruit_BusIO_RegisterBits t_ch_en = Adafruit_BusIO_RegisterBits(&reg, 1, 0);
  return t_ch_en.read() == 1;
}

/*!
 * @brief Read a 16-bit result from two consecutive registers
 * @param msb_reg Address of MSB register
 * @return 16-bit signed value
 */
int16_t Adafruit_TMAG5273::read16BitResult(uint8_t msb_reg) {
  uint8_t buffer[2];
  buffer[0] = msb_reg;
  i2c_dev->write_then_read(buffer, 1, buffer, 2);
  return ((int16_t)buffer[0] << 8) | buffer[1];
}

/*!
 * @brief Read raw X-axis value
 * @return Raw 16-bit signed value
 */
int16_t Adafruit_TMAG5273::readX() {
  return read16BitResult(TMAG5273_REG_X_MSB_RESULT);
}

/*!
 * @brief Read raw Y-axis value
 * @return Raw 16-bit signed value
 */
int16_t Adafruit_TMAG5273::readY() {
  return read16BitResult(TMAG5273_REG_Y_MSB_RESULT);
}

/*!
 * @brief Read raw Z-axis value
 * @return Raw 16-bit signed value
 */
int16_t Adafruit_TMAG5273::readZ() {
  return read16BitResult(TMAG5273_REG_Z_MSB_RESULT);
}

/*!
 * @brief Convert raw value to microTesla
 * @param raw Raw 16-bit value
 * @param range_mT Range in milliTesla
 * @return Value in microTesla
 */
float Adafruit_TMAG5273::rawToMicroTesla(int16_t raw, float range_mT) {
  // Formula: B(mT) = (raw / 32768) * range
  // Then multiply by 1000 to get uT
  return (raw / 32768.0) * range_mT * 1000.0;
}

/*!
 * @brief Read X-axis magnetic field in microTesla
 * @return Magnetic field in uT
 */
float Adafruit_TMAG5273::readMagneticX() {
  return rawToMicroTesla(readX(), _range_xy);
}

/*!
 * @brief Read Y-axis magnetic field in microTesla
 * @return Magnetic field in uT
 */
float Adafruit_TMAG5273::readMagneticY() {
  return rawToMicroTesla(readY(), _range_xy);
}

/*!
 * @brief Read Z-axis magnetic field in microTesla
 * @return Magnetic field in uT
 */
float Adafruit_TMAG5273::readMagneticZ() {
  return rawToMicroTesla(readZ(), _range_z);
}

/*!
 * @brief Get the temperature reading
 * @return Temperature in degrees Celsius
 */
float Adafruit_TMAG5273::getTemperature() {
  int16_t raw = read16BitResult(TMAG5273_REG_T_MSB_RESULT);
  // Formula: T = 25.0 + (raw - 17508) / 60.1
  return TMAG5273_TEMP_SENS_T0 +
         ((raw - TMAG5273_TEMP_ADC_T0) / TMAG5273_TEMP_ADC_RES);
}

/*!
 * @brief Read the calculated angle
 * @return Angle in degrees (0-360)
 */
float Adafruit_TMAG5273::readAngle() {
  uint8_t buffer[2];
  buffer[0] = TMAG5273_REG_ANGLE_RESULT_MSB;
  i2c_dev->write_then_read(buffer, 1, buffer, 2);

  // MSB contains bits[12:5], LSB contains bits[4:0] in upper bits
  // The angle is 13 bits: 9-bit integer + 4-bit fraction
  uint16_t raw = ((uint16_t)buffer[0] << 4) | (buffer[1] >> 4);

  // Convert to degrees: raw * (360 / 8192)
  return raw * (360.0 / 8192.0);
}

/*!
 * @brief Read the calculated magnitude
 * @return 8-bit magnitude value
 */
uint8_t Adafruit_TMAG5273::readMagnitude() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MAGNITUDE_RESULT, 1);
  return reg.read();
}

/*!
 * @brief Enable or disable CRC
 * @param enable True to enable CRC
 * @return True on success
 */
bool Adafruit_TMAG5273::enableCRC(bool enable) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits crc_en = Adafruit_BusIO_RegisterBits(&reg, 1, 7);
  return crc_en.write(enable ? 1 : 0);
}

/*!
 * @brief Check if CRC is enabled
 * @return True if CRC enabled
 */
bool Adafruit_TMAG5273::crcEnabled() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits crc_en = Adafruit_BusIO_RegisterBits(&reg, 1, 7);
  return crc_en.read() == 1;
}

/*!
 * @brief Set the interrupt mode
 * @param mode Interrupt mode setting
 * @return True on success
 */
bool Adafruit_TMAG5273::setInterruptMode(tmag5273_int_mode_t mode) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_INT_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits int_mode =
      Adafruit_BusIO_RegisterBits(&reg, 3, 2);
  return int_mode.write(mode);
}

/*!
 * @brief Get the interrupt mode
 * @return Current interrupt mode
 */
tmag5273_int_mode_t Adafruit_TMAG5273::getInterruptMode() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_INT_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits int_mode =
      Adafruit_BusIO_RegisterBits(&reg, 3, 2);
  return (tmag5273_int_mode_t)int_mode.read();
}

/*!
 * @brief Enable or disable result interrupt
 * @param enable True to enable
 * @return True on success
 */
bool Adafruit_TMAG5273::enableResultInterrupt(bool enable) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_INT_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits rslt_int =
      Adafruit_BusIO_RegisterBits(&reg, 1, 7);
  return rslt_int.write(enable ? 1 : 0);
}

/*!
 * @brief Enable or disable threshold interrupt
 * @param enable True to enable
 * @return True on success
 */
bool Adafruit_TMAG5273::enableThresholdInterrupt(bool enable) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_INT_CONFIG_1, 1);
  Adafruit_BusIO_RegisterBits thrsld_int =
      Adafruit_BusIO_RegisterBits(&reg, 1, 6);
  return thrsld_int.write(enable ? 1 : 0);
}

/*!
 * @brief Set X-axis threshold
 * @param threshold 8-bit signed threshold value
 * @return True on success
 */
bool Adafruit_TMAG5273::setXThreshold(int8_t threshold) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_X_THR_CONFIG, 1);
  return reg.write((uint8_t)threshold);
}

/*!
 * @brief Get X-axis threshold
 * @return Current X threshold
 */
int8_t Adafruit_TMAG5273::getXThreshold() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_X_THR_CONFIG, 1);
  return (int8_t)reg.read();
}

/*!
 * @brief Set Y-axis threshold
 * @param threshold 8-bit signed threshold value
 * @return True on success
 */
bool Adafruit_TMAG5273::setYThreshold(int8_t threshold) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_Y_THR_CONFIG, 1);
  return reg.write((uint8_t)threshold);
}

/*!
 * @brief Get Y-axis threshold
 * @return Current Y threshold
 */
int8_t Adafruit_TMAG5273::getYThreshold() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_Y_THR_CONFIG, 1);
  return (int8_t)reg.read();
}

/*!
 * @brief Set Z-axis threshold
 * @param threshold 8-bit signed threshold value
 * @return True on success
 */
bool Adafruit_TMAG5273::setZThreshold(int8_t threshold) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_Z_THR_CONFIG, 1);
  return reg.write((uint8_t)threshold);
}

/*!
 * @brief Get Z-axis threshold
 * @return Current Z threshold
 */
int8_t Adafruit_TMAG5273::getZThreshold() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_Z_THR_CONFIG, 1);
  return (int8_t)reg.read();
}

/*!
 * @brief Set temperature threshold
 * @param threshold 7-bit threshold (8 degrees C per LSB)
 * @return True on success
 */
bool Adafruit_TMAG5273::setTemperatureThreshold(uint8_t threshold) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_T_CONFIG, 1);
  Adafruit_BusIO_RegisterBits t_thr = Adafruit_BusIO_RegisterBits(&reg, 7, 1);
  return t_thr.write(threshold);
}

/*!
 * @brief Get temperature threshold
 * @return Current temperature threshold
 */
uint8_t Adafruit_TMAG5273::getTemperatureThreshold() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_T_CONFIG, 1);
  Adafruit_BusIO_RegisterBits t_thr = Adafruit_BusIO_RegisterBits(&reg, 7, 1);
  return t_thr.read();
}

/*!
 * @brief Set gain correction value
 * @param value Gain value (0-255, 0 means 1.0)
 * @return True on success
 */
bool Adafruit_TMAG5273::setGainValue(uint8_t value) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MAG_GAIN_CONFIG, 1);
  return reg.write(value);
}

/*!
 * @brief Get gain correction value
 * @return Current gain value
 */
uint8_t Adafruit_TMAG5273::getGainValue() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MAG_GAIN_CONFIG, 1);
  return reg.read();
}

/*!
 * @brief Set first axis offset correction
 * @param offset 8-bit signed offset
 * @return True on success
 */
bool Adafruit_TMAG5273::setOffset1(int8_t offset) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MAG_OFFSET_CONFIG_1, 1);
  return reg.write((uint8_t)offset);
}

/*!
 * @brief Get first axis offset
 * @return Current offset value
 */
int8_t Adafruit_TMAG5273::getOffset1() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MAG_OFFSET_CONFIG_1, 1);
  return (int8_t)reg.read();
}

/*!
 * @brief Set second axis offset correction
 * @param offset 8-bit signed offset
 * @return True on success
 */
bool Adafruit_TMAG5273::setOffset2(int8_t offset) {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MAG_OFFSET_CONFIG_2, 1);
  return reg.write((uint8_t)offset);
}

/*!
 * @brief Get second axis offset
 * @return Current offset value
 */
int8_t Adafruit_TMAG5273::getOffset2() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_MAG_OFFSET_CONFIG_2, 1);
  return (int8_t)reg.read();
}

/*!
 * @brief Get conversion status register
 * @return Status byte
 */
uint8_t Adafruit_TMAG5273::getConversionStatus() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_CONV_STATUS, 1);
  return reg.read();
}

/*!
 * @brief Get device status register
 * @return Status byte
 */
uint8_t Adafruit_TMAG5273::getDeviceStatus() {
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_DEVICE_STATUS, 1);
  return reg.read();
}

/*!
 * @brief Trigger a single conversion (for standby mode)
 * @return True on success
 */
bool Adafruit_TMAG5273::triggerConversion() {
  // In standby mode, reading from result registers triggers conversion
  // Or we can set CONV_START by writing to DEVICE_CONFIG_2
  // Using a dummy read approach for simplicity
  Adafruit_BusIO_Register reg =
      Adafruit_BusIO_Register(i2c_dev, TMAG5273_REG_CONV_STATUS, 1);
  reg.read();
  return true;
}

/*!
 * @brief Get sensor event data (Adafruit_Sensor interface)
 * @param event Pointer to event structure to fill
 * @return True on success
 */
bool Adafruit_TMAG5273::getEvent(sensors_event_t* event) {
  memset(event, 0, sizeof(sensors_event_t));

  event->version = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type = SENSOR_TYPE_MAGNETIC_FIELD;
  event->timestamp = millis();

  event->magnetic.x = readMagneticX();
  event->magnetic.y = readMagneticY();
  event->magnetic.z = readMagneticZ();

  return true;
}

/*!
 * @brief Get sensor details (Adafruit_Sensor interface)
 * @param sensor Pointer to sensor structure to fill
 */
void Adafruit_TMAG5273::getSensor(sensor_t* sensor) {
  memset(sensor, 0, sizeof(sensor_t));

  strncpy(sensor->name, "TMAG5273", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_MAGNETIC_FIELD;
  sensor->min_delay = 0;

  // Values in microTesla based on variant and range
  float max_range_uT = _range_xy * 1000.0; // Convert mT to uT
  sensor->max_value = max_range_uT;
  sensor->min_value = -max_range_uT;

  // Resolution: range / 32768 * 1000 uT
  sensor->resolution = (max_range_uT * 2.0) / 65536.0;
}
