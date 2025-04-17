/*
"PWM-SAW" v2.0 (now with VCA or VCF!) by Kassen Oud

LICENSE:
This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

DESCRIPION;
	SAW oscillator for Ginkosynthese's "Grains" module with a VCA or
	optional VFC build in.
MANUAL;
	Knob 1 / mod 1; PWM modulation
		Gives you a variable amount of PWM like in the Juno2 "PWM-Saw".
		In reality this is more like a saw faded in and out by a square.
		You don't need to use it for "hoover" sounds if those offend your
		sensibilities. If a modulation source gets connected the knob
		attentuates this.
	Knob 2/ mod 2:
		Sets the volume to save you a VCA. If you uncomment the line
		saying "#define FILTER" this instead sets a simplelow-pass filter
		and with that the volume. At its lowest setting this filter will
		leave a DC-offset, but the Grains's output should remove that.
	Knob 3 / mod 3: Frequency
		The modulation input modulates the frequency, with the related
		knob setting an offset (for your tuning). This attempts to be
		tuned to a Volt per octave (not yet sure how succesful that is).

	This code owes considerable debt to Jan Willem Hagenbeek's original
	"Grains" code, the people online analysing the "hoover" sound and the
	Juno2's "PWM-Saw" wave. Jan Willem didn't elect to pick a FOSS
	license. I still wanted to publish this under a GPL2 license so
	people like yourself would feel more confident adapting this and
	hopefully sharing their results.

	Rationale for the change of having input "2" x-fade to noise to the
	current behaviour; I started work on a dedicated noise module and
	like the Unix idea of "do one thing and do it well". Also' JW asked
	me to save him a VCA :-).
	Yours, Kas.
*/

//uncomment this line to have input 2 act as a LPF
#define FILTER

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

uint16_t phase_accumulator;		//keeps track of the phase
uint16_t phase_inc;				//phase increase per sample
uint16_t mod_value;				//value of the pulse-width modulation
uint16_t mix_A;					//volume of the saw wave
uint16_t mix_B;					//volume of the noise
uint16_t output = 0;			//output we'll listen to
uint16_t last_out = 0;				//previous output
bool cut = 0;					//whether we'll output 0
bool last_cut = 0;				//whether we did last sample

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

// Map Analogue channels
#define FREQ_CONTROL		CV4_PIN
#define FREQ_OFSET			CV1_PIN
#define MOD_CONTROL			CV2_PIN
#define GAIN_CONTROL		CV3_PIN

// Changing these will also requires rewriting setup()
//    Output is on pin 11
#define PWM_PIN 9
#define PWM_VALUE OCR4B
#define PWM_INTERRUPT TIMER4_OVF_vect

//maps cv to samples of phase increase each time we output a new value
//this might need tuning in the future
const uint16_t freqTable[] PROGMEM = {
  69,  69,  69,  69,  70,  70,  70,  70,  70,  71,			//   0 to 9
  71,  71,  71,  72,  72,  72,  72,  73,  73,  73,			//  
  73,  74,  74,  74,  74,  75,  75,  75,  75,  76,			//  
  76,  76,  76,  77,  77,  77,  77,  78,  78,  78,			//  
  78,  79,  79,  79,  79,  80,  80,  80,  81,  81,			//  
  81,  81,  82,  82,  82,  82,  83,  83,  83,  84,			//  
  84,  84,  84,  85,  85,  85,  86,  86,  86,  86,			//  
  87,  87,  87,  88,  88,  88,  88,  89,  89,  89,			//  
  90,  90,  90,  91,  91,  91,  91,  92,  92,  92,			//  
  93,  93,  93,  94,  94,  94,  95,  95,  95,  96,			//  
  96,  96,  97,  97,  97,  97,  98,  98,  98,  99,			//  100 to 109
  99,  99, 100, 100, 100, 101, 101, 101, 102, 102,			//  
 102, 103, 103, 104, 104, 104, 105, 105, 105, 106,			//  
 106, 106, 107, 107, 107, 108, 108, 109, 109, 109,			//  
 110, 110, 110, 111, 111, 111, 112, 112, 113, 113,			//  
 113, 114, 114, 114, 115, 115, 116, 116, 116, 117,			//  
 117, 118, 118, 118, 119, 119, 120, 120, 120, 121,			//  
 121, 122, 122, 122, 123, 123, 124, 124, 124, 125,			//  
 125, 126, 126, 127, 127, 127, 128, 128, 129, 129,			//  
 130, 130, 130, 131, 131, 132, 132, 133, 133, 134,			//  
 134, 134, 135, 135, 136, 136, 137, 137, 138, 138,			//  200 to 209
 139, 139, 139, 140, 140, 141, 141, 142, 142, 143,			//  
 143, 144, 144, 145, 145, 146, 146, 147, 147, 148,			//  
 148, 149, 149, 150, 150, 151, 151, 152, 152, 153,			//  
 153, 154, 154, 155, 155, 156, 156, 157, 157, 158,			//  
 158, 159, 159, 160, 161, 161, 162, 162, 163, 163,			//  
 164, 164, 165, 165, 166, 167, 167, 168, 168, 169,			//  
 169, 170, 171, 171, 172, 172, 173, 173, 174, 175,			//  
 175, 176, 176, 177, 178, 178, 179, 179, 180, 181,			//  
 181, 182, 182, 183, 184, 184, 185, 185, 186, 187,			//  
 187, 188, 189, 189, 190, 190, 191, 192, 192, 193,			//  300 to 309
 194, 194, 195, 196, 196, 197, 198, 198, 199, 200,			//  
 200, 201, 202, 202, 203, 204, 204, 205, 206, 206,			//  
 207, 208, 208, 209, 210, 211, 211, 212, 213, 213,			//  
 214, 215, 216, 216, 217, 218, 218, 219, 220, 221,			//  
 221, 222, 223, 224, 224, 225, 226, 227, 227, 228,			//  
 229, 230, 231, 231, 232, 233, 234, 234, 235, 236,			//  
 237, 238, 238, 239, 240, 241, 242, 242, 243, 244,			//  
 245, 246, 246, 247, 248, 249, 250, 251, 251, 252,			//  
 253, 254, 255, 256, 257, 257, 258, 259, 260, 261,			//  
 262, 263, 264, 264, 265, 266, 267, 268, 269, 270,			//  400 to 409
 271, 272, 273, 273, 274, 275, 276, 277, 278, 279,			//  
 280, 281, 282, 283, 284, 285, 286, 287, 288, 288,			//  
 289, 290, 291, 292, 293, 294, 295, 296, 297, 298,			//  
 299, 300, 301, 302, 303, 304, 305, 306, 307, 308,			//  
 310, 311, 312, 313, 314, 315, 316, 317, 318, 319,			//  
 320, 321, 322, 323, 324, 325, 327, 328, 329, 330,			//  
 331, 332, 333, 334, 335, 337, 338, 339, 340, 341,			//  
 342, 343, 345, 346, 347, 348, 349, 350, 352, 353,			//  
 354, 355, 356, 357, 359, 360, 361, 362, 363, 365,			//  
 366, 367, 368, 370, 371, 372, 373, 375, 376, 377,			//  500 to 509
 378, 380, 381, 382, 383, 385, 386, 387, 389, 390,			//  
 391, 393, 394, 395, 397, 398, 399, 401, 402, 403,			//  
 405, 406, 407, 409, 410, 411, 413, 414, 416, 417,			//  
 418, 420, 421, 423, 424, 425, 427, 428, 430, 431,			//  
 433, 434, 436, 437, 438, 440, 441, 443, 444, 446,			//  
 447, 449, 450, 452, 453, 455, 456, 458, 460, 461,			//  
 463, 464, 466, 467, 469, 470, 472, 474, 475, 477,			//  
 478, 480, 482, 483, 485, 486, 488, 490, 491, 493,			//  
 495, 496, 498, 500, 501, 503, 505, 506, 508, 510,			//  
 511, 513, 515, 517, 518, 520, 522, 524, 525, 527,			//  600 to 609
 529, 531, 532, 534, 536, 538, 540, 541, 543, 545,			//  
 547, 549, 551, 552, 554, 556, 558, 560, 562, 564,			//  
 566, 567, 569, 571, 573, 575, 577, 579, 581, 583,			//  
 585, 587, 589, 591, 593, 595, 597, 599, 601, 603,			//  
 605, 607, 609, 611, 613, 615, 617, 619, 621, 623,			//  
 625, 627, 630, 632, 634, 636, 638, 640, 642, 644,			//  
 647, 649, 651, 653, 655, 658, 660, 662, 664, 666,			//  
 669, 671, 673, 675, 678, 680, 682, 685, 687, 689,			//  
 691, 694, 696, 698, 701, 703, 705, 708, 710, 713,			//  
 715, 717, 720, 722, 725, 727, 729, 732, 734, 737,			//  700 to 709
 739, 742, 744, 747, 749, 752, 754, 757, 759, 762,			//  
 764, 767, 770, 772, 775, 777, 780, 783, 785, 788,			//  
 791, 793, 796, 799, 801, 804, 807, 809, 812, 815,			//  
 817, 820, 823, 826, 828, 831, 834, 837, 840, 842,			//  
 845, 848, 851, 854, 857, 860, 862, 865, 868, 871,			//  
 874, 877, 880, 883, 886, 889, 892, 895, 898, 901,			//  
 904, 907, 910, 913, 916, 919, 922, 925, 928, 932,			//  
 935, 938, 941, 944, 947, 950, 954, 957, 960, 963,			//  
 966, 970, 973, 976, 979, 983, 986, 989, 993, 996,			//  
 999,1003,1006,1009,1013,1016,1020,1023,1027,1030,			//  800 to 809
1033,1037,1040,1044,1047,1051,1054,1058,1061,1065,			//  
1069,1072,1076,1079,1083,1087,1090,1094,1098,1101,			//  
1105,1109,1112,1116,1120,1124,1127,1131,1135,1139,			//  
1143,1146,1150,1154,1158,1162,1166,1170,1174,1178,			//  
1182,1186,1189,1193,1197,1201,1206,1210,1214,1218,			//  
1222,1226,1230,1234,1238,1242,1247,1251,1255,1259,			//  
1263,1268,1272,1276,1280,1285,1289,1293,1298,1302,			//  
1306,1311,1315,1320,1324,1328,1333,1337,1342,1346,			//  
1351,1355,1360,1365,1369,1374,1378,1383,1388,1392,			//  
1397,1402,1406,1411,1416,1421,1425,1430,1435,1440,			//  900 to 909
1444,1449,1454,1459,1464,1469,1474,1479,1484,1489,			//  
1494,1499,1504,1509,1514,1519,1524,1529,1534,1539,			//  
1545,1550,1555,1560,1565,1571,1576,1581,1587,1592,			//  
1597,1603,1608,1613,1619,1624,1630,1635,1641,1646,			//  
1652,1657,1663,1668,1674,1679,1685,1691,1696,1702,			//  
1708,1714,1719,1725,1731,1737,1742,1748,1754,1760,			//  
1766,1772,1778,1784,1790,1796,1802,1808,1814,1820,			//  
1826,1832,1838,1845,1851,1857,1863,1869,1876,1882,			//  
1888,1895,1901,1907,1914,1920,1927,1933,1940,1946,			//  
1953,1959,1966,1972,1979,1986,1992,1999,2006,2012,			//  1000 to 1009
2019,2026,2033,2040,2046,2053,2060,2067,2074,2081,			//  1010 to 1019
2088,2095,2102,2109                              ,			//  1020 to 1023
};

uint16_t mapFreq(uint16_t input)
{
	return pgm_read_word_near(freqTable + input);
}



//sets up pins and configures the samplerate and frequency of the PWM output
void setup()
{
  TCCR4A = _BV(COM4B0) | _BV(PWM4B);
  TCCR4B = _BV(CS40);
  TCCR4C = _BV(COM4B0S);
  TCCR4D = _BV(WGM40);
  TIMSK4 = _BV(TOIE4);
	pinMode(PWM_PIN,OUTPUT);
}

//reads modulation inputs
void loop()
{
	//sum the frequency modulatio with the tuning offset
	int pwmv = min( analogRead(FREQ_CONTROL) + analogRead(FREQ_OFSET), 1023);
	phase_inc = mapFreq(pwmv);
	//mod value needs to be 15 bits as it "cuts" the output twice per cycle and one cycle is 16 bit
	mod_value = analogRead(MOD_CONTROL);	//10 bits
	mod_value = mod_value << 5;			//15 bits
	//by reducing the bit-depth of the noise mix to 8 bits we get to mix the final output in a way
	//that preserves our saw's resolution at 8 bits, which is the most we can get out of the method
	//that the Grains uses for audio output.
	mix_B = analogRead(GAIN_CONTROL) >> 2;
	mix_A = 255 - mix_B;
}

//actual work happens here, triggered by an interupt for each output value
SIGNAL(PWM_INTERRUPT)
{
	//start by increasing the phase
	phase_accumulator += phase_inc;
	//note we ignore the most signifficant bit here, to make the PWM x-fade to 0 twice per
	//saw cycle, like the Juno did.
	if ((phase_accumulator & 0b0111111111111111) < mod_value) cut = true;
	else cut = false;
	//Now we start calculating the actual output. There are several cases
	//case 1; when the phase wraps around, creating a jump from the maximal to the minmal output
	//here we need to smooth a bit to prevent notable aliassing
	if (phase_accumulator < phase_inc)
	{
		// uncomment this line to turn the noise into tuned noise by resetting the random
		//generator by periodically resetting the seed
		//rand_seed = 1;

		//insert one sample at "0" at the vertical flank as the phase wraps against aliassing
		output = 32768;
	}
	//case 2; when the PWM goes from "on" to "off", creating another need for interpolation
	else if (cut != last_cut)
	{
		//interpolate pwm flanks against aliassing. Since we are averaging between
		//a value and 0 we can simply halve the value, we do this the cheap way.
		output = phase_accumulator >> 1;
		last_cut = cut;
	}
	//case 3: when the PWM sets the output to 0
	else if (cut)
	{
		output = (uint16_t)0;
	}
	//finally case 4; where the output is simply equal to the phase
	else
	{
		output = phase_accumulator;
	}
#ifdef FILTER
	//x-fade between the last generated sample and the last utput for a simple LPF
	output = (last_out * mix_A) + ((output >> 8) * mix_B);
#else
	output = (output >> 8) * mix_B;
#endif
	//reduce the output to 8 bit.
	output = output >> 8;
	// Output to PWM (this is faster than using analogWrite)  
	PWM_VALUE = output;
#ifdef FILTER
	last_out = output;
#endif
}
