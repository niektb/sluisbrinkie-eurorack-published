#include "encoder.h"

Encoder::Encoder(int pinA, int pinB) : rot_enc_table{0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0} {
  this->pinA = pinA;  // sometimes called CLK (Clock)
  this->pinB = pinB;  // sometimes called DT  (Data)
  pinMode(this->pinA, INPUT);
  pinMode(this->pinB, INPUT);
  this->prevNextCode = 0;
  this->store = 0;
  this->val = 0;
  this->c = 0;
}

Encoder::~Encoder() {
}

// Function to execute in the task
// Implementation derived from https://www.best-microcontroller-projects.com/rotary-encoder.html
// Returns:
// -1 if nothing happened
// 0 if rotated CW
// 1 if rotated CCW
int Encoder::TaskFunction() {
  int ret = -1;

  if ((this->val = this->read_rotary())) {
    this->c += this->val;

    if (prevNextCode == 0x0b)  // CCW
      ret = 1;
    else  // CW
      ret = 0;
  }
  return ret;
}

int Encoder::getCounter() {
  return this->c;
}

void Encoder::setCounter(int counter) {
  this->c = counter;
}

// A vald CW or CCW move returns 1/-1, invalid returns 0.
int8_t Encoder::read_rotary() {
  this->prevNextCode <<= 2;
  if (digitalRead(this->pinB)) this->prevNextCode |= 0x02;
  if (digitalRead(this->pinA)) this->prevNextCode |= 0x01;
  this->prevNextCode &= 0x0f;

  // If valid then store as 16 bit data.
  if (rot_enc_table[prevNextCode]) {
    this->store <<= 4;
    this->store |= this->prevNextCode;

    if ((this->store & 0xff) == 0x2b) return -1;
    if ((this->store & 0xff) == 0x17) return 1;
  }
  return 0;
}