// Wavegrains V2.0
//
// based on the Auduino code by Peter Knight, revised and finetuned by Ginkosynthese for use with cv inputs.
// Change to a wavetable oscillator by Janne G:son Berg (jgb @ muffwiggler)
// Version for PCB V2
//
// inputs
// Analog in 0: Pitch CV 0-5 V
// Analog in 1: Wavetable select 0-5 V
// Analog in 2: Sweep between two wavetables 0-5 V
// output
// Digital 11: Audio out (PWM)


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

uint16_t syncPhaseAcc;
uint16_t syncPhaseInc;
uint8_t wavetableStep;
uint8_t wavetableA;
uint8_t wavetableB;
uint16_t sweepPosition;

#define PC7 13
#define CV4_PIN PF7
#define CV3_PIN PF6
#define CV2_PIN PF5
#define CV1_PIN PF4
#define PF7 A0
#define PF6 A1
#define PF5 A2
#define PF4 A3
#define PF1 A4
#define PF0 A5

// Define wavetable parameters
#define WT_LENGTH 16
#define WT_NO_OF 32
#define WT_POT_SCALING 5

// Map Analogue channels
#define FREQUENCY CV4_PIN
#define WAVETABLE_SELECT CV2_PIN
#define SWEEP CV3_PIN
#define FREQUENCY_OFFSET CV1_PIN

// Changing these will also requires rewriting audioOn()
#define PWM_PIN 9
#define PWM_VALUE OCR4B
#define LED_PIN PC7
#define LED_PORT PORTC
#define LED_BIT 7
#define PWM_INTERRUPT TIMER4_OVF_vect

// For converting incoming CV to pitch
const uint16_t freqTable[] PROGMEM = {
  549, 550, 552, 554, 556, 557, 559, 561, 563, 565, 566, 568, 570, 572, 574, 575, 577, 579, 581, 583, 585, 586, 588, 590, 592, 594, 596, 598, 600, 601, 603, 605, 607, 609, 611, 613, 615, 617, 619, 621, 623, 625, 627, 629, 631, 633, 635, 637, 639, 641, 643, 645, 647, 649, 651, 653, 655, 657, 659, 661, 663, 666, 668, 670, 672, 674, 676, 678, 680, 683, 685, 687, 689, 691, 693, 696, 698, 700, 702, 705, 707, 709, 711, 714, 716, 718, 720, 723, 725, 727, 729, 732, 734, 736, 739, 741, 743, 746, 748, 751, 753, 755, 758, 760, 763, 765, 767, 770, 772, 775, 777, 780, 782, 785, 787, 790, 792, 795, 797, 800, 802, 805, 807, 810, 812, 815, 818, 820, 823, 825, 828, 831, 833, 836, 838, 841, 844, 846, 849, 852, 855, 857, 860, 863, 865, 868, 871, 874, 876, 879, 882, 885, 888, 890, 893, 896, 899, 902, 905, 908, 910, 913, 916, 919, 922, 925, 928, 931, 934, 937, 940, 943, 946, 949, 952, 955, 958, 961, 964, 967, 970, 973, 976, 979, 982, 985, 989, 992, 995, 998, 1001, 1004, 1007, 1011, 1014, 1017, 1020, 1024, 1027, 1030, 1033, 1037, 1040, 1043, 1046, 1050, 1053, 1056, 1060, 1063, 1067, 1070, 1073, 1077, 1080, 1084, 1087, 1090, 1094, 1097, 1101, 1104, 1108, 1111, 1115, 1118, 1122, 1125, 1129, 1133, 1136, 1140, 1143, 1147, 1151, 1154, 1158, 1162, 1165, 1169, 1173, 1176, 1180, 1184, 1188, 1191, 1195, 1199, 1203, 1207, 1210, 1214, 1218, 1222, 1226, 1230, 1234, 1238, 1242, 1245, 1249, 1253, 1257, 1261, 1265, 1269, 1273, 1277, 1281, 1285, 1290, 1294, 1298, 1302, 1306, 1310, 1314, 1318, 1323, 1327, 1331, 1335, 1339, 1344, 1348, 1352, 1357, 1361, 1365, 1369, 1374, 1378, 1383, 1387, 1391, 1396, 1400, 1405, 1409, 1414, 1418, 1423, 1427, 1432, 1436, 1441, 1445, 1450, 1454, 1459, 1464, 1468, 1473, 1478, 1482, 1487, 1492, 1496, 1501, 1506, 1511, 1515, 1520, 1525, 1530, 1535, 1540, 1545, 1549, 1554, 1559, 1564, 1569, 1574, 1579, 1584, 1589, 1594, 1599, 1604, 1609, 1614, 1620, 1625, 1630, 1635, 1640, 1645, 1651, 1656, 1661, 1666, 1672, 1677, 1682, 1688, 1693, 1698, 1704, 1709, 1715, 1720, 1725, 1731, 1736, 1742, 1747, 1753, 1759, 1764, 1770, 1775, 1781, 1787, 1792, 1798, 1804, 1809, 1815, 1821, 1827, 1832, 1838, 1844, 1850, 1856, 1862, 1868, 1873, 1879, 1885, 1891, 1897, 1903, 1909, 1915, 1921, 1928, 1934, 1940, 1946, 1952, 1958, 1965, 1971, 1977, 1983, 1990, 1996, 2002, 2009, 2015, 2021, 2028, 2034, 2041, 2047, 2054, 2060, 2067, 2073, 2080, 2086, 2093, 2100, 2106, 2113, 2120, 2126, 2133, 2140, 2147, 2153, 2160, 2167, 2174, 2181, 2188, 2195, 2202, 2209, 2216, 2223, 2230, 2237, 2244, 2251, 2258, 2265, 2272, 2280, 2287, 2294, 2301, 2309, 2316, 2323, 2331, 2338, 2346, 2353, 2360, 2368, 2375, 2383, 2390, 2398, 2406, 2413, 2421, 2429, 2436, 2444, 2452, 2460, 2467, 2475, 2483, 2491, 2499, 2507, 2515, 2523, 2531, 2539, 2547, 2555, 2563, 2571, 2579, 2587, 2596, 2604, 2612, 2620, 2629, 2637, 2645, 2654, 2662, 2671, 2679, 2687, 2696, 2705, 2713, 2722, 2730, 2739, 2748, 2756, 2765, 2774, 2783, 2791, 2800, 2809, 2818, 2827, 2836, 2845, 2854, 2863, 2872, 2881, 2890, 2900, 2909, 2918, 2927, 2937, 2946, 2955, 2965, 2974, 2983, 2993, 3002, 3012, 3021, 3031, 3041, 3050, 3060, 3070, 3079, 3089, 3099, 3109, 3119, 3128, 3138, 3148, 3158, 3168, 3178, 3188, 3198, 3209, 3219, 3229, 3239, 3249, 3260, 3270, 3280, 3291, 3301, 3312, 3322, 3333, 3343, 3354, 3365, 3375, 3386, 3397, 3407, 3418, 3429, 3440, 3451, 3462, 3473, 3484, 3495, 3506, 3517, 3528, 3539, 3551, 3562, 3573, 3584, 3596, 3607, 3619, 3630, 3642, 3653, 3665, 3676, 3688, 3700, 3711, 3723, 3735, 3747, 3759, 3771, 3783, 3795, 3807, 3819, 3831, 3843, 3855, 3867, 3880, 3892, 3904, 3917, 3929, 3942, 3954, 3967, 3979, 3992, 4004, 4017, 4030, 4043, 4055, 4068, 4081, 4094, 4107, 4120, 4133, 4146, 4159, 4173, 4186, 4199, 4212, 4226, 4239, 4253, 4266, 4280, 4293, 4307, 4320, 4334, 4348, 4362, 4375, 4389, 4403, 4417, 4431, 4445, 4459, 4474, 4488, 4502, 4516, 4530, 4545, 4559, 4574, 4588, 4603, 4617, 4632, 4647, 4661, 4676, 4691, 4706, 4721, 4736, 4751, 4766, 4781, 4796, 4811, 4827, 4842, 4857, 4873, 4888, 4904, 4919, 4935, 4950, 4966, 4982, 4998, 5013, 5029, 5045, 5061, 5077, 5093, 5110, 5126, 5142, 5158, 5175, 5191, 5207, 5224, 5241, 5257, 5274, 5291, 5307, 5324, 5341, 5358, 5375, 5392, 5409, 5426, 5443, 5461, 5478, 5495, 5513, 5530, 5548, 5565, 5583, 5601, 5618, 5636, 5654, 5672, 5690, 5708, 5726, 5744, 5763, 5781, 5799, 5818, 5836, 5854, 5873, 5892, 5910, 5929, 5948, 5967, 5986, 6005, 6024, 6043, 6062, 6081, 6100, 6120, 6139, 6159, 6178, 6198, 6217, 6237, 6257, 6277, 6297, 6316, 6337, 6357, 6377, 6397, 6417, 6438, 6458, 6478, 6499, 6520, 6540, 6561, 6582, 6603, 6624, 6645, 6666, 6687, 6708, 6729, 6751, 6772, 6793, 6815, 6837, 6858, 6880, 6902, 6924, 6946, 6968, 6990, 7012, 7034, 7056, 7079, 7101, 7124, 7146, 7169, 7192, 7215, 7237, 7260, 7283, 7306, 7330, 7353, 7376, 7400, 7423, 7447, 7470, 7494, 7518, 7541, 7565, 7589, 7613, 7637, 7662, 7686, 7710, 7735, 7759, 7784, 7809, 7833, 7858, 7883, 7908, 7933, 7958, 7983, 8009, 8034, 8060, 8085, 8111, 8137, 8162, 8188, 8214, 8240, 8266, 8293, 8319, 8345, 8372, 8398, 8425, 8452, 8478, 8505, 8532, 8559, 8586, 8614, 8641, 8668, 8696, 8723, 8751, 8779, 8807, 8834, 8862, 8891, 8919, 8947, 8975, 9004, 9032, 9061, 9090, 9119, 9147, 9176, 9206, 9235, 9264, 9293, 9323, 9352, 9382, 9412, 9442, 9472, 9502, 9532, 9562, 9592, 9623, 9653, 9684, 9714, 9745, 9776, 9807, 9838, 9869, 9901, 9932, 9964, 9995, 10027, 10059, 10090, 10122, 10155, 10187, 10219, 10251, 10284, 10317, 10349, 10382, 10415, 10448, 10481, 10514, 10548, 10581, 10615, 10648, 10682, 10716, 10750, 10784, 10818, 10852, 10887, 10921, 10956, 10991, 11026, 11060, 11096, 11131, 11166, 11201, 11237, 11273, 11308, 11344, 11380, 11416, 11452, 11489, 11525, 11562, 11598, 11635, 11672, 11709, 11746, 11783, 11821, 11858, 11896, 11933, 11971, 12009, 12047, 12085, 12124, 12162, 12201, 12239, 12278, 12317, 12356, 12395, 12435, 12474, 12514, 12553, 12593, 12633, 12673, 12713, 12753, 12794, 12834, 12875, 12916, 12957, 12998, 13039, 13081, 13122, 13164, 13205, 13247, 13289, 13331, 13374, 13416, 13458, 13501, 13544, 13587, 13630, 13673, 13717, 13760, 13804, 13847, 13891, 13935, 13979
};

// Wavetables, to be filled in by randomization and some precalculated waveforms
uint16_t wavetable[WT_NO_OF][WT_LENGTH];

uint16_t mapFreq(uint16_t input) {
  return (pgm_read_word_near(freqTable + input));
}

void audioOn() {
  TCCR4A = _BV(COM4B0) | _BV(PWM4B);
  TCCR4B = _BV(CS40);
  TCCR4C = _BV(COM4B0S);
  TCCR4D = _BV(WGM40);
  TIMSK4 = _BV(TOIE4);
}


void setup() {
  pinMode(PWM_PIN, OUTPUT);
  audioOn();
  pinMode(LED_PIN, OUTPUT);
  // Change random seed value to change the randomized wavetables
  randomSeed(123717284);
  int tmpTable[WT_LENGTH];
  uint16_t maxVal;

  for (int j = 0; j < WT_NO_OF; j++) {
    maxVal = 0;
    for (int i = 0; i < WT_LENGTH; i++) {
      tmpTable[i] = random(-32768, 32767);
      maxVal = abs(tmpTable[i]) > maxVal ? abs(tmpTable[i]) : maxVal;
    }
    // Normalize wavetables
    for (int i = 0; i < WT_LENGTH; i++) {
      wavetable[j][i] = (32768 * tmpTable[i]) / maxVal + 32768;
    }

    // Use up some more random values... totally unnecessary
    for (int i = 0; i < random(4); i++) {
      i == i;
    }
  }

  // Hardcoded wavetables sprinkled among the randomized ones

  // Square one octave up
  wavetable[0][0] = 65535;
  wavetable[0][1] = 65535;
  wavetable[0][2] = 65535;
  wavetable[0][3] = 65535;
  wavetable[0][4] = 0;
  wavetable[0][5] = 0;
  wavetable[0][6] = 0;
  wavetable[0][7] = 0;
  wavetable[0][8] = 65535;
  wavetable[0][9] = 65535;
  wavetable[0][10] = 65535;
  wavetable[0][11] = 65535;
  wavetable[0][12] = 0;
  wavetable[0][13] = 0;
  wavetable[0][14] = 0;
  wavetable[0][15] = 0;

  // Lowest square possible
  wavetable[4][0] = 65535;
  wavetable[4][1] = 65535;
  wavetable[4][2] = 65535;
  wavetable[4][3] = 65535;
  wavetable[4][4] = 65535;
  wavetable[4][5] = 65535;
  wavetable[4][6] = 65535;
  wavetable[4][7] = 65535;
  wavetable[4][8] = 0;
  wavetable[4][9] = 0;
  wavetable[4][10] = 0;
  wavetable[4][11] = 0;
  wavetable[4][12] = 0;
  wavetable[4][13] = 0;
  wavetable[4][14] = 0;
  wavetable[4][15] = 0;

  // Square with two extra peaks
  wavetable[8][0] = 65535;
  wavetable[8][1] = 65535;
  wavetable[8][2] = 65535;
  wavetable[8][3] = 65535;
  wavetable[8][4] = 65535;
  wavetable[8][5] = 65535;
  wavetable[8][6] = 65535;
  wavetable[8][7] = 65535;
  wavetable[8][8] = 0;
  wavetable[8][9] = 65535;
  wavetable[8][10] = 0;
  wavetable[8][11] = 0;
  wavetable[8][12] = 65535;
  wavetable[8][13] = 0;
  wavetable[8][14] = 0;
  wavetable[8][15] = 0;

  // Somewhat random?
  wavetable[9][0] = 8345;
  wavetable[9][1] = 50000;
  wavetable[9][2] = 7643;
  wavetable[9][3] = 65535;
  wavetable[9][4] = 52000;
  wavetable[9][5] = 10000;
  wavetable[9][6] = 20000;
  wavetable[9][7] = 40000;
  wavetable[9][8] = 300;
  wavetable[9][9] = 5120;
  wavetable[9][10] = 60240;
  wavetable[9][11] = 2048;
  wavetable[9][12] = 65535;
  wavetable[9][13] = 0;
  wavetable[9][14] = 58755;
  wavetable[9][15] = 36289;

  // Sawtooth
  wavetable[16][0] = 0x0000;
  wavetable[16][1] = 0x1000;
  wavetable[16][2] = 0x2000;
  wavetable[16][3] = 0x3000;
  wavetable[16][4] = 0x4000;
  wavetable[16][5] = 0x5000;
  wavetable[16][6] = 0x6000;
  wavetable[16][7] = 0x7000;
  wavetable[16][8] = 0x8000;
  wavetable[16][9] = 0x9000;
  wavetable[16][10] = 0xa000;
  wavetable[16][11] = 0xb000;
  wavetable[16][12] = 0xc000;
  wavetable[16][13] = 0xd000;
  wavetable[16][14] = 0xe000;
  wavetable[16][15] = 0xf000;

  // Sine
  wavetable[24][0] = 128 << 8;
  wavetable[24][1] = 176 << 8;
  wavetable[24][2] = 218 << 8;
  wavetable[24][3] = 246 << 8;
  wavetable[24][4] = 255 << 8;
  wavetable[24][5] = 246 << 8;
  wavetable[24][6] = 218 << 8;
  wavetable[24][7] = 176 << 8;
  wavetable[24][8] = 128 << 8;
  wavetable[24][9] = 79 << 8;
  wavetable[24][10] = 37 << 8;
  wavetable[24][11] = 9 << 8;
  wavetable[24][12] = 0 << 8;
  wavetable[24][13] = 9 << 8;
  wavetable[24][14] = 37 << 8;
  wavetable[24][15] = 79 << 8;

  wavetableStep = 0;
}

void loop() {
  // Get CV in and convert it to the phase accumulator
  int pwmv = analogRead(FREQUENCY) + analogRead(FREQUENCY_OFFSET);
  if (pwmv > 1023) pwmv = 1023;
  if (pwmv < 0) pwmv = 0;
  syncPhaseInc = mapFreq(pwmv);

  // Read position of sweep
  sweepPosition = analogRead(SWEEP);

  // Read current wavetable and set up wavetables to morph between
  uint8_t readWavetable = analogRead(WAVETABLE_SELECT) >> WT_POT_SCALING;
  wavetableA = readWavetable;
  wavetableB = (readWavetable + 1) & 0x1f;
}

SIGNAL(PWM_INTERRUPT) {
  uint16_t output;
  uint16_t waveA;
  uint16_t waveB;
  uint32_t delta;
  bool aLarger;

  // Phase accumulator
  syncPhaseAcc += syncPhaseInc;
  if (syncPhaseAcc < syncPhaseInc) {
    // Time to increase the wavetable step
    wavetableStep++;
    wavetableStep &= WT_LENGTH - 1;
    LED_PORT ^= 1 << LED_BIT;  // Faster than using digitalWrite
  }

  // Calculate morphing between wavetables, keep delta value positive
  waveA = (wavetable[wavetableA][wavetableStep]);
  waveB = (wavetable[wavetableB][wavetableStep]);
  if (waveA >= waveB) {
    aLarger = true;
    delta = waveA - waveB;
  } else {
    aLarger = false;
    delta = waveB - waveA;
  }

  delta = (delta * sweepPosition) >> 10;
  output = aLarger ? waveA - delta : waveA + delta;

  // Output to PWM (this is faster than using analogWrite)
  // Scale down to 8 bit value
  PWM_VALUE = output >> 8;
}
