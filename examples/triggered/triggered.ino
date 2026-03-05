/*!
 * @file triggered.ino
 * @brief Demonstrates triggered (on-demand) measurements with the TMAG5273
 *
 * Instead of continuous mode, this example uses STANDBY mode where
 * conversions are triggered manually. This saves power between measurements
 * since the sensor is not continuously converting.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 * MIT license
 */

#include <Adafruit_TMAG5273.h>

Adafruit_TMAG5273 tmag;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("Adafruit TMAG5273 Triggered Mode Example"));

  if (!tmag.begin()) {
    Serial.println(F("Failed to find TMAG5273 sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("TMAG5273 found!"));

  // Switch to STANDBY mode for triggered measurements
  // In standby, sensor is idle until we trigger a conversion
  tmag.setOperatingMode(TMAG5273_MODE_STANDBY);
  Serial.println(F("Set to STANDBY mode - power saving between reads"));
}

void loop() {
  // Trigger a single conversion
  tmag.triggerConversion();

  // Wait for conversion to complete (RESULT_STATUS bit set)
  // Typical conversion time is ~3ms with 1x averaging
  while (!(tmag.getConversionStatus() & 0x01)) {
    delay(1);
  }

  // Read results
  float x = tmag.readMagneticX();
  float y = tmag.readMagneticY();
  float z = tmag.readMagneticZ();
  float temp = tmag.getTemperature();

  Serial.print(F("X: "));
  Serial.print(x, 1);
  Serial.print(F(" uT\tY: "));
  Serial.print(y, 1);
  Serial.print(F(" uT\tZ: "));
  Serial.print(z, 1);
  Serial.print(F(" uT\tT: "));
  Serial.print(temp, 1);
  Serial.println(F(" C"));

  // Delay between measurements - sensor stays in low-power standby
  delay(1000);
}
