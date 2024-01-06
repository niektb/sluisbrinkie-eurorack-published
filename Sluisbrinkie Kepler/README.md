# Sluisbrinkie Kepler
The Kepler (named after a German scientist) is a digital module for 3U Eurorack systems. This module is my take at porting the Bastl Kastle Arp to Eurorack. The size of this module is about 14HP.

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

## Things that are useful to know:
### Potentiometer Knob size
Be aware: the potmeters are fitted quite close together so big knobs won't fit. For reference, the knobs used on the photo above are Black Small Unskirted Intellijel/Sifam Plastic Knobs which have a diameter of 11.5mm.

### A word about voltage levels. 
The voltage levels of this module are 0-5V, both in- and output. Negative CV Voltages will be clipped away. The Audio outputs are also DC-coupled
