/* This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ */

#pragma once

#include <Arduino.h>

class AnalogIn {
public:
  AnalogIn(int pin, float minVal, float maxVal);
  ~AnalogIn();
  float readValue();
  bool hasValueChanged();
  float getCurrentValue();
  float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);

private:
  int pin;
  float minVal;
  float maxVal;
  float curVal;
  float tolerance;
  float lastVal;
};
