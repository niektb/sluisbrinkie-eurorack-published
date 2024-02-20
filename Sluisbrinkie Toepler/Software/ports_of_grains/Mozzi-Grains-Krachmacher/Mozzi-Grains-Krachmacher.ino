/*
###
2024 - 02 - 20: Port for Sluisbrinkie Toepler. Routing Mode is selected using the buttons (LEDs represent currently selected mode using binary coding). 
Audio In goes to CV4. Gate Out (which is used as Gate In in this sketch) is connected to TR1. TR2, TROUT1 and MIDI are unused in this sketch.
Port is released under the same license as the original software.
###

# 'Mozzi-GRAINS Krachmacher' 
  
an experimental Firmware for the AE Modular GRAINS module by tangible waves, using the Mozzi library: https://sensorium.github.io/Mozzi
The intention of this Firmware is to provide noisy tones, useful for all kinds of things, along with optional "VCAs" and a VCF.
    
To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
To include the Mozzi library with your sketch please also refer to "Installation" at https://sensorium.github.io/Mozzi.
Documentation on Mozzi can be found here: https://sensorium.github.io/Mozzi/doc/html/index.html

This sketch is an experimental implementation using waveguide technology, as well-known by the Karplus Strong agorithm:
https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis
It also was inspired by the following publication:
https://www.researchgate.net/publication/330667582_uTubo_-_Development_and_Application_of_an_Alternative_Digital_Musical_Instrument

Demotrack available here: https://soundcloud.com/taitekatto/mozzi-grains-krachmacher
  
# Usage of this Firmware with GRAINS:

__Mode-Switch__ 

 * G/M Switch has to be set to 'M' to make GRAINS work properly with this Mozzi based sketch!
  
__Inputs__ 
  
* IN1 / Pot1: Size of delayline one, changes pitch and timbre
* IN2 / Pot2: Size of delayline two, changes pitch and timbre
* IN3:        Filter cutoff frequency in case if filter is used by routing-setting, optionally controls VCA2 with one routing mode 
* Pot3:       Algorithm- and routing-modes for KM (Krachmacher):  1) KM (fast), 2) KM (slower) 3) KM (slowest), 4) VCA->KM, 5) VCA->KM->VCF, 6) KM->VCF, 7) VCA2->KM->VCA, 8) KM->VCA
* A:          CV for VCA or resonance of filter, depending on routing (resonance only is available in case if VCA is unused!)

* D:          Select Noise-sources for our impulse-sources: a) White+Pink Noise, b) White+White Noise, c) Pink+Pink Noise in "round-robin"-mode
              Please note: 'D' is situated on the right side of GRAINS, but used as a gate/trigger-input in this case! 
              
__Outputs__

* OUT:        Sound output

__Patching and settings suggestions__

* Use with much delay and/or reverb to create "wall of sound"-type ambiences
* Use with a "colouring" module, NYLEFILTER highly recommended!
* Use as percussive sound as input to a VCA and/or Filter with fast attack and release
* Use as percussive sound as input to LOPAG (Low Pass Gate)
* Use a trigger-source like TRIQ164 or TOPOGRAF to change the character of the noise-waves via 'D' (Gate input in this special case)
* Use 2ATT/CV and/or 4ATTMIX to send trigger/gate signal to 'D' by hand (Gateoutput, used as an input with Krachmacher!)
* Use the triggerbutton from 2ENV (short attack!) to to send trigger/gate signal to 'D' by hand (Gateoutput, used as an input with Krachmacher!)
* Use a sequencer like SEQ16 or SEQ8 to change the character of the delaylines and or drive the VCA via IN-3 and/or Audio-in, depending on setting of Pot-3 (routing-mode) 
* Use a sequencer like SEQ16 or SEQ8 to change the VCA-Level, thus resulting in an overall change in soundcharacter via Audio-in and/or IN-3, depending on routing-mode
* Use an LFO to change filtercutoff and/or VCA automatically via IN-3 and/or Audio-in, depending on setting of Pot-3 (routing-mode) 
* Use 2ATT/CV and/or 4ATTMIX to send CV to IN-3 or Audio-In, changing the soundcharacter, depending on setting of Pot-3 (routing-mode) 
* Use a MIDI-keyboard to send CV to IN-3 or Audio-In, changing the soundcharacter, depending on setting of Pot-3 (routing-mode) 
* Combine any of the above or find your own ways to make as much noise as possible (which is what Krachmacher means in German ;-)


# Notes:
  
Caution! Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'Mozzi-GRAINS Krachmacher' an experimental firmware for the AE Modular GRAINS module by tangible waves
    
Copyright (C) 2020  Mathias Brüssel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.
  
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// --- Include files needed for this sketch ---
#include <MozziGuts.h>      // --- This is the basic include to be done to enable the Mozzi-Framework ---
#include <Oscil.h>          // Oscillator template, uses the tables as below
#include <AudioDelay.h>     // We use this to delay our tones and to filter them by overlaying two tones
#include <Smooth.h>         // Smoothing of CV using an infinite impulse response low pass filter
#include <LowPassFilter.h>  // Classical 24db lowpass-filter
#include <IntMap.h>         // Faster version of Arduino map()

// Pin Definition and button handling for Toepler
#include "PinDef.h"
#include "switch.h"
Switch switch1 = Switch(BUTTON1);  // button top-left
Switch switch2 = Switch(BUTTON2);  // button top-right

// --- Wavetables used for the envelope-follower ---
#include <tables/whitenoise8192_int8.h>
#include <tables/pinknoise8192_int8.h>

// --- GRAINS CV ins, outs and Pots, using 'Arduino-PINS' according to: https://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS ---
// Pin mapping adjusted for Toepler
#define CONTROL_RATE 128  // Frequency for reading of CV/Pot values of GRAINS

#define CV_POT_IN1 CV1_PIN
#define CV_POT_IN2 CV2_PIN
#define CV_IN3 CV3_PIN
#define CV_AUDIO_IN CV4_PIN
#define CV_AUDIO_OUT OUT1A
#define CV_GATE_OUT TR1_PIN

// --- DELAY Lines sizes and min/max indexes ---
#define DL_ONE_SIZE 256  // Delay line one size - max for Mozzi is 512, we use values that proofed to be appropriate and the ATmega328 has little space anyhow
#define DL_TWO_SIZE 384  // Delay line two size - max for Mozzi is 512
#define DL_ONE_MIN 1     // Delay line one min index
#define DL_TWO_MIN 1     // Delay line one min index
#define DL_ONE_MAX 256   // Delay line one max index - note: we use the highest index higher than possible on purpose, should be no issue with this circular buffer
#define DL_TWO_MAX 384   // Delay line two max index - note: we use the highest index higher than possible on purpose, should be no issue with this circular buffer

// --- Oscillator-objects containing wavetables  ---
static Oscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> whiteNoise(WHITENOISE8192_DATA);
static Oscil<PINKNOISE8192_NUM_CELLS, AUDIO_RATE> pinkNoise(PINKNOISE8192_DATA);

// --- Objects to smooth input data from pots or CV via rolling averages ---
static Smooth<int> cv_pot_in1_smoothed(0.90F);
static Smooth<int> cv_pot_in2_smoothed(0.90F);
static Smooth<int> cv_pot3_smoothed(0.85F);
static Smooth<int> cv_in3_smoothed(0.80F);
static Smooth<int> cv_audio_in_smoothed(0.80F);

// --- Two delay lines to overlay two impulses, given a filter-effect via phase-distinction and similar ---
static AudioDelay<DL_ONE_SIZE> delayLineOne;
static AudioDelay<DL_TWO_SIZE> delayLineTwo;

// --- Filters, will be used in different way, depending on routing-mode selected ---
static LowPassFilter filter_24db;

// --- We use these variables to communicate between updateControl() and updateAudio() ---
static int8_t lineOneDelay, lineTwoDelay = DL_ONE_MIN;  // Initial offset for delay-lines
static int8_t routing_mode = 4;                         // Function selection via Pot-3
static int16_t inline_gain, gain = 0;                   // Volumes for our "VCAs"
static uint8_t wave_mode = 0;                           // We have 3 combinations of "impulse"-waves to choose from: WhiteNoise+PinkNoise, 2*WhiteNoise or 2*PinkNoise

// --- The arduino setup() function, initialise the Mozzi-Framework here as well ---
void setup() {
  pinMode(CV_GATE_OUT, INPUT_PULLUP);  // We will use GRAINS Gate-out to switch waves for soundgeneration
  //Serial.begin(115200);
  filter_24db.setResonance(0);
  startMozzi(CONTROL_RATE);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

// --- After arduino's loop() function is entered, the Mozzi-Framework will take over control of the application
void loop() {
  audioHook();
}

// --- Helper function: supply a means to map floating-point values according to CV / Pot selection ---
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// --- Here we process all CV data and Pot-settings ---
void updateControl() {
  static int cv_pot_in1, cv_pot_in2, cv_pot3, cv_in3, cv_audio_in, cv_gate_in;  // Variables to store CV/Pot values of GRAINS-module, don't use local variables to speed up calling of updateControl()
  static IntMap cv_pot3_map(0, 1023, 0, 8);                                     // Use Mozzi's faster value-mapping objects. It utilizes only the precision needed and precalculates values during compiletime where possible!
  static IntMap cv_in3_map(0, 1023, 0, 255);
  static IntMap cv_pot_in1_map(0, 1023, DL_ONE_MIN, DL_ONE_MAX);
  static IntMap cv_pot_in2_map(0, 1023, DL_TWO_MIN, DL_TWO_MAX);
  static IntMap cv_audio_in_map(0, 1023, 0, 255);
  static IntMap inline_gain_map(0, 1023, 386, 1023);
  static bool gate_was_low = true;

  // Routing modes for KM (Krachmacher):  KM only, KM->VCA, VCA->KM, KM->VCF, VCA->KM->VCF, KM->VCA->VCF

  // Switch.TaskFunction Returns:
  // -1 if nothing happened
  // 0 if switch pressed
  // 1 if switch is held
  // 2 if switch is released before being held
  switch (switch1.TaskFunction()) {
    case 0:
      // don't do anything on press
      break;
    case 1:
      break;
    case 2:
      if (routing_mode > 0)
        routing_mode--;
      break;
    default:
      break;
  }

  switch (switch2.TaskFunction()) {
    case 0:
      // don't do anything on press
      break;
    case 1:
      break;
    case 2:
      if (routing_mode < 7)
        routing_mode++;
      break;
    default:
      break;
  }

  digitalWrite(LED1, routing_mode & 0x04);
  digitalWrite(LED2, routing_mode & 0x02);
  digitalWrite(LED3, routing_mode & 0x01);

  // --- Process CV-IN-3 ---
  cv_in3 = cv_in3_smoothed.next(mozziAnalogRead(CV_IN3));  // Smooth the value for scaling the LFO speeds!
  filter_24db.setCutoffFreq(cv_in3_map(cv_in3));

  // --- Process CV/IN-1 ---
  cv_pot_in1 = cv_pot_in1_smoothed.next(mozziAnalogRead(CV_POT_IN1));
  lineOneDelay = cv_pot_in1_map(cv_pot_in1);  // Calculate the shift-value for delay-line one, to be applied in updateAudio()

  // --- Process CV/IN-2 ---
  cv_pot_in2 = cv_pot_in2_smoothed.next(mozziAnalogRead(CV_POT_IN2));
  lineTwoDelay = cv_pot_in2_map(cv_pot_in2);  // Calculate the shift-value for delay-line one, to be applied in updateAudio()

  // --- Check for VCA-CV via audio-in ---
  cv_audio_in = cv_audio_in_smoothed.next(mozziAnalogRead(CV_AUDIO_IN));  // Already make the audio-CV zero-centered

  // --- Depending on the routing mode make some settings to be "reported" to updateAudio()
  switch (routing_mode)  // Depending on the routing set via Pot-3 we apply different values to resonance and gain
  {
    case 0:
    case 1:
    case 2:
      break;  // The first three modes require no filtering, nor setting of VCA[s], so to speed up things we ignore these here!
    case 3:
      inline_gain = inline_gain_map(cv_audio_in);  // We apply the gain within the delayline and use different boundaries, to only change the sound instead of muting
      break;
    case 4:                                        // VCF + inline VCA used here
      inline_gain = inline_gain_map(cv_audio_in);  // We apply the gain within the delayline and use different boundaries, to only change the sound instead of muting
      filter_24db.setResonance(0);                 // Resonance not applipicale, reset to default value!
      break;
    case 5:  // Only VCF used: we have a free CV-source to change the filter-resonance (instead of inline VCA)
      filter_24db.setResonance(cv_audio_in_map(cv_audio_in));
      break;
    case 6:  // In Mode 6 we have In-3 controlling the inline-gain and Audio-in controlling gain (No filter)
      inline_gain = inline_gain_map(cv_in3);
      gain = cv_audio_in;
      break;
    case 7:
      gain = cv_audio_in;  // Only VCA is applied, no filter, no inline-VCA
      break;
  }
  // --- Check for new trigger to change the connected soundtables per oscillator ---
  cv_gate_in = digitalRead(CV_GATE_OUT);  // Already make the audio-CV zero-centered
  if (cv_gate_in == HIGH) {
    if (gate_was_low)
      wave_mode = (++wave_mode) % 3;  // Cycle through values 0,1,2 - will be set appropriately in updateAudio()
    gate_was_low = false;
  } else
    gate_was_low = true;
}

// --- Calculation of audio-data has to be done here in Mozzi's audio-callback function ---
int updateAudio()                      // This is the main function of the Mozzi-framework for processing of audio-data, it will be called at audio-rate (16384 Hz)
{                                      // We do as little work as possible here, because this routine gets called at audio-rate!
  static int8_t my_wave = 0;           // This is the result of our audio-calculation that will be send to audio-out
  static int8_t lineOne, lineTwo = 0;  // Store results of delaylines permanentely for iterative processing
  static int8_t waveA, waveB = 0;      // One sample of noise

  // --- Process incoming/generative noise ---                // Get samples from noise wavetables, which is used as a kind of Karplus Strong impulse, here
  switch (wave_mode)  // THis is to be switched via a trigger on "Gate-out"
  {
    case 0:
      waveA = whiteNoise.next();  // We use White Noise for delay line 1 and Pink Noise for delay line 2 (or other combinations as below)
      waveB = pinkNoise.next();   // Note: there is no frequency set for these oscillators, so we get byte by byte from the wavetable,
      break;                      // each time in identical order, thus resulting in repetative waves
    case 1:
      waveA = whiteNoise.next();
      waveB = whiteNoise.next();
      break;
    case 2:
      waveA = pinkNoise.next();
      waveB = pinkNoise.next();
      break;
  }
  // --- Different routings (and/or algorithms) to be selected from ---
  switch (routing_mode)                                                    // Pot-3 is used to select different routing options, see description of Pot-3 for details in header above
  {                                                                        // Routing modes for KM (Krachmacher):  0: KM only, 1: KM->VCA, 2: VCA->KM, 3: KM->VCF, 4: VCA->KM->VCF, 5: KM->VCF->VCA
    case 0:                                                                // Pure output (fast frequency), no VCA or VCF applied
      lineOne = waveA + delayLineOne.next(waveA + lineOne, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = waveB + delayLineTwo.next(waveB + lineTwo, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = lineOne + lineTwo;                                         // Adding the two delay lines, so we have two tones mixed together for output and/or VCA and/or Filter, depending on routing
      break;

    case 1:                                                                       // Pure output (slower frequency), no VCA or VCF applied
      lineOne = (waveA >> 1) + delayLineOne.next(waveA + lineOne, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = (waveB >> 1) + delayLineTwo.next(waveB + lineTwo, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = lineOne + lineTwo;                                                // Adding the two delay lines, so we have two tones mixed together for output and/or VCA and/or Filter, depending on routing
      break;

    case 2:                                                        // Pure output (slowest frequency), no VCA or VCF applied
      lineOne = delayLineOne.next(waveA + lineOne, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = delayLineTwo.next(waveB + lineTwo, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = lineOne + lineTwo;                                 // Adding the two delay lines, so we have two tones mixed together for output and/or VCA and/or Filter, depending on routing
      break;

    case 3:                                                                             // VCA->KM, the VCA gets applied within the delay line, thus changin also the soundcharacter along with the volume
      lineOne = delayLineOne.next((waveA + lineOne) * inline_gain >> 9, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = delayLineTwo.next((waveB + lineTwo) * inline_gain >> 9, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = lineOne + lineTwo;                                                      // Adding the two delay lines, so we have two tones mixed together for output and/or VCA and/or Filter, depending on routing
      break;

    case 4:                                                                             // VCA->KM->VCF, the VCA gets applied within the delay line, changing also the soundcharacter, then filter cutoff is applied
      lineOne = delayLineOne.next((waveA + lineOne) * inline_gain >> 9, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = delayLineTwo.next((waveB + lineTwo) * inline_gain >> 9, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = filter_24db.next((lineOne + lineTwo) >> 1);
      break;

    case 5:                                                        // KM->VCF, the VCF alone gets applied to the output
      lineOne = delayLineOne.next(waveA + lineOne, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = delayLineTwo.next(waveB + lineTwo, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = filter_24db.next((lineOne + lineTwo) >> 1);        // We reduce bitwith just a bit to have more headroom in case if high resonances are applied!
      break;

    case 6:                                                                             // VCA2->KM->VCA, the VCA gets applied inline and to the output (IN3-CV used to control inline-gain!)
      lineOne = delayLineOne.next((waveA + lineOne) * inline_gain >> 9, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = delayLineTwo.next((waveB + lineTwo) * inline_gain >> 9, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = (lineOne + lineTwo) * gain >> 9;                                        // Apply VCA (depending on CV on Audio-in) to output
      break;

    case 7:                                                        // KM->VCA, the VCA gets applied to the output
      lineOne = delayLineOne.next(waveA + lineOne, lineOneDelay);  // Delay-Lines, to overlay our noise-grains, also resulting in filter-effect: AudioDelay::next(in_value,delaytime_cells);
      lineTwo = delayLineTwo.next(waveB + lineTwo, lineTwoDelay);  // lineOne and lineTwo results will be applied/shifted iteratively
      my_wave = (lineOne + lineTwo) * gain >> 9;                   // Apply VCA (depending on CV on Audio-in) to output
      break;
  }
  return (my_wave);
}
