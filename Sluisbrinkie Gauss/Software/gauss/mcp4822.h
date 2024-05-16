#pragma once

#include <Arduino.h>
#include <SPI.h>

class Mcp4822 {
public:
  Mcp4822(int CS);
  ~Mcp4822();
  void writeDac(int chan, int val);
  void writeDac1(int val);
  void writeDac2(int val);

private:
  int CS;
  int amp_gain;
};
