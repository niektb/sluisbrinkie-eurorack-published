#pragma once

// Pin definitions taken from:
//    https://learn.adafruit.com/assets/100337

// LEDs
#define PIN_LED        (14u)

// NeoPixel
#define PIN_NEOPIXEL   (16u)

// Serial
#define PIN_SERIAL1_TX (12u)
#define PIN_SERIAL1_RX (13u)

// Not pinned out
#define PIN_SERIAL2_TX (31u)
#define PIN_SERIAL2_RX (31u)

// Not pinned out
#define PIN_SPI0_MISO  (31u)
#define PIN_SPI0_MOSI  (31u)
#define PIN_SPI0_SCK   (31u)
#define PIN_SPI0_SS    (31u)

// Not pinned out
#define PIN_SPI1_MISO  (31u)
#define PIN_SPI1_MOSI  (31u)
#define PIN_SPI1_SCK   (31u)
#define PIN_SPI1_SS    (31u)

// Wire
#define __WIRE0_DEVICE i2c1
#define PIN_WIRE0_SDA  (31u)
#define PIN_WIRE0_SCL  (31u)
#define __WIRE1_DEVICE i2c0
#define PIN_WIRE1_SDA  (31u)
#define PIN_WIRE1_SCL  (31u)

#define SERIAL_HOWMANY (1u)
#define SPI_HOWMANY    (0u)
#define WIRE_HOWMANY   (0u)

// Toepler Plus pins
#define OUT1 (0u)
#define OUT2 (1u)
#define SW1 (8u)
#define CV1 (26u)
#define CV2 (27u)
#define CV3 (28u)


#include "../generic/common.h"
