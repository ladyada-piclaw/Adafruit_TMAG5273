// I2C Scanner
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Wire.begin();
  Serial.println(F("I2C Scanner"));

  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print(F("Found device at 0x"));
      if (addr < 16) Serial.print(F("0"));
      Serial.println(addr, HEX);
    }
  }
  Serial.println(F("Scan done."));
}

void loop() { delay(1000); }
