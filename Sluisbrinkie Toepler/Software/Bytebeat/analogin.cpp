/* This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ */

#include "analogin.h"

AnalogIn::AnalogIn(int pin, float minVal, float maxVal) {
  this->pin = pin;
  this->minVal = minVal;
  this->maxVal = maxVal;
  curVal = analogRead(pin);
  curVal = mapFloat(curVal, 0, 1023, minVal, maxVal);
  tolerance = 0.001 * (maxVal - minVal);  // 5% of the full value range
  //tolerance = 0.00; // semitone
  lastVal = curVal;
}

AnalogIn::~AnalogIn() {
}

float AnalogIn::readValue() {
  float val = analogRead(pin);
  val = mapFloat(val, 0, 1023, minVal, maxVal);
  curVal = val;
  return curVal;
}

bool AnalogIn::hasValueChanged() {
  float val = readValue();
  float diff = abs(val - lastVal);
  if (diff > tolerance) {
    lastVal = val;
    return true;
  }
  return false;
}

float AnalogIn::getCurrentValue() {
  return curVal;
}

float AnalogIn::mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
