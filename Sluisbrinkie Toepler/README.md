# Sluisbrinkie Toepler
The Toepler (named after a German scientist) is a digital module for 3U Eurorack systems, positioned as a software development platform. The core of the module consists of a Atmega32u4 and is compatible with Arduino and the well-known Mozzi Library. The size is 14 HP. Initially I made this module for myself. I wanted a development platform with which I could experiment with various Arduino libraries and use them in a broad context and also interface. e broad range of connectivity allows it to be used as a swiss army knife in a wide range of situations. MIDI and Mozzi also allow the module to be truly polyphonic. The most obvious use case is a digital VCO but audio effects such as a Wavefolder, Clipper, Chorus or Pitch Shifter are also possible.

![20231115_192806](https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/faaf6aef-b2a3-4863-9b0b-0fc373ffbd65)

Overview of connections:
![20240127_094107](https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/52aae7f0-b71f-4000-a554-3affe9406e22)

- 1x USB Type B (for USB MIDI and flashing firmware)
- 1x MIDI TRS In
- 1x MIDI TRS Out / Thru (selectable from firmware)
- 3x Button (one of which is a reset)
- 4x LEDs (1x Power, 3x PWM Controllable)
- 2x Trigger In
- 1x Trigger Out
- 4x CV In (3 of which have an scale and offset, 4th can also be used as audio in)
- 1x Audio Out (14-bit Dual PWM, DC-coupled)

In case you bought a PCB (with assembled SMD components) and panel from me, there are a number of things you need to finish the build:
- 8x Thonkiconn mono jacks
- 2x Thonkiconn Stereo jack
- 3x pushbuttons with at least 15mm shaft
- 3x 100kOhm potentiometer (Bourns PVT09 series are used in my builds)
- 3x 10kOhm potentiometer (Bourns PVT09 series are used in my builds)
- 6x 3mm LED (color of choice)
- 1x USB-B KUSBVX-BS1N-B from Kycon
- 1x 2x3 pinheader (for flashing the bootloader)
- 1x 2x7 pinheader (for eurorack power supply)

One point of attention: the orientation of the bottom 3 potentiometers is flipped. Be sure to get Reversed D Shaft knobs or shafts where the orientation doesn't matter.

## Writing software
### Flashing the firmware
The bootloader has to be flashed using the Arduino IDE and an ISP programmer. After that, a USB-B cable can be used to flash the microcontroller. The Toepler will show up as 'Arduino Micro' (as they are based around the same microcontroller).
### Writing software
The Software folder contains a number of examples that I wrote or ported over. I would recommend to start with the sketches in Hardware Test to verify that everything was built correctly. After that, install the Mozzi library and hop over to the Mozzi examples and try some of those.
The Software map contains a header file 'PinDef.h' that maps the Toepler pins to an Arduino Micro number. This means you can 
### A word about voltage levels
The voltage levels of the microcontroller are 0-5V. The Audio Output is DC-Coupled and between 0 and 5V. The CV inputs are -5V to 5V and are mapped to a 0-5V range using attenuation and offset. This is especially important for CV4, in case you are using it for pitch input. The microcontroller essentially sees 1/2Voct and you need to compensate for this in software. Also, a 0V input will give an ADC reading of about 512.

## Things that are useful to know
### Tips for assembly
- Save the LED for last and initially solder just one leg, then you can easily line up the LED with the hole. The hole is a bit bigger than needed because I think that that looks nicer. I also think it looks nicer when the LED is a little 'sunk' and doesn't extend above the frontpanel but that's up to you of course.
- If you use the boxed header (like I did), it's slightly bigger than the footprint but it should still fit. Be sure to check the polarity before soldering it to the board.

![20231115_192708-EDIT](https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/a5d1d355-d6c3-440f-9783-a198386a5951)

