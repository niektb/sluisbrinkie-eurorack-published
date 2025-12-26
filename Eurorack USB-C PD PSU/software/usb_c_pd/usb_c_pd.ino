#include <Adafruit_NeoPixel.h>

#include <Wire.h>
#include <FUSB302_UFP.h>
#include <PD_UFP.h>
#include <PD_UFP_Protocol.h>

#include "pin_def.h"

PD_UFP_c PD_UFP;

Adafruit_NeoPixel strip(1, PIN_RGB, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  pinMode(PIN_FLT_N, INPUT);
  pinMode(PIN_IMON, INPUT);
  pinMode(PIN_PGOOD, INPUT);
  pinMode(PIN_POWER_EN, OUTPUT);

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  digitalWrite(PIN_LED1, 1);
  digitalWrite(PIN_LED2, 1);

  Wire.begin();
  PD_UFP.init(PIN_INT_N, PD_POWER_OPTION_MAX_15V);

  strip.begin();
  strip.show();
  strip.setBrightness(50);
}

void loop() {
  PD_UFP.run();

  bool flt_n = digitalRead(PIN_FLT_N);
  bool flt = !flt_n;
  bool pgood = digitalRead(PIN_PGOOD);
  int current = 0;


  if (PD_UFP.is_power_ready()) {    // PD handshake success
    digitalWrite(PIN_POWER_EN, 1);  // enable power
    digitalWrite(PIN_LED1, 1);
    digitalWrite(PIN_LED2, 1);
    if (pgood) {                    // if PGOOD also high we start monitoring current
      int current = analogRead(PIN_IMON);
    }
  } else {
    digitalWrite(PIN_POWER_EN, 0);  // enable power
    digitalWrite(PIN_LED1, 0);
    digitalWrite(PIN_LED2, 0);
  }
  strip.setPixelColor(0, strip.Color(flt * 50, pgood * 50, map(current, 0, 1023, 0, 255)));
  strip.show();
  delay(1);
}