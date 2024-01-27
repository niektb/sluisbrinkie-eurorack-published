# Sluisbrinkie Kepler
The Kepler (named after a German scientist) is a digital module for 3U Eurorack systems. This module is my take at porting the Bastl Kastle Arp to Eurorack. The size of this module is about 14HP (it's a little bit smaller than most 14HP modules).

![Fully built module](https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/5b2d75d8-9da9-4ddc-9673-966e0306e4b7)

In case you bought a PCB (with assembled SMD components) and panel from me, there are a number of things you need to finish the build:
- 15x Thonkiconn mono jacks
- 8x 100kOhm potentiometer 
- 1x 3mm LED (color of choice)
- 2x 2x3 pinheader (for flashing the firmware)
- 1x 2x7 pinheader (for eurorack power supply)
- Eurorack power cable

## Flashing the firmware
The firmware has to be flashed using the Arduino IDE and an ISP programmer. The firmware for the LFO Chip and VCO chip can be found in the Software folder (check the silkscreen on the PCB to see which chip is which). [Instructions on how to flash](https://highlowtech.org/?p=1695).

## Getting started with patching
Right after flashing and powering up, the Kepler already can make sound without any patch cables. This is because some things are already connected together using the switched jack mechanism. There is a little block diagram at the top of the module which shows this. TRI > WS, PULSE > TIMBRE, RUNGLER OUT > PITCH.

![20240106_120326](https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/eaca5b53-44c1-4c91-8427-d68ba5e6b84a)

To understand how the internals work, I highly recommend the [Official ARP Manual](https://bastl-instruments.com/content/files/manual-kastle-arp.pdf) and the
[Kastle ARP Walkthrough](https://www.youtube.com/watch?v=Qxb1zNuFSnc) on Youtube. An external (passive) multiple is highly recommended if you want to try patches for the original ARP. Things can get out of hand quite quickly!

## Things that are useful to know
### Tips for assembly
- Save the LED for last and initially solder just one leg, then you can easily line up the LED with the hole. The hole is a bit bigger than needed because I think that that looks nicer. I also think it looks nicer when the LED is a little 'sunk' and doesn't extend above the frontpanel.
- If you use the boxed header (like I did), it's slightly bigger than the footprint but it should still fit. Be sure to check the polarity before soldering it to the board.

### Frontpanel naming
Initialy I planned to use the Kastle Synth firmware, which was also the case when I designed the frontpanel. So I looked at the 'conventional' Kastle Synth for the naming, not the Kastle ARP. But then I discovered the ARP whose sound I like tons more. These are the name mappings:
PITCH > NOTE,
WS > DECAY,
SQR > BASS,
RESET > CLK IN,
LFO > TEMPO,
MODE > CHORD.
Update 27-01-2024: I've also made an ARP Frontpanel, slightly smaller at 12HP. This clarifies the mapping also a bit more.

![20240127_114050](https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/c6d7d34a-cf09-471c-908f-455df3c375b1)

### Potentiometers and knob size
Be aware: the potmeters are fitted quite close together so big knobs won't fit. For reference, the knobs used on the photo above are Black Small Unskirted Intellijel/Sifam Plastic Knobs which have a diameter of 11.5mm. I use the Bourns PVT09 series potentiometers in my builds, they are verified to fit.

### A word about voltage levels
The voltage levels of this module are 0-5V, both in- and output. Negative CV Voltages will be clipped away. The Audio outputs are also DC-coupled. This is also the case for the 'MIX OR OUT'. This can be used as a passive mixer to mix in audio with the main oscillator but you should not apply a bipolar audio signal to this input (only modules that have a DC-coupled 0-5V output such as the Kepler (or Toepler, which is one of my other modules). On the upside, the SQR output can be used to modulate the CV inputs (yes, this actually works great!)
