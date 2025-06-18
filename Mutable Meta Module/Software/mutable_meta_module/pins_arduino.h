#pragma once
#define PICO_RP2350B 1
// Pin definitions taken from:
//    https://learn.adafruit.com/assets/100337

// LEDs
#define PIN_LED        (25u)

// Not pinned out
#define PIN_SERIAL1_TX (31u)
#define PIN_SERIAL1_RX (31u)

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
#define PIN_WIRE1_SDA  (20u)
#define PIN_WIRE1_SCL  (21u)

#define SERIAL_HOWMANY (0u)
#define SPI_HOWMANY    (0u)
#define WIRE_HOWMANY   (1u)

// MMM pins
#define OUT1 (22u)
#define OUT2 (30u)

#define SW1 (6u)
#define SW2 (17u)

#define ENC1A (8u)
#define ENC1B (9u)
#define ENC2A (2u)
#define ENC2B (3u)
#define ENC3A (18u)
#define ENC3B (19u)

#define CV1 (40u)
#define CV2 (41u)
#define CV3 (42u)
#define CV4 (43u)
#define CV5 (44u)
#define CV6 (45u)
#define CV7 (46u)
#define CV8 (47u)

#include "../generic/common.h"
