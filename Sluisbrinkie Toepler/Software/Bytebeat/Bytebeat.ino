/*
This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/
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
        output = t * (42 & t >> 10);
        break;
      default:
        output = 0;
        break;
    }
    PWM_VALUE = output;
    t++;
  }

  sample_div++;
  if (sample_div > 3)
    sample_div = 0;
}
