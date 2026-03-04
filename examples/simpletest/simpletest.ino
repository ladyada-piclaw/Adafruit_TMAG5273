/*!
 * @file simpletest.ino
 * @brief Simple test for the Adafruit TMAG5273 3-axis Hall-effect sensor
 *
 * Reads magnetic field (X, Y, Z in uT) and temperature, prints to serial.
 */

#include <Adafruit_TMAG5273.h>

Adafruit_TMAG5273 tmag;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("Adafruit TMAG5273 Simple Test"));

  if (!tmag.begin()) {
    Serial.println(F("Failed to find TMAG5273 sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("TMAG5273 found!"));

  Serial.print(F("Manufacturer ID: 0x"));
  Serial.println(tmag.getManufacturerID(), HEX);
  Serial.print(F("Device ID: 0x"));
  Serial.println(tmag.getDeviceID(), HEX);
}

void loop() {
  float x = tmag.readMagneticX();
  float y = tmag.readMagneticY();
  float z = tmag.readMagneticZ();
  float temp = tmag.getTemperature();

  Serial.print(F("X: "));
  Serial.print(x, 2);
  Serial.print(F(" uT  Y: "));
  Serial.print(y, 2);
  Serial.print(F(" uT  Z: "));
  Serial.print(z, 2);
  Serial.print(F(" uT  Temp: "));
  Serial.print(temp, 2);
  Serial.println(F(" C"));

  delay(100);
}
