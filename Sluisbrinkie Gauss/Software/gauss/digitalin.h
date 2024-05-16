#pragma once

#include <Arduino.h>

class DigitalIn {
public:
  // Constructor
  // @param pin : Pin number of the switch
  DigitalIn(int pin);

  // Destructor
  ~DigitalIn();

  int TaskFunction();

private:
  // Pin number of the DigitalIn
  int pin;

  int digitalInState;
  int prevDigitalInState;
};