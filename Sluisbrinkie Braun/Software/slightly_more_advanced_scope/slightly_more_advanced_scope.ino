#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "encoder.h"
#include <math.h>

#define TFT_CS 1
#define TFT_RST 15
#define TFT_DC 8
#define TFT_SCK 2
#define TFT_MOSI 3

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Display settings
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320
#define NUM_CHANNELS 4
#define ROW_HEIGHT (DISPLAY_HEIGHT / NUM_CHANNELS)

// Buffer settings
#define BUFFER_SIZE 2048  // Larger buffer for oversampling

// Encoder settings
Encoder encoder = Encoder(ENCA, ENCB);
#define INVERTED_ENCODER true
int displayTimes[] = {1,2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000 };
const int numDisplayTimes = sizeof(displayTimes) / sizeof(displayTimes[0]);
long encVal = 7;

// Sampling
unsigned long lastSampleTime = 0;
uint16_t buffer[NUM_CHANNELS][BUFFER_SIZE];
uint16_t drawBuffer[NUM_CHANNELS][DISPLAY_WIDTH];
volatile int writeIndex = 0;
int triggerIndex = 0;
volatile bool triggered = false;

// Variables dedicated to the streaming mode
int streamX;
int streamY[NUM_CHANNELS];

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Scope Starting...");

  // Setup display
  SPI.setSCK(TFT_SCK);
  SPI.setTX(TFT_MOSI);
  SPI.setCS(TFT_CS);
  SPI.begin();
  tft.init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  tft.setRotation(2);
  tft.setSPISpeed(62500000);
  tft.fillScreen(ST77XX_BLACK);

  analogReadResolution(12);
}

void loop() {
  handleEncoder();

  float displayTime = displayTimes[encVal];
  float sampleInterval = displayTime / DISPLAY_WIDTH;
  unsigned long sampleDelay_us = sampleInterval * 1e3;

  unsigned long now = micros();
  if (now - lastSampleTime < sampleDelay_us)
    return;
  lastSampleTime = now;

  if (displayTime > 100) {
    // Streaming mode

    // Clear the current column by drawing a vertical black line over the entire height
    tft.drawFastVLine(streamX, 0, DISPLAY_HEIGHT, ST77XX_BLACK);

    // For each channel, read a new sample and draw a line from the previous value
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
      // Read analog value from channel; cast to uint16_t for clarity
      uint16_t adcVal = (uint16_t)analogRead(26 + ch);
      //Serial.println(adcVal);
      // Map the ADC value (0-4095) to the channel's display row.
      // A lower ADC value appears at the top of the row and a higher one at the bottom.
      int y = map(adcVal, 0, 4095, ch * ROW_HEIGHT, ch * ROW_HEIGHT + ROW_HEIGHT - 1);

      // Determine the previous x position (with wrap-around)
      int prevX = (streamX == 0) ? DISPLAY_WIDTH - 1 : streamX - 1;
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

      if (streamX == 0)
        tft.drawPixel(streamX, y, color);
      else {
        tft.drawLine(prevX, streamY[ch], streamX, y, color);
      }

      // Update the previous Y value for this channel
      streamY[ch] = y;
    }

    // Move to the next column; wrap back to 0 when reaching the display width
    streamX = (streamX + 1) % DISPLAY_WIDTH;

  } else {
    // Sample all channels
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
      buffer[ch][writeIndex] = analogRead(26 + ch);
    }

    // Trigger detection
    if (!triggered && writeIndex > 0) {
      int prev = buffer[0][(writeIndex - 1 + BUFFER_SIZE) % BUFFER_SIZE];
      int curr = buffer[0][writeIndex];
      if (prev < 2048 && curr >= 2048) {
        triggerIndex = writeIndex;
        triggered = true;
      }
    }

    writeIndex = (writeIndex + 1) % BUFFER_SIZE;

    // After capturing a full sweep, signal Core 1
    if (triggered && (writeIndex == (triggerIndex + DISPLAY_WIDTH) % BUFFER_SIZE)) {
      drawWaveform();
      triggered = false;
    }
  }
}

void handleEncoder() {
  switch (encoder.TaskFunction()) {
    case 0: encVal += (INVERTED_ENCODER ? 1 : -1); break;
    case 1: encVal += (INVERTED_ENCODER ? -1 : 1); break;
  }
  encVal = constrain(encVal, 0, numDisplayTimes - 1);
}

void drawWaveform() {
  int start = triggerIndex;

  // Copy a linear chunk from circular buffer
  for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      int idx = (start + x) % BUFFER_SIZE;
      drawBuffer[ch][x] = buffer[ch][idx];
    }
  }

  // Draw from the linear buffer
  for (int x = 0; x < DISPLAY_WIDTH; x++) {
    tft.drawFastVLine(x, 0, DISPLAY_HEIGHT, ST77XX_BLACK);

    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
      uint16_t y1 = map(drawBuffer[ch][x], 0, 4095, ch * ROW_HEIGHT, ch * ROW_HEIGHT + ROW_HEIGHT - 1);
      uint16_t y2 = map(drawBuffer[ch][x + 1], 0, 4095, ch * ROW_HEIGHT, ch * ROW_HEIGHT + ROW_HEIGHT - 1);

      uint16_t color;
      switch (ch) {
        case 0: color = ST77XX_YELLOW; break;
        case 1: color = ST77XX_MAGENTA; break;
        case 2: color = ST77XX_CYAN; break;
        case 3: color = ST77XX_GREEN; break;
      }

      //tft.drawLine(x, y1, x + 1, y2, color);
      tft.drawPixel(x, y1, color);
      //tft.drawFastVLine(x, y1, (y1-y2), color);
    }
  }
}