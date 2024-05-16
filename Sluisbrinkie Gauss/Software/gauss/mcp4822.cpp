#include "mcp4822.h"

Mcp4822::Mcp4822(int CS) {
  this->amp_gain = 1;
  this->CS = CS;
}

Mcp4822::~Mcp4822() {
}

// Taken partialy from: https://little-scale.blogspot.com/2018/06/monophonic-usb-midi-to-cv-converter.html
// chan (select left or right output)
// val (value to output)
void Mcp4822::writeDac(int chan, int val) {
  chan = chan & 1;
  val = val & 4095;

  digitalWrite(CS, LOW);

  //(left/right, Dont Care, shutdown, amp_gain, value (4 bits))
  SPI.transfer(chan << 7 | this->amp_gain << 5 | 1 << 4 | val >> 8);
  //send second byte [value (last 8 bits)]
  SPI.transfer(val & 255);

  digitalWrite(CS, HIGH);
}

void Mcp4822::writeDac1(int val) {
  this->writeDac(0, val);
}

void Mcp4822::writeDac2(int val) {
  this->writeDac(1, val);
}
