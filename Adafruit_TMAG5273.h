/*!
 * @file Adafruit_TMAG5273.h
 *
 * This is a library for the TI TMAG5273 3-axis Hall-effect sensor
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * MIT license, all text above must be included in any redistribution
 */

#ifndef ADAFRUIT_TMAG5273_H
#define ADAFRUIT_TMAG5273_H

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Wire.h>

#define TMAG5273_DEFAULT_I2CADDR 0x35 ///< Default I2C address (A variant)

// Register addresses
#define TMAG5273_REG_DEVICE_CONFIG_1 0x00     ///< Device configuration 1
#define TMAG5273_REG_DEVICE_CONFIG_2 0x01     ///< Device configuration 2
#define TMAG5273_REG_SENSOR_CONFIG_1 0x02     ///< Sensor configuration 1
#define TMAG5273_REG_SENSOR_CONFIG_2 0x03     ///< Sensor configuration 2
#define TMAG5273_REG_X_THR_CONFIG 0x04        ///< X threshold configuration
#define TMAG5273_REG_Y_THR_CONFIG 0x05        ///< Y threshold configuration
#define TMAG5273_REG_Z_THR_CONFIG 0x06        ///< Z threshold configuration
#define TMAG5273_REG_T_CONFIG 0x07            ///< Temperature configuration
#define TMAG5273_REG_INT_CONFIG_1 0x08        ///< Interrupt configuration
#define TMAG5273_REG_MAG_GAIN_CONFIG 0x09     ///< Magnetic gain configuration
#define TMAG5273_REG_MAG_OFFSET_CONFIG_1 0x0A ///< Offset config 1
#define TMAG5273_REG_MAG_OFFSET_CONFIG_2 0x0B ///< Offset config 2
#define TMAG5273_REG_I2C_ADDRESS 0x0C         ///< I2C address register
#define TMAG5273_REG_DEVICE_ID 0x0D           ///< Device ID register
#define TMAG5273_REG_MANUFACTURER_ID_LSB 0x0E ///< Manufacturer ID LSB
#define TMAG5273_REG_MANUFACTURER_ID_MSB 0x0F ///< Manufacturer ID MSB
#define TMAG5273_REG_T_MSB_RESULT 0x10        ///< Temperature MSB
#define TMAG5273_REG_T_LSB_RESULT 0x11        ///< Temperature LSB
#define TMAG5273_REG_X_MSB_RESULT 0x12        ///< X-axis MSB
#define TMAG5273_REG_X_LSB_RESULT 0x13        ///< X-axis LSB
#define TMAG5273_REG_Y_MSB_RESULT 0x14        ///< Y-axis MSB
#define TMAG5273_REG_Y_LSB_RESULT 0x15        ///< Y-axis LSB
#define TMAG5273_REG_Z_MSB_RESULT 0x16        ///< Z-axis MSB
#define TMAG5273_REG_Z_LSB_RESULT 0x17        ///< Z-axis LSB
#define TMAG5273_REG_CONV_STATUS 0x18         ///< Conversion status
#define TMAG5273_STATUS_RESULT_READY 0x01     ///< Conversion data ready
#define TMAG5273_STATUS_DIAG_FAIL 0x02        ///< Diagnostic fail detected
#define TMAG5273_STATUS_POR 0x10              ///< Power-on reset occurred
#define TMAG5273_REG_ANGLE_RESULT_MSB 0x19    ///< Angle result MSB
#define TMAG5273_REG_ANGLE_RESULT_LSB 0x1A    ///< Angle result LSB
#define TMAG5273_REG_MAGNITUDE_RESULT 0x1B    ///< Magnitude result
#define TMAG5273_REG_DEVICE_STATUS 0x1C       ///< Device status

/** Conversion averaging options */
typedef enum {
  TMAG5273_CONV_AVG_1X = 0x00,  ///< 1x averaging
  TMAG5273_CONV_AVG_2X = 0x01,  ///< 2x averaging
  TMAG5273_CONV_AVG_4X = 0x02,  ///< 4x averaging
  TMAG5273_CONV_AVG_8X = 0x03,  ///< 8x averaging
  TMAG5273_CONV_AVG_16X = 0x04, ///< 16x averaging
  TMAG5273_CONV_AVG_32X = 0x05, ///< 32x averaging
} tmag5273_conv_avg_t;

/** Magnetic channel enable options */
typedef enum {
  TMAG5273_MAG_CH_OFF = 0x00, ///< All channels off
  TMAG5273_MAG_CH_X = 0x01,   ///< X only
  TMAG5273_MAG_CH_Y = 0x02,   ///< Y only
  TMAG5273_MAG_CH_XY = 0x03,  ///< X and Y
  TMAG5273_MAG_CH_Z = 0x04,   ///< Z only
  TMAG5273_MAG_CH_ZX = 0x05,  ///< Z and X
  TMAG5273_MAG_CH_YZ = 0x06,  ///< Y and Z
  TMAG5273_MAG_CH_XYZ = 0x07, ///< X, Y, and Z
  TMAG5273_MAG_CH_XYX = 0x08, ///< X, Y, X (pseudo-simultaneous)
  TMAG5273_MAG_CH_YXY = 0x09, ///< Y, X, Y (pseudo-simultaneous)
  TMAG5273_MAG_CH_YZY = 0x0A, ///< Y, Z, Y (pseudo-simultaneous)
  TMAG5273_MAG_CH_XZX = 0x0B, ///< X, Z, X (pseudo-simultaneous)
} tmag5273_mag_ch_en_t;

/** Sleep time options for wake-up and sleep mode */
typedef enum {
  TMAG5273_SLEEP_1MS = 0x00,     ///< 1ms
  TMAG5273_SLEEP_5MS = 0x01,     ///< 5ms
  TMAG5273_SLEEP_10MS = 0x02,    ///< 10ms
  TMAG5273_SLEEP_15MS = 0x03,    ///< 15ms
  TMAG5273_SLEEP_20MS = 0x04,    ///< 20ms
  TMAG5273_SLEEP_30MS = 0x05,    ///< 30ms
  TMAG5273_SLEEP_50MS = 0x06,    ///< 50ms
  TMAG5273_SLEEP_100MS = 0x07,   ///< 100ms
  TMAG5273_SLEEP_500MS = 0x08,   ///< 500ms
  TMAG5273_SLEEP_1000MS = 0x09,  ///< 1000ms
  TMAG5273_SLEEP_2000MS = 0x0A,  ///< 2000ms
  TMAG5273_SLEEP_5000MS = 0x0B,  ///< 5000ms
  TMAG5273_SLEEP_20000MS = 0x0C, ///< 20000ms
} tmag5273_sleeptime_t;

/** Operating mode options */
typedef enum {
  TMAG5273_MODE_STANDBY = 0x00,      ///< Standby (trigger) mode
  TMAG5273_MODE_SLEEP = 0x01,        ///< Sleep mode (ultra-low power)
  TMAG5273_MODE_CONTINUOUS = 0x02,   ///< Continuous measure mode
  TMAG5273_MODE_WAKEUP_SLEEP = 0x03, ///< Wake-up and sleep mode
} tmag5273_operating_mode_t;

/** Magnet temperature coefficient options */
typedef enum {
  TMAG5273_TEMPCO_NONE = 0x00,    ///< No compensation (0%)
  TMAG5273_TEMPCO_NDFEB = 0x01,   ///< 0.12%/C (NdFeB magnets)
  TMAG5273_TEMPCO_CERAMIC = 0x03, ///< 0.2%/C (Ceramic/Ferrite magnets)
} tmag5273_mag_tempco_t;

/** Angle calculation axis pair options */
typedef enum {
  TMAG5273_ANGLE_OFF = 0x00, ///< No angle calculation
  TMAG5273_ANGLE_XY = 0x01,  ///< X 1st, Y 2nd
  TMAG5273_ANGLE_YZ = 0x02,  ///< Y 1st, Z 2nd
  TMAG5273_ANGLE_XZ = 0x03,  ///< X 1st, Z 2nd
} tmag5273_angle_en_t;

/** I2C read mode options */
typedef enum {
  TMAG5273_I2C_RD_STANDARD = 0x00,    ///< Standard 3-byte read
  TMAG5273_I2C_RD_1BYTE_16BIT = 0x01, ///< 1-byte read for 16-bit data
  TMAG5273_I2C_RD_1BYTE_8BIT = 0x02,  ///< 1-byte read for 8-bit MSB data
} tmag5273_i2c_rd_t;

/** Interrupt mode options */
typedef enum {
  TMAG5273_INT_NONE = 0x00,           ///< No interrupt
  TMAG5273_INT_THROUGH_INT = 0x01,    ///< Interrupt through INT pin
  TMAG5273_INT_INT_EXCEPT_I2C = 0x02, ///< INT except when I2C busy
  TMAG5273_INT_THROUGH_SCL = 0x03,    ///< Interrupt through SCL
  TMAG5273_INT_SCL_EXCEPT_I2C = 0x04, ///< SCL except when I2C busy
} tmag5273_int_mode_t;

/*!
 * @brief Class for interfacing with the TMAG5273 3-axis Hall-effect sensor
 */
class Adafruit_TMAG5273 : public Adafruit_Sensor {
 public:
  Adafruit_TMAG5273();
  ~Adafruit_TMAG5273();

  bool begin(uint8_t addr = TMAG5273_DEFAULT_I2CADDR, TwoWire* theWire = &Wire);

  // ID functions
  uint16_t getManufacturerID();
  uint8_t getDeviceID();

  // Channel configuration
  bool setMagneticChannels(tmag5273_mag_ch_en_t channels);
  tmag5273_mag_ch_en_t getMagneticChannels();

  // Range configuration
  bool setXYRangeWide(bool wide);
  bool getXYRangeWide();
  bool setZRangeWide(bool wide);
  bool getZRangeWide();

  // Averaging
  bool setConversionAverage(tmag5273_conv_avg_t avg);
  tmag5273_conv_avg_t getConversionAverage();

  // Operating mode
  bool setOperatingMode(tmag5273_operating_mode_t mode);
  tmag5273_operating_mode_t getOperatingMode();

  // Sleep time
  bool setSleepTime(tmag5273_sleeptime_t time);
  tmag5273_sleeptime_t getSleepTime();

  // Low noise mode
  bool setLowNoiseMode(bool enable);
  bool getLowNoiseMode();

  // Temperature compensation
  bool setMagTempComp(tmag5273_mag_tempco_t tempco);
  tmag5273_mag_tempco_t getMagTempComp();

  // Angle calculation
  bool setAngleCalculation(tmag5273_angle_en_t angle);
  tmag5273_angle_en_t getAngleCalculation();

  // Temperature channel
  bool enableTemperature(bool enable);
  bool temperatureEnabled();

  // Raw reads (16-bit signed)
  int16_t readX();
  int16_t readY();
  int16_t readZ();

  // Converted reads (in microTesla!)
  float readMagneticX();
  float readMagneticY();
  float readMagneticZ();

  // Temperature
  float getTemperature();

  // Angle and magnitude
  float readAngle();
  uint8_t readMagnitude();
  float readMagnitudeMT();

  // CRC
  bool enableCRC(bool enable);
  bool crcEnabled();

  // Interrupt configuration
  bool setInterruptMode(tmag5273_int_mode_t mode);
  tmag5273_int_mode_t getInterruptMode();
  bool enableResultInterrupt(bool enable);
  bool enableThresholdInterrupt(bool enable);
  bool setInterruptPulsed(bool pulsed);
  bool getInterruptPulsed();

  // Threshold configuration
  bool setXThreshold(int8_t threshold);
  int8_t getXThreshold();
  bool setYThreshold(int8_t threshold);
  int8_t getYThreshold();
  bool setZThreshold(int8_t threshold);
  int8_t getZThreshold();
  bool setTemperatureThreshold(uint8_t threshold);
  uint8_t getTemperatureThreshold();

  // Gain and offset
  bool setGainValue(uint8_t value);
  uint8_t getGainValue();
  bool setOffset1(int8_t offset);
  int8_t getOffset1();
  bool setOffset2(int8_t offset);
  int8_t getOffset2();

  // Status
  uint8_t getConversionStatus();
  uint8_t getDeviceStatus();

  // Trigger conversion (for standby mode)
  bool triggerConversion();

  // Adafruit_Sensor interface
  bool getEvent(sensors_event_t* event);
  void getSensor(sensor_t* sensor);

 private:
  Adafruit_I2CDevice* i2c_dev; ///< I2C device
  bool _is_x2_variant;         ///< True if x2 variant (133/266mT range)
  int32_t _sensorID;           ///< Unique sensor ID
  float _range_xy;             ///< Current XY range in mT
  float _range_z;              ///< Current Z range in mT

  float rawToMicroTesla(int16_t raw, float range_mT);
};

#endif // ADAFRUIT_TMAG5273_H
