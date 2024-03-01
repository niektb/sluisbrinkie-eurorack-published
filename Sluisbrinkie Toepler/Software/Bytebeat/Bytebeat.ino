/*
This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/

Attempt to implement bytebeats (the works from Viznut)
Concept of morphable bytebeats and equations borrowed from Glitch Storm by Spherical Sound Society (https://github.com/spherical-sound-society/glitch-storm)

TODO: The Trigger inputs and CV4 should do something :D
*/

//libraries used
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

volatile uint8_t output = 0;  // value to output
volatile unsigned long t = 0;
volatile uint8_t sample_div = 0;
volatile uint8_t func = 0;
volatile int8_t speed = 1;
volatile int a, b, c;
volatile int aTop = 99;
volatile int aBottom = 0;
volatile int bTop = 99;
volatile int bBottom = 0;
volatile int cTop = 99;
volatile int cBottom = 0;

#include "PinDef.h"
#include "switch.h"
Switch switch1 = Switch(BUTTON1);  // button top-left
Switch switch2 = Switch(BUTTON2);  // button top-right

// Changing these will also requires rewriting audioOn()
// PWM defines
#define PWM_PIN 9
#define PWM_VALUE OCR4B
#define PWM_INTERRUPT TIMER4_OVF_vect

void audioOn() {
  // Set up PWM to 31.25kHz, phase accurate
  TCCR4A = _BV(COM4B0) | _BV(PWM4B);
  TCCR4B = _BV(CS40);
  TCCR4C = _BV(COM4B0S);
  TCCR4D = _BV(WGM40);
  TIMSK4 = _BV(TOIE4);
}

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  audioOn();
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  //speed = map(analogRead(CV1_PIN), 0, 1023, -4, 4);
  //if (speed == 0)
  speed = 1;

  a = map(analogRead(CV1_PIN), 0, 1023, aBottom, aTop);
  b = map(analogRead(CV2_PIN), 0, 1023, bBottom, bTop);
  c = map(analogRead(CV3_PIN), 0, 1023, cBottom, cTop);

  // Switch.TaskFunction Returns:
  // -1 if nothing happened
  // 0 if switch pressed
  // 1 if switch is held
  // 2 if switch is released before being held
  switch (switch1.TaskFunction()) {
    case 0:
      // don't do anything on press
      break;
    case 1:
      break;
    case 2:
      if (func > 0)
        func--;
      break;
    default: 
      break;
  }

  switch (switch2.TaskFunction()) {
    case 0:
      // don't do anything on press
      break;
    case 1:
      break;
    case 2:
      if (func < 7)
        func++;
      break;
    default:
      break;
  }

  digitalWrite(LED1, func & 0x04);
  digitalWrite(LED2, func & 0x02);
  digitalWrite(LED3, func & 0x01);
}

// PWM_INTERRUPT runs at 31.25kHz to oversample and reduce aliasing, but the bytebeat itself runs at a quarter of that frequency
SIGNAL(PWM_INTERRUPT) {
  if (sample_div == 0) {

    switch (func) {
      case 0:
        output = ((t & ((t >> a))) + (t | ((t >> b)))) & (t >> (c + 1)) | (t >> a) & (t * (t >> b));
        aTop = 10;
        aBottom = 0;
        bTop = 14;
        bBottom = 0;
        cTop = 14;
        cBottom = 0;
        break;
      case 1:
        output = t >> c ^ t & 37 | t + (t ^ t >> a) - t * ((t >> a ? 2 : 6) & t >> b) ^ t << 1 & (t & b ? t >> 4 : t >> 10);
        aTop = 30;
        aBottom = 6;
        bTop = 16;
        bBottom = 0;
        cTop = 10;
        cBottom = 0;
        break;
      case 2:
        output = b * t >> a ^ t & (37 - c) | t + ((t ^ t >> 11)) - t * ((t >> 6 ? 2 : a) & t >> (c + b)) ^ t << 1 & (t & 6 ? t >> 4 : t >> c);
        aTop = 12;
        aBottom = 0;
        bTop = 16;
        bBottom = 0;
        cTop = 10;
        cBottom = 0;
        break;
      case 3:
        output = c * t >> 2 ^ t & (30 - b) | t + ((t ^ t >> b)) - t * ((t >> 6 ? a : c) & t >> (a)) ^ t << 1 & (t & b ? t >> 4 : t >> c);
        aTop = 24;
        aBottom = 0;
        bTop = 22;
        bBottom = 0;
        cTop = 16;
        cBottom = 0;
        break;
      case 4:
        output = ((t >> a & t) - (t >> a) + (t >> a & t)) + (t * ((t >> c) & b));
        aTop = 10;
        aBottom = 3;
        bTop = 28;
        bBottom = 0;
        cTop = 10;
        cBottom = 3;
        break;
      case 5:
        output = ((t * (t >> a | t >> (a & c)) & b & t >> 8)) ^ (t & t >> c | t >> 6);
        aTop = 16;
        aBottom = 0;
        bTop = 86;
        bBottom = 0;
        cTop = 26;
        cBottom = 0;
        break;
      case 6:
        output = ((t >> c) * 7 | (t >> a) * 8 | (t >> b) * 7) & (t >> 7);
        aTop = 8;
        aBottom = 0;
        bTop = 22;
        bBottom = 0;
        cTop = 13;
        cBottom = 0;
        break;
      case 7:
        output = (t * 12 & t >> a | t * b & t >> c | t * b & c / (b << 2)) - 2;
        aTop = 18;
        aBottom = 0;
        bTop = 8;
        bBottom = 1;
        cTop = 14;
        cBottom = 5;
        break;
    }
    PWM_VALUE = output;
    t += speed;
  }

  sample_div++;
  if (sample_div > 3)
    sample_div = 0;
}
