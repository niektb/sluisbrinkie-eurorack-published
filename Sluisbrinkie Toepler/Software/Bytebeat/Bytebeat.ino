/*
This work is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/
Attempt to implement bytebeats (originally discovered and described by Viznut)

Some equations are borrowed from Poetaster's Rampart, these are licensed under GPLv3. 
Other equations are taken from the internet, see adjacent comment for origins
*/

//libraries used
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

volatile uint8_t output = 0;      // value to output
volatile double _t = 0;           // time constant, double so I can do smooth pitch shift
long t = 0;                       // rounded version is used for the bytebeat itself
volatile long tmax = 0;           // max time before restart
volatile uint8_t sample_div = 0;  // clock divider
volatile uint8_t func = 0;        // with algorithm to use
volatile uint8_t bank = 0;
const float default_speed = 1.024;
volatile float speedVal = default_speed;  // Time Step Size, slightly faster than 1 because we have a 7.8kHz sample rate instead of 8.0kHz
volatile uint8_t bitmask = 0;             // bitmask for logic operation on the output

#include "PinDef.h"
#include "switch.h"
#include "analogin.h"
Switch switch1 = Switch(BUTTON1);  // button top-left
Switch switch2 = Switch(BUTTON2);  // button top-right
AnalogIn morph(CV1_PIN, 0, 255);
volatile uint8_t morphVal = 0;  // amount of morphing
volatile uint8_t p0, p1, p2;    // morphing parameters
AnalogIn speed(CV2_PIN, -1.51, 1.51);

// Changing these will also requires rewriting audioOn()
// PWM defines
#define PWM_PIN 9
#define PWM_VALUE OCR4B
#define PWM_INTERRUPT TIMER4_OVF_vect

uint32_t eeprom_timeout = 0;
bool wait_for_timeout = false;

volatile int a, b, c;
// these ranges are provisional and in schollz equations need to be reset
volatile int aMax = 99, aMin = 0, bMax = 99, bMin = 0, cMax = 99, cMin = 0;

void audioOn() {
  // Set up PWM to 31.25kHz, phase accurate
  TCCR4A = _BV(COM4B0) | _BV(PWM4B);
  TCCR4B = _BV(CS40);
  TCCR4C = _BV(COM4B0S);
  TCCR4D = _BV(WGM40);
  TIMSK4 = _BV(TOIE4);
}

void setup() {
  get_from_EEPROM();
  pinMode(PWM_PIN, OUTPUT);
  pinMode(TROUT1, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(TR1_PIN, INPUT_PULLUP);
  pinMode(TR2_PIN, INPUT_PULLUP);
  audioOn();
}

void loop() {
  if (morph.hasValueChanged()) {
    uint8_t morphVal = morph.getCurrentValue();
    p0 = (morphVal & 31) | 1;  // lower 5 bits
    p2 = ((morphVal & (63 << 2)) >> 2) | 1;
    p1 = (p0 + p2) >> 1;
  }

  if (speed.hasValueChanged()) {
    float _val = speed.getCurrentValue();
    if (_val < 0.1 && _val > -0.1)  // Centered pot means speed of 1
      speedVal = default_speed;
    else if (_val > 1.5)
      speedVal = 1.5;
    else if (_val < -1.5)
      speedVal = -1.5;
    else
      speedVal = _val;
  }

  uint8_t tr1 = digitalRead(TR1_PIN);
  uint8_t tr2 = digitalRead(TR2_PIN);

  if (!tr1)  // meaning active
    _t = 0;

  if (!tr2)  // meaning active
    speedVal = default_speed;

  digitalWrite(TROUT1, tr1 ^ tr2);

  bitmask = analogRead(CV3_PIN) >> 2;

  tmax = (long)analogRead(CV4_PIN) << 22;
  if (_t > tmax)
    _t = 0;

  // Switch.TaskFunction Returns:
  // -1 if nothing happened
  // 0 if switch pressed
  // 1 if switch is held
  // 2 if switch is released before being held
  switch (switch2.TaskFunction()) {
    case 0:
      // don't do anything on press
      break;
    case 1:
      if (bank < 7) {
        bank++;
        wait_for_timeout = true;
        eeprom_timeout = millis();
      }
      _t = 0;
      digitalWrite(LED1, 0);
      digitalWrite(LED2, 0);
      digitalWrite(LED3, 0);
      delay(100);
      digitalWrite(LED3, bank & 0x04);
      digitalWrite(LED2, bank & 0x02);
      digitalWrite(LED1, bank & 0x01);
      delay(100);
      digitalWrite(LED1, 0);
      digitalWrite(LED2, 0);
      digitalWrite(LED3, 0);
      delay(100);

      break;
    case 2:
      if (func < 7) {
        func++;
        wait_for_timeout = true;
        eeprom_timeout = millis();
        Serial.println(func);
      }
      _t = 0;
      break;
    default:
      break;
  }

  switch (switch1.TaskFunction()) {
    case 0:
      // don't do anything on press
      break;
    case 1:
      if (bank > 1) {
        bank--;
        wait_for_timeout = true;
        eeprom_timeout = millis();
      }
      _t = 0;

      digitalWrite(LED1, 0);
      digitalWrite(LED2, 0);
      digitalWrite(LED3, 0);
      delay(100);
      digitalWrite(LED3, bank & 0x04);
      digitalWrite(LED2, bank & 0x02);
      digitalWrite(LED1, bank & 0x01);
      delay(100);
      digitalWrite(LED1, 0);
      digitalWrite(LED2, 0);
      digitalWrite(LED3, 0);
      delay(100);
      break;
    case 2:
      if (func > 1) {
        func--;
        wait_for_timeout = true;
        eeprom_timeout = millis();
        Serial.println(func);
      }
      _t = 0;

      break;
    default:
      break;
  }

  if ((millis() - eeprom_timeout) > 10000 && wait_for_timeout) {
    // Time to save bank and func to EEPROM
    write_to_EEPROM();
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
    delay(50);
    digitalWrite(LED1, 1);
    digitalWrite(LED2, 1);
    digitalWrite(LED3, 1);
    delay(50);
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
    delay(50);
    wait_for_timeout = false;
  }

  digitalWrite(LED3, func & 0x04);
  digitalWrite(LED2, func & 0x02);
  digitalWrite(LED1, func & 0x01);
}

void write_to_EEPROM() {
  EEPROM.put(0, bank);
  EEPROM.put(sizeof(bank), func);
}

void get_from_EEPROM() {
  EEPROM.get(0, bank);
  EEPROM.get(sizeof(bank), func);
  if (bank < 1)
    bank = 1;
  if (bank > 7)
    bank = 7;
  if (func < 1)
    func = 1;
  if (func > 7)
    func = 7;
}

/* From here on starts the audio section */

// PWM_INTERRUPT runs at 31.25kHz to oversample and reduce aliasing, but the bytebeat itself runs at a quarter of that frequency
SIGNAL(PWM_INTERRUPT) {
  if (sample_div == 0) {

    calculate_bytebeat();

    if (bitmask > 127)
      output &= (bitmask >> 1);
    else
      output |= bitmask;

    PWM_VALUE = output;
  } else if (sample_div == 1) {
    // Spread out calculations across the interrupts
    _t += speedVal;
    t = round(_t);
  }

  sample_div++;
  if (sample_div > 3)
    sample_div = 0;
}

void calculate_bytebeat() {
  switch (bank) {
    case 1:
      bank1();
      break;
    case 2:
      bank2();
      break;
    case 3:
      bank3();
      break;
    case 4:
      bank4();
      break;
    case 5:
      bank5();
      break;
    case 6:
      bank6();
      break;
    case 7:
      bank7();
      break;
    default:
      output = 0;
      break;
  }
}

/* this needs many many more bytebeats */

// Rhythmic Bank
void bank1() {
  switch (func) {
    case 1:
      // Nightmachines https://forum.aemodular.com/post/235
      output = (t * 4 | t | t >> 3 & t + t / 4 & t * p1 | t * 8 >> p0 | t / p2 & t + 140) & t >> 4;
      break;
    case 2:
      //Street Surfer by skurk, raer (2011-09-30) https://www.pouet.net/topic.php?which=8357&page=4#c388479
      output = t & (4096) ? t / 2 * (t ^ t % (p0 << 1)) | t >> 5 : t / (p0 << 1) | (t & (p2 << 7) ? 4 * t : t);
      break;
    case 3:

      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      output = 0;
      break;
  }
}

// Textural Bank
void bank2() {
  switch (func) {
    case 1:
      // from http://xifeng.weebly.com/bytebeats.html
      output = ((((((((t >> p0) | t) | (t >> p0)) * p2) & ((5 * t) | (t >> p2))) | (t ^ (t % p1))) & 0xFF));
      break;
    case 2:
      //"fractal trees", 216's version by Danharaj & 216 (2011-10-03) 13c https://www.reddit.com/r/programming/comments/kyj77/algorithmic_symphonies_from_one_line_of_code_how/
      output = t | t % 255 | t % 257;
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      output = 0;
      break;
  }
}

// Sound FX Bank
void bank3() {
  switch (func) {
    case 1:
      // tejeez 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
      output = (t * (t >> 5 | t >> p1)) >> (t >> p2);  // has some 'dead spots' on the knobs
      break;
    case 2:
      output = t >> 5 | (t >> 2) * (t >> 5);
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      output = 0;
      break;
  }
}

// Melody Bank
void bank4() {
  switch (func) {
    case 1:

      break;
    case 2:
      // miiro 2011-09-30 https://www.youtube.com/watch?v=qlrs2Vorw2Y&t=134s
      output = 5 * t & t >> 7 | 3 * t & 4 * t >> 10;
      break;
    case 3:
      // Extraordinary thread of FRACTAL MUSIC by Anonymous from russian imageboards (2014-07-12) http://arhivach.ng/thread/28592/#71678984
      output = t >> 4 + t % p2 | t >> 5 + t % (t / 31108 & 1 ? 46 : 43) | t / 4 | t / 8 % p2;
      break;
    case 4:
      // xpansive 2011-09-29 https://www.pouet.net/topic.php?which=8357&page=3#c388375
      output = t * (t >> 8 | t >> 9) & 46 & t >> 8 ^ (t & t >> 13 | t >> 6);
      break;
    case 5:
      //OMG TEH BYTEBEATNESS number two (pwm) (cover of Keaton Monger - This is Sparta! Last techno remix) (2023-07-22) 15360Hz https://www.reddit.com/r/bytebeat/comments/1567oiq/omg_teh_bytebeatness/
      output = 64 + ((t / 2 * (t & 16384 ? 16 / 3 : t & 32768 ? 416 / 93
                                                              : 5)
                        * (t & 65536 ? t & 4096 ? 3 / 2 : 1 : 1)
                      & 127)
                       + (t >> (t & 65536 ? 5 : 7) & 127)
                     & 128);
      break;
    case 6:
      // t>>t% (2023-12-14)
      output = ((t >> t % (t % 2 ? p0 : p1) | t >> t % (t & 16384 ? (p2 + 4) : p2)) & 65535) / (t % 4096);
      break;
    case 7:
      int T = t / 5000;
      float melody[] = { 0, 1, 5, 8, 10 };
      output = t * pow(2, 2.0 + melody[T % 5] / 12.0);  // .0 to make sure it's a float division
      output = output & 127;
      break;
    default:
      output = 0;
      break;
  }
}

// Electrohouse bank
void bank5() {
  switch (func) {
    case 1:
      // tejeez 2011-10-05 #countercomplex
      output = (~t >> 2) * ((127 & t * (7 & t >> 10)) < (245 & t * (2 + (5 & t >> p0))));
      break;
    case 2:
      // Electrohouse 43c+ http://arhivach.ng/thread/28592/#71681785
      output = t >> t % (t % 2 ? t & 32768 ? 41 : t & 16384 ? 51
                                                            : 61
                               : 34)
               & t >> 4;
      break;
    case 3:
      // generic 3/4 buildup by yumeji (2011-10-03) https://www.pouet.net/topic.php?which=8357&page=6#c388717
      output = t & (p0 << 5) ? t >> 4 : t >> p2;
      break;
    case 4:

      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      output = 0;
      break;
  }
}

// Unsorted
void bank6() {
  switch (func) {
    case 1:
      a = constrain(p0, aMin, aMax);
      b = constrain(p2, bMin, bMax);
      c = constrain(p1, cMin, cMax);

      // poetaster helicopters has some arps with b in the middle, various!
      output = ((t >> a) - (t >> a & t) + (t >> t & a)) + (t * ((t >> b) & c));
      break;
    case 2:
      cMax = 12;
      cMin = 1;
      aMax = 16;
      aMin = 1;
      bMax = 16;
      bMin = 1;
      a = constrain(p0, aMin, aMax);
      b = constrain(p1, bMin, bMax);
      c = constrain(p2, cMin, cMax);
      if (_t > 65536)
        _t = -65536;

      // poetaster breaky, jungle stuff a at one oclock, c middle, etc
      output = a + ((t >> a + 1)) * (t >> c | b | t >> (t >> 16));
      break;
    case 3:
      aMax = 15;
      aMin = 0;
      bMax = 15;
      bMin = 0;
      cMax = 5;
      cMin = 0;

      a = constrain(p0, aMin, aMax);
      b = constrain(p1, bMin, bMax);
      c = constrain(p2, cMin, cMax);

      //  poetaster click mouth harp and hum and other chaos, clicky too :)
      output = ((t >> 6 ? 2 : 3) & t * (t >> a) | (a + b + c) - (t >> b)) % (t >> a) + (a << t | (t >> c));
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      output = 0;
      break;
  }
}

void bank7() {
  switch (func) {
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      output = 0;
      break;
  }
}