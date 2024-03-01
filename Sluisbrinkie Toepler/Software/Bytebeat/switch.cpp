/* This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ */

#include "switch.h"

Switch::Switch(const uint8_t pin)
  : pin(pin),
    switchState(HIGH),
    prevSwitchState(HIGH),
    lastDebounceTime(0U),
    firstTapTime(0U),
    firstReleaseTime(0U),
    WaitForRelease(false),
    prevButtonState(HIGH) {
  pinMode(this->pin, INPUT_PULLUP);
}

Switch::~Switch() {
}

int8_t Switch::TaskFunction() {
  int8_t result = -1;
  /* DEBOUNCE PARAMETERS */
  const uint8_t debounce_delay = 20;

  int buttonState = digitalRead(this->pin);

  // reset debouncing timer if the switch changed, due to noise or pressing:
  if (buttonState != this->prevButtonState) {
    this->lastDebounceTime = millis();
  }

  // if the button state is stable for at least [debounce_delay], fire body of the statement
  if ((millis() - this->lastDebounceTime) > debounce_delay) {
    // buttonState and lastButtonState represent the 'unstable' input that gets updated continuously.
    // these are used for debouncing.
    // switchState is the stable input that can be used for reading button presses.
    if (buttonState != this->switchState) {
      this->switchState = buttonState;
    }

    if ((this->switchState == LOW) && (this->prevSwitchState == HIGH))  // SWITCH PRESS
    {
      this->firstTapTime = millis();
      this->WaitForRelease = true;
      result = 0;

      this->prevSwitchState = this->switchState;
    } else if (this->WaitForRelease && ((millis() - this->firstTapTime) >= 1000))  // SWITCH HOLD
    {
      this->WaitForRelease = false;
      result = 1;
    } else if (this->switchState == HIGH && this->prevSwitchState == LOW)  // SWITCH RELEASE
    {
      if (this->WaitForRelease) {
        this->firstReleaseTime = millis();
        result = 2;
        this->WaitForRelease = false;
      }
      this->prevSwitchState = this->switchState;
    }
  }
  this->prevButtonState = buttonState;
  return result;
}
