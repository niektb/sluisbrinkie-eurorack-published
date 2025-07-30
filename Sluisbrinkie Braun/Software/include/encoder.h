#pragma once

#include <Arduino.h>

class Encoder {
public:
  Encoder(int pinA, int pinB);
  ~Encoder();
  int TaskFunction();
  int getCounter();
  int8_t read_rotary();
  void setCounter(int counter);

private:
  int pinA;
  int pinB;
  int c;
  int val;
  uint8_t prevNextCode;
  uint16_t store; 
  const int8_t rot_enc_table[16];
};
