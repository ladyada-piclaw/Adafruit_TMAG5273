/*!
 * @file tft_demo.ino
 *
 * TFT demo for TMAG5273 3-axis Hall-effect sensor on Feather ESP32-S2 TFT
 * Big, bold display for product photos/video.
 *
 * Hardware:
 *  - Adafruit Feather ESP32-S2 TFT
 *  - TMAG5273 Hall-effect sensor breakout (I2C)
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 * for Adafruit Industries.
 *
 * MIT license, all text above must be included in any redistribution
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_TMAG5273.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_TMAG5273 tmag;

#define SCREEN_W 240
#define SCREEN_H 135

GFXcanvas16 canvas(SCREEN_W, SCREEN_H);

bool isX2Variant = false;
const float MAX_RANGE = 100000.0; // ±100,000 µT

// Colors
#define COLOR_X 0xF800 // Red
#define COLOR_Y 0x07E0 // Green
#define COLOR_Z 0x539F // Light blue

// Layout
#define TITLE_Y 20
#define BAR_START_Y 30
#define BAR_H 28
#define BAR_GAP 6
#define BAR_MARGIN 6

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(100);

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  tft.init(135, 240);
  tft.setRotation(3);

  canvas.setTextWrap(false);

  if (!tmag.begin()) {
    canvas.fillScreen(ST77XX_BLACK);
    canvas.setFont(&FreeSansBold18pt7b);
    canvas.setTextColor(ST77XX_RED);
    canvas.setCursor(10, 80);
    canvas.print(F("No TMAG5273!"));
    tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
    while (1)
      delay(10);
  }

  uint8_t devID = tmag.getDeviceID();
  isX2Variant = ((devID & 0x03) == 0x02);

  Serial.print(F("TMAG5273"));
  Serial.println(isX2Variant ? F("x2") : F("x1"));
}

void loop() {
  float magX = tmag.readMagneticX();
  float magY = tmag.readMagneticY();
  float magZ = tmag.readMagneticZ();

  canvas.fillScreen(ST77XX_BLACK);

  // ---- Centered title ----
  canvas.setFont(&FreeSansBold9pt7b);
  canvas.setTextColor(ST77XX_WHITE);
  const char *title = isX2Variant ? "Adafruit TMAG5273x2" : "Adafruit TMAG5273x1";
  int16_t x1, y1;
  uint16_t tw, th;
  canvas.getTextBounds(title, 0, 0, &x1, &y1, &tw, &th);
  canvas.setCursor((SCREEN_W - tw) / 2, TITLE_Y);
  canvas.print(title);

  // ---- Three bipolar bars ----
  uint16_t barW = SCREEN_W - (BAR_MARGIN * 2);
  float vals[3] = {magX, magY, magZ};
  uint16_t colors[3] = {COLOR_X, COLOR_Y, COLOR_Z};
  const char *labels[3] = {"X", "Y", "Z"};

  for (int i = 0; i < 3; i++) {
    int16_t by = BAR_START_Y + i * (BAR_H + BAR_GAP);

    // White outline
    canvas.drawRect(BAR_MARGIN, by, barW, BAR_H, ST77XX_WHITE);

    // Center line
    int16_t cx = BAR_MARGIN + barW / 2;
    canvas.drawFastVLine(cx, by, BAR_H, ST77XX_WHITE);

    // Fill from center
    float clamped = constrain(vals[i], -MAX_RANGE, MAX_RANGE);
    int16_t fillW = (int16_t)(clamped / MAX_RANGE * (barW / 2 - 1));
    if (fillW > 0) {
      canvas.fillRect(cx, by + 1, fillW, BAR_H - 2, colors[i]);
    } else if (fillW < 0) {
      canvas.fillRect(cx + fillW, by + 1, -fillW, BAR_H - 2, colors[i]);
    }

    // Axis label with outline for readability
    canvas.setFont(&FreeSansBold9pt7b);
    int16_t lx = BAR_MARGIN + 4;
    int16_t ly = by + BAR_H - 7;
    for (int8_t dx = -1; dx <= 1; dx++) {
      for (int8_t dy = -1; dy <= 1; dy++) {
        if (dx == 0 && dy == 0)
          continue;
        canvas.setTextColor(ST77XX_BLACK);
        canvas.setCursor(lx + dx, ly + dy);
        canvas.print(labels[i]);
      }
    }
    canvas.setTextColor(colors[i]);
    canvas.setCursor(lx, ly);
    canvas.print(labels[i]);

    // Value on right side with outline
    char valBuf[10];
    dtostrf(vals[i], 0, 0, valBuf);
    canvas.getTextBounds(valBuf, 0, 0, &x1, &y1, &tw, &th);
    int16_t vx = SCREEN_W - BAR_MARGIN - tw - 4;
    int16_t vy = ly;
    for (int8_t dx = -1; dx <= 1; dx++) {
      for (int8_t dy = -1; dy <= 1; dy++) {
        if (dx == 0 && dy == 0)
          continue;
        canvas.setTextColor(ST77XX_BLACK);
        canvas.setCursor(vx + dx, vy + dy);
        canvas.print(valBuf);
      }
    }
    canvas.setTextColor(ST77XX_WHITE);
    canvas.setCursor(vx, vy);
    canvas.print(valBuf);
  }

  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
  delay(100);
}
