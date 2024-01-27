#include <MozziGuts.h>
#include <Oscil.h>                // oscillator template
#include <tables/sin2048_int8.h>  // sine table for oscillator
#include "PinDef.h"
#include "Arduino.h"
#include <math.h>
#include <mozzi_midi.h>
#include "tables.h"

Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin2(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin3(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin4(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin5(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin6(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin7(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin8(SIN2048_DATA);

#define CONTROL_RATE 256  // Hz, powers of 2 are most reliable

int harm_knob = 0;  //AD wave knob
byte gain = 127;

//OSC frequency knob
int pitch_offset = 12;
float max_voltage_of_adc = 5;
float voltage_division_ratio = 0.5;
float notes_per_octave = 12;
float volts_per_octave = 1;
float scaler = 0.931;
float scaler_offset = 0;
float mapping_upper_limit = (max_voltage_of_adc / voltage_division_ratio) * notes_per_octave * volts_per_octave * scaler + scaler_offset;

float mapFloat(int value, int fromLow, int fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

void setup() {
  Serial.begin(9600);
  startMozzi(CONTROL_RATE);
}

void updateControl() {
  //harmonics
  harm_knob = map(mozziAnalogRead(CV3_PIN) + 1, 0, 1023, 0, 255);

  //harmonics_gain
  gain = map(mozziAnalogRead(CV2_PIN), 0, 1023, 0, 255);

  pitch_offset = map(mozziAnalogRead(CV1_PIN), 0, 1023, -21, 3);

  float data = mozziAnalogRead(CV4_PIN);                                     // read pitch CV as data value using ADC
  float pitch = pitch_offset + mapFloat(data, 0, 1023, 0.0, mapping_upper_limit);  // convert pitch CV data value to a MIDI note number
  pitch = round(pitch);
  int freq = mtof((int)pitch);                                                    // convert MIDI note number to frequency

  aSin1.setFreq(freq);  // set the frequency
  aSin2.setFreq(freq * (pgm_read_byte(&(harm_table[0][harm_knob]))));
  aSin3.setFreq(freq * (pgm_read_byte(&(harm_table[1][harm_knob]))));
  aSin4.setFreq(freq * (pgm_read_byte(&(harm_table[2][harm_knob]))));
  aSin5.setFreq(freq * (pgm_read_byte(&(harm_table[3][harm_knob]))));
  aSin6.setFreq(freq * (pgm_read_byte(&(harm_table[4][harm_knob]))));
  aSin7.setFreq(freq * (pgm_read_byte(&(harm_table[5][harm_knob]))));
  aSin8.setFreq(freq * (pgm_read_byte(&(harm_table[6][harm_knob]))));
}

int updateAudio() {
  return MonoOutput::from8Bit(aSin1.next() * (pgm_read_byte(&(gain_table[0][gain]))) / 1024 + aSin2.next() * (pgm_read_byte(&(gain_table[1][gain]))) / 1024 + aSin3.next() * (pgm_read_byte(&(gain_table[2][gain]))) / 1024 + aSin4.next() * (pgm_read_byte(&(gain_table[3][gain]))) / 1024 + aSin5.next() * (pgm_read_byte(&(gain_table[4][gain]))) / 1024 + aSin6.next() * (pgm_read_byte(&(gain_table[5][gain]))) / 1024 + aSin7.next() * (pgm_read_byte(&(gain_table[6][gain]))) / 1024 + aSin8.next() * (pgm_read_byte(&(gain_table[7][gain]))) / 1024);
}

void loop() {
  audioHook();  // required here
}