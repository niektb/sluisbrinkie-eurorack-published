#include "ADEnvelope.h"

ADEnvelope::ADEnvelope(uint16_t attack, uint16_t decay) {
  this->attackTime = attack;
  this->decayTime = decay;
  this->envelopeValue = 0;
  this->attackStartTime = 0;
  this->decayStartTime = 0;
  this->isAttacking = false;
  this->loopEnabled = false;  // Looping is disabled by default
  this->startingEnvelopeValue = 0;
  this->flagEndOfRise = false;
  this->flagEndOfCycle = false;
}

void ADEnvelope::setAttack(uint16_t attack) {
  this->attackTime = attack;
}

void ADEnvelope::setDecay(uint16_t decay) {
  this->decayTime = decay;
}

bool ADEnvelope::getEORStatus() {
  return this->flagEndOfRise;
}

bool ADEnvelope::getEOCStatus() {
  return this->flagEndOfCycle;
}

void ADEnvelope::resetEORStatus() {
  this->flagEndOfRise = false;
}

void ADEnvelope::resetEOCStatus() {
  this->flagEndOfCycle = false;
}

void ADEnvelope::startAttack() {
  this->attackStartTime = millis();
  this->startingEnvelopeValue = this->envelopeValue;
  this->isAttacking = true;
}

void ADEnvelope::startDecay() {
  if (!this->isAttacking)
    return;

  this->decayStartTime = millis();
  this->startingEnvelopeValue = this->envelopeValue;
  this->isAttacking = false;
}

void ADEnvelope::enableLoop(bool enable) {
  this->loopEnabled = enable;
}

uint8_t ADEnvelope::taskFunction() {
  unsigned long currentTime = millis();

  if (this->isAttacking) {
    uint16_t elapsedTime = min(currentTime - this->attackStartTime, this->attackTime);
    this->envelopeValue = map(elapsedTime, 0, this->attackTime, this->startingEnvelopeValue, 255);

    if (elapsedTime >= this->attackTime) {
      this->startDecay();
      this->flagEndOfRise = true;  // should be reset inside main sketch
    }
  } else {
    uint16_t elapsedTime = min(currentTime - this->decayStartTime, this->decayTime);
    this->envelopeValue = map(elapsedTime, 0, this->decayTime, this->startingEnvelopeValue, 0);

    if (elapsedTime >= this->decayTime && this->loopEnabled) {
      this->startAttack();          // Restart from the attack phase if looping is enabled
      this->flagEndOfCycle = true;  // should be reset inside main sketch
    }
  }

  return this->envelopeValue;
}
