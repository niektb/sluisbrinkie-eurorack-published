#include "digitalin.h"
#include "Arduino.h"

DigitalIn::DigitalIn(int pin) {
  this->pin = pin;
  pinMode(this->pin, INPUT_PULLUP);
  this->prevDigitalInState = digitalRead(this->pin);
}

DigitalIn::~DigitalIn() {
}

// Returns:
// -1 if nothing happened
// 0 if toggled from high to low
// 1 if toggled from low to high
int DigitalIn::TaskFunction() {
  uint8_t result = -1;
  this->digitalInState = digitalRead(this->pin);

  // Digital In is active low
  if (this->digitalInState != this->prevDigitalInState && this->digitalInState == 1) {
    // Toggle from high to low
    result = 0;
  } else if (this->digitalInState != this->prevDigitalInState && this->digitalInState == 0) {
    // Toggle from low to high
    result = 1;
  } else {
  }

  this->prevDigitalInState = this->digitalInState;
  return result;
}
