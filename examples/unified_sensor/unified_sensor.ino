/*!
 * @file unified_sensor.ino
 * @brief Adafruit Unified Sensor example for TMAG5273 3-axis Hall-effect sensor
 *
 * Demonstrates using the Adafruit_Sensor interface to read magnetic field data.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 */

#include <Adafruit_Sensor.h>
#include <Adafruit_TMAG5273.h>

Adafruit_TMAG5273 tmag;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("Adafruit TMAG5273 Unified Sensor Example"));

  if (!tmag.begin()) {
    Serial.println(F("Failed to find TMAG5273 sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("TMAG5273 found!"));

  // Print sensor details using Unified Sensor interface
  sensor_t sensor;
  tmag.getSensor(&sensor);

  Serial.println(F("-------- Magnetic Sensor --------"));
  Serial.print(F("Sensor:       "));
  Serial.println(sensor.name);
  Serial.print(F("Sensor ID:    "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Type:         "));
  Serial.println(sensor.type);
  Serial.print(F("Max Value:    "));
  Serial.print(sensor.max_value);
  Serial.println(F(" uT"));
  Serial.print(F("Min Value:    "));
  Serial.print(sensor.min_value);
  Serial.println(F(" uT"));
  Serial.print(F("Resolution:   "));
  Serial.print(sensor.resolution, 4);
  Serial.println(F(" uT"));
  Serial.println(F("---------------------------------"));
  Serial.println();
}

void loop() {
  sensors_event_t event;
  tmag.getEvent(&event);

  Serial.print(F("Magnetic X: "));
  Serial.print(event.magnetic.x, 2);
  Serial.print(F(" uT  Y: "));
  Serial.print(event.magnetic.y, 2);
  Serial.print(F(" uT  Z: "));
  Serial.print(event.magnetic.z, 2);
  Serial.println(F(" uT"));

  // Temperature is not part of the unified sensor interface
  float temp = tmag.getTemperature();
  Serial.print(F("Temperature: "));
  Serial.print(temp, 2);
  Serial.println(F(" C"));

  Serial.println();
  delay(500);
}
