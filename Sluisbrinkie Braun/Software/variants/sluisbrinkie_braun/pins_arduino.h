#pragma once

// Pin definitions taken from:
//    https://learn.adafruit.com/assets/100337

// Serial
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
#define PIN_SPI1_MOSI  (11u)
#define PIN_SPI1_SCK   (10u)
#define PIN_SPI1_SS    (9u)

// Wire (I2C)
#define __WIRE0_DEVICE i2c0
#define PIN_WIRE0_SDA  (4u)
#define PIN_WIRE0_SCL  (5u)
#define __WIRE1_DEVICE i2c1
#define PIN_WIRE1_SDA  (6u)
#define PIN_WIRE1_SCL  (7u)

#define SERIAL_HOWMANY (1u)
#define SPI_HOWMANY    (2u)
#define WIRE_HOWMANY   (2u)

// Braun pins
#define OUT1 (0u)
#define OUT2 (1u)
#define SW1 (8u)
#define CV1 (26u)
#define CV2 (27u)
#define CV3 (28u)
#define CV4 (29u)
#define DC (8u)
#define ENCA (24u)
#define ENCB (25u)


// Include standard Arduino pin definitions
#ifndef PINS_COUNT
#define PINS_COUNT          (30u)
#define NUM_DIGITAL_PINS    (30u)
#define NUM_ANALOG_INPUTS   (4u)
#define NUM_ANALOG_OUTPUTS  (0u)
#define ADC_RESOLUTION      (12u)
#endif

// SPI Definitions
#define SPI_INTERFACES_COUNT (2)

// Wire Definitions
#define WIRE_INTERFACES_COUNT (2)

// Serial Definitions  
#define SERIAL_INTERFACES_COUNT (3)

#define PIN_LED        (25u)
#define LED_BUILTIN    PIN_LED
