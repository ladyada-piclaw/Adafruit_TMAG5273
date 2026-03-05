/*!
 * @file 12_dataready_int_verify.ino
 * @brief Verify TMAG5273 INT pin fires data-ready interrupts on D2
 *
 * Hardware: Metro Mini (AVR) + TMAG5273 breakout
 * INT pin connected to D2 (external interrupt capable)
 */

#include <Adafruit_TMAG5273.h>
#include <Wire.h>

#define INT_PIN 2
#define TMAG5273_ADDR 0x35
#define INT_CONFIG_1_REG 0x08

Adafruit_TMAG5273 tmag;

volatile uint16_t isrCount = 0;

void dataReadyISR() {
  isrCount++;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== TMAG5273 Data-Ready Interrupt Verify ==="));
  Serial.println(F("Target: Metro Mini (AVR), INT on D2"));
  Serial.println();

  // Initialize sensor (begin() enables data-ready interrupt by default)
  if (!tmag.begin()) {
    Serial.println(F("FAIL: Could not find TMAG5273 sensor!"));
    while (1)
      delay(100);
  }
  Serial.println(F("TMAG5273 found!"));

  // Dump INT_CONFIG_1 register to verify data-ready interrupt is enabled
  Wire.beginTransmission(TMAG5273_ADDR);
  Wire.write(INT_CONFIG_1_REG);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)TMAG5273_ADDR, (uint8_t)1);
  uint8_t intConfig1 = Wire.read();
  Serial.print(F("INT_CONFIG_1 (0x08) = 0x"));
  Serial.println(intConfig1, HEX);
  Serial.print(F("  Data-ready on INT: "));
  Serial.println((intConfig1 & 0x04) ? F("ENABLED") : F("DISABLED"));
  Serial.println();

  // Set up D2 as input with pullup, attach ISR on FALLING edge
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), dataReadyISR, FALLING);

  Serial.println(F("Starting 5-second interrupt test..."));
  Serial.println(F("Reading# | ISR Count | INT Pin State"));
  Serial.println(F("--------|-----------|---------------"));
}

void loop() {
  static uint16_t readingCount = 0;
  static unsigned long startTime = millis();

  // Run test for 5 seconds
  if (millis() - startTime < 5000) {
    readingCount++;

    // Read magnetic data (clears latched interrupt, allows new one)
    sensors_event_t event;
    tmag.getEvent(&event);

    // Print status
    Serial.print(F("   "));
    Serial.print(readingCount);
    Serial.print(F("    |    "));
    Serial.print(isrCount);
    Serial.print(F("      |      "));
    Serial.println(digitalRead(INT_PIN) ? F("HIGH") : F("LOW"));

    delay(200);
  } else {
    // Test complete - print summary
    detachInterrupt(digitalPinToInterrupt(INT_PIN));

    Serial.println();
    Serial.println(F("=== TEST SUMMARY ==="));
    Serial.print(F("Total readings: "));
    Serial.println(readingCount);
    Serial.print(F("Total ISR fires: "));
    Serial.println(isrCount);
    Serial.println();

    if (isrCount >= 10) {
      Serial.println(F("*** PASS: INT pin is working! ***"));
      Serial.println(F("D2 received data-ready interrupts from TMAG5273."));
    } else if (isrCount > 0) {
      Serial.println(F("*** PARTIAL: Some interrupts detected ***"));
      Serial.print(F("Only "));
      Serial.print(isrCount);
      Serial.println(F(" ISR fires (expected >= 10)."));
      Serial.println(F("Check wiring or interrupt configuration."));
    } else {
      Serial.println(F("*** FAIL: No interrupts detected! ***"));
      Serial.println(F("Check: INT pin wired to D2?"));
      Serial.println(F("Check: INT_CONFIG_1 shows data-ready enabled?"));
    }

    Serial.println();
    Serial.println(F("Test complete. Halting."));
    while (1)
      delay(1000);
  }
}
