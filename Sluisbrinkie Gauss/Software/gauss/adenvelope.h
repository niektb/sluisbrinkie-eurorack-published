#pragma once

#include <Arduino.h>

class ADEnvelope {
private:
  uint16_t attackTime;            // Attack time in milliseconds (16-bit)
  uint16_t decayTime;             // Decay time in milliseconds (16-bit)
  uint8_t envelopeValue;          // Current envelope value
  uint8_t startingEnvelopeValue;  // Current envelope value
  unsigned long attackStartTime;  // Timestamp when attack started
  unsigned long decayStartTime;   // Timestamp when decay started
  bool isAttacking;               // Flag to track if it's in attack phase
  bool loopEnabled;               // Flag to enable looping
  bool flagEndOfRise;             // Flag to signal end of Rising phase
  bool flagEndOfCycle;            // Flag to signal end of full cycle

public:
  ADEnvelope(uint16_t attack, uint16_t decay);
  void setAttack(uint16_t attack);
  void setDecay(uint16_t decay);
  bool getEORStatus();
  bool getEOCStatus();
  void resetEORStatus();
  void resetEOCStatus();
  void startAttack();
  void startDecay();
  void enableLoop(bool enable);  // Method to enable or disable looping
  uint8_t taskFunction();        // Renamed from calculateEnvelope and returns the envelope value
};