/* This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ */

//To convert the port names of the ATMEGA32U4 to Arduino Micro pin numbers
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
#define PD3 0
#define PD2 1
#define PB0 17
#define PF7 A0
#define PF6 A1
#define PF5 A2
#define PF4 A3
#define PF1 A4
#define PF0 A5

// Actual pin mappings
#define CV4_PIN PF7
#define CV3_PIN PF6
#define CV2_PIN PF5
#define CV1_PIN PF4

#define CLKOUT_PIN PB6
#define EORB_PIN PB5
#define EOCA_PIN PB4
#define GATEA_PIN PD0
#define GATEB_PIN PD1

#define SWITCH1 PD6
#define SWITCH2 PD4

#define LED_RX PB0
#define LED_TX PD5

#define PB2 MOSI
#define PB3 MISO
#define PB1 SCK
#define CS1 PB0 