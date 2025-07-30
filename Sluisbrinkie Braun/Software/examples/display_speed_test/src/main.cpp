#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS 9
#define TFT_RST 15
#define TFT_DC 8
#define TFT_SCK 10
#define TFT_MOSI 11

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI1, TFT_CS, TFT_DC, TFT_RST);

const int displayWidth = 240;
const int displayHeight = 320;

void setup() {
  Serial.begin(9600);
  delay(2000);
  while (!Serial) delay(10);
  Serial.println(F("Hello! ST77xx TFT Test"));

  SPI1.setSCK(TFT_SCK);
  SPI1.setTX(TFT_MOSI);
  SPI1.setCS(TFT_CS);
  SPI1.begin();

  tft.init(displayWidth, displayHeight);
  tft.setRotation(2);
  tft.setSPISpeed(62500000);

  tft.fillScreen(ST77XX_BLACK);

  Serial.println("Starting speed test...");
  delay(1000);

  unsigned long t0 = millis();

  for (int i = 0; i < 10; i++) {
    tft.fillRect(0, 0, 240, 320, ST77XX_BLACK);
  }

  unsigned long t1 = millis();
  Serial.print("Time for 100 fillRects: ");
  Serial.print(t1 - t0);
  Serial.println(" ms");

  Serial.println("Done.");
}

void loop() {
  // put your main code here, to run repeatedly:
}