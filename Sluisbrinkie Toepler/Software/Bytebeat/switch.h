/* This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ */

#pragma once

#include <Arduino.h>

class Switch {
public:
  // Constructor
  // @param pin  Pin number of the switch
  Switch(const uint8_t pin);

  // Destructor
  ~Switch();

  // Function to execute in the task
  // Returns:
  // -1 if nothing happened
  // 0 if switch pressed
  // 1 if switch is held
  // 2 if switch is released before being held
  int8_t TaskFunction();

private:
  // Pin number of the switch
  const uint8_t pin;

  // Stable input that can be used for reading button
  int switchState;
  // Previous stable input state
  int prevSwitchState = HIGH;

  // Last debounce time
  uint32_t lastDebounceTime;
  // First tap time
  uint32_t firstTapTime;
  // First release time
  uint32_t firstReleaseTime;

  // Wait for release
  bool WaitForRelease;

  // Raw previous button state used for debouncing
  int prevButtonState = HIGH;
};
