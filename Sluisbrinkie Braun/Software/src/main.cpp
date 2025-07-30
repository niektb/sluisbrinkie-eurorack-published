#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <math.h>
#include "encoder.h"

#define TFT_CS 9
#define TFT_RST 15
#define TFT_DC 8
#define TFT_SCK 10
#define TFT_MOSI 11

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI1, TFT_CS, TFT_DC, TFT_RST);

// Exponential time base parameters
const float minDisplayTime = 0.005;  // 5 ms minimum display span
const float maxDisplayTime = 5.0;    // 5 seconds maximum display span
const int encoderMax = 1000;         // Encoder value range

const int numChannels = 4;
const int displayWidth = 240;
const int displayHeight = 320;  // Portrait mode height
int rowHeight = displayHeight / numChannels;

#define numDisplayTimes 8
int displayTimes[numDisplayTimes] = { 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000 };

#define DEBUG_OUTPUT false

// Store previous Y positions for each channel (for drawing connecting lines)
int prevY[numChannels];
// Current drawing column
int currentX = 0;

Encoder encoder = Encoder(ENCA, ENCB);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("Hello World!"));

  SPI1.setSCK(TFT_SCK);
  SPI1.setTX(TFT_MOSI);
  SPI1.setCS(TFT_CS);
  SPI1.begin();

  tft.init(displayWidth, displayHeight);
  tft.setRotation(2);
  tft.setSPISpeed(62500000);
  tft.fillScreen(ST77XX_BLACK);

  analogReadResolution(12);  // Use the RP2040's 12-bit ADC

  // Initialize each channel's previous Y to the midpoint of its row
  for (int ch = 0; ch < numChannels; ch++) {
    prevY[ch] = ch * rowHeight + rowHeight / 2;
  }
}

#define INVERTED_ENCODER true
long encVal = 0;
unsigned long lastSampleTime = 0;

void loop() {
  // Read and constrain the encoder value for the time base
  switch (encoder.TaskFunction()) {
    // Returns:
    // -1 if nothing happened
    // 0 if rotated CW
    // 1 if rotated CCW
    case 0:  // rotated clockwise
      if (INVERTED_ENCODER)
        encVal++;
      else
        encVal--;

      encVal = constrain(encVal, 0, numDisplayTimes - 1);

#if DEBUG_OUTPUT
      Serial.println("ENC CW");
#endif

      break;
    case 1:  // if rotated counter-clockwise
      if (INVERTED_ENCODER)
        encVal--;
      else
        encVal++;

      encVal = constrain(encVal, 0, numDisplayTimes - 1);

#if DEBUG_OUTPUT
      Serial.println("ENC CCW");
#endif

      break;
    default:  // nothing happened
      break;
  }

  float displayTime = displayTimes[encVal];
  // Determine sample interval per pixel
  float sampleInterval = displayTime / displayWidth;
  unsigned long sampleDelay_us = sampleInterval * 1e3;

#if DEBUG_OUTPUT
  Serial.print("Display Time: ");
  Serial.print(displayTime, 5);
  Serial.print(" s, Sample Delay: ");
  Serial.print(sampleDelay_us);
  Serial.println(" us");
#endif

  // If enough time has passed since the last sample
  unsigned long now = micros();
  if (now - lastSampleTime < sampleDelay_us) {
    return;  // Wait until it's time for the next sample
  }
  lastSampleTime = now;

  // Clear the current column by drawing a vertical black line over the entire height
  tft.drawFastVLine(currentX, 0, displayHeight, ST77XX_BLACK);

  // For each channel, read a new sample and draw a line from the previous value
  for (int ch = 0; ch < numChannels; ch++) {
    // Read analog value from channel; cast to uint16_t for clarity
    uint16_t adcVal = (uint16_t)analogRead(26 + ch);
    //Serial.println(adcVal);
    // Map the ADC value (0-4095) to the channel's display row.
    // A lower ADC value appears at the top of the row and a higher one at the bottom.
    int y = map(adcVal, 0, 4095, ch * rowHeight, ch * rowHeight + rowHeight - 1);

    // Determine the previous x position (with wrap-around)
    int prevX = (currentX == 0) ? displayWidth - 1 : currentX - 1;
    // Draw a connecting line for a smooth waveform

    int color = 0x0000;

    switch (ch) {
      case 0:
        color = ST77XX_YELLOW;
        break;
      case 1:
        color = ST77XX_MAGENTA;
        break;
      case 2:
        color = ST77XX_CYAN;
        break;
      case 3:
        color = ST77XX_GREEN;
        break;
    }

    if (currentX == 0)
      tft.drawPixel(currentX, y, color);
    else {
      tft.drawLine(prevX, prevY[ch], currentX, y, color);
    }

    // Update the previous Y value for this channel
    prevY[ch] = y;
  }

  // Move to the next column; wrap back to 0 when reaching the display width
  currentX = (currentX + 1) % displayWidth;
}