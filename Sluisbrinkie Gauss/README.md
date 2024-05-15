# Sluisbrinkie Gauss
The Gauss (named after a German scientist) is a digital module for 3U Eurorack systems. The core of the module consists of a Atmega32u4 and a MCP4822. The size of this module is 14 HP. It is aimed to be a LFO / (Cycling) Envelope Generator / Oscillator inspired by the well-known VCS modules such as Maths. But the software is easily hackable and can be customized to the user's liking.

<img src="https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/000707f4-be9d-4c8f-b388-08e5b87a3464" width="40%">
<img src="https://github.com/niektb/sluisbrinkie-eurorack-published/assets/1948785/16e08ec8-96af-435b-be8f-8634c0214ab7" width="40%">

Overview of connections:
- 1x USB Type B (for USB MIDI and flashing firmware)
- 2x Loop Switch
- 2x LEDs
- 2x Gate/Trigger In
- 3x Digital Out
- 4x CV In offset by the potmeters
- 2x DAC Out

In case you bought a PCB (with assembled SMD components) and panel from me, there are a number of things you need to finish the build:
- 11x Thonkiconn mono jacks
- 2x Subminiature Toggle Switches
- 4x 10kOhm Type A potentiometer (Bourns PVT09 series are used in my personal builds)
- 4x potentiometer knobs (see note below)
- 2x 3mm LED (color of choice)
- 1x USB-B KUSBVX-BS1N-B from Kycon
- 1x 2x3 pinheader (for flashing the bootloader)
- 1x 2x7 pinheader (for eurorack power supply)

## Writing software
### Flashing the firmware
The bootloader has to be flashed using the Arduino IDE and an ISP programmer. After that, a USB-B cable can be used to flash the microcontroller. The Toepler will show up as 'Arduino Micro' (as they are based around the same microcontroller).

### Writing software
The Software folder contains the example software that I wrote. I would recommend to start with this sketch and expand on this.
This folder contains a header file 'PinDef.h' that maps the Gauss pins to an Arduino Micro number so you don't have to figure out the pin mapping.

### A word about voltage levels
The voltage levels of the microcontroller are 0-5V. The DAC Output is amplified and ranges between -5V and 5V. The CV inputs are -5V to 5V and are mapped to a 0-5V range using attenuation and offset. The microcontroller essentially sees 1/2Voct and you need to compensate for this in software if you plan to try Pitch Tracking. Also, a 0V input will give an ADC reading of about 512.

## Things that are useful to know
### Tips for assembly
- Save the LED for last and initially solder just one leg, then you can easily line up the LED with the hole. The hole is a bit bigger than needed because I think that that looks nicer. I also think it looks nicer when the LED is a little 'sunk' and doesn't extend above the frontpanel but that's up to you of course.
- If you use the boxed header (like I did), it's slightly bigger than the footprint but it should still fit. Be sure to check the polarity before soldering it to the board.
