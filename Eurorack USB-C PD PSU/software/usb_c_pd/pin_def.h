#pragma once

// Map functions to Atmega port numbers, according to schematic
#define PIN_RGB PC7
#define PIN_SCL PD0
#define PIN_SDA PD1

#define PIN_PGOOD PB4
#define PIN_FLT_N PB5
#define PIN_POWER_EN PB6
#define PIN_INT_N PE6
#define PIN_IMON PF0
#define PIN_LED1 PB0
#define PIN_LED2 PD5

// Map Atmega ports to Arduino Micro 
// ANALOG
#define PF7 A0
#define PF6 A1
#define PF5 A2
#define PF4 A3
#define PF1 A4
#define PF0 A5

// DIGITAL
#define PC7 13
#define PD6 12
#define PB7 11
#define PB6 10
#define PB5 9
#define PB4 8
#define PE6 7
#define PD7 6
#define PC6 5
#define PD4 4
#define PD0 3
#define PD1 2
#define PD5 30
#define PD3 0
#define PD2 1
#define PB0 17