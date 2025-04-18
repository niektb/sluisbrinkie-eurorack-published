# Sluisbrinkie Braun
The Braun is a simple, straightforward 4-channel oscilloscope, meant to fill the gap between low-cost 1-channel scopes (such as the Hagiwo) and expensive scopes like the Mordax Data. The analog front-end is a simple scale and offset with fixed attenuation (It can be changed in hardware though) and no hardware triggering.
It features a RP2040 processor (meaning some mathematical operations like FFT are possible but also visualizers), a 320x240p IPS display and USB-C. The Reset and Boot button are also mounted to front, meaning you never have to remove the module from your rack when writing firmware :)
The frontjacks are connected directly to each other (without buffering) so it doesn't really matter which one you use as input. It is compatible with both Arduino and CircuitPython, offering a friendly programming environment. The flash size is 128Mbit, meaning there is ample space for storing data.

<img src="https://github.com/user-attachments/assets/434dca47-15a8-4792-b1c6-9f4da315542c" width="33%">

<img src="https://github.com/user-attachments/assets/651cb765-01ba-476c-b398-cb38701e72fd" width="33%">

<img src="https://github.com/user-attachments/assets/bcc37b46-77e7-41fd-9a2b-22aeed220083" width="33%">

# Project Sponsor
This project is sponsored by PCBWay. Whether for small projects or complex designs, PCBWay offers quality PCB manufacturing, PCBA services, and CNC machining at competitive prices. Learn more at [pcbway.com](https://www.pcbway.com/).

These are the through-hole parts that you need to add to finish the build:
- 8x Thonkiconn mono jacks
- 2x pushbuttons with at least 13mm shaft
- 1x encoder knob
- 1x encoder (see note below)
- 1x USB-C UJ20-C-V-C-2-SMT-TR
- 1x 2x5 pinheader (for eurorack power supply)
- M2.5 spacers and bolts to fixate display
- GMT020-02-7P display

## Known issues
The current design has a few points that need to be improved, being:
- I want to make the encoder hole on the frontpanel and the mounting holes on the PCB a bit bigger so it can also accomodate the PEC11R series.
- Some of the holes for the pinheader (connecting the display to the PCB) are too small. For the proto I solved it by mounting the display to the PCB (instead of the panel, solving the next issue as well) and cutting the pinheaders short such that I could solder it from the topside.
- When mounting the display to the panel, it's important to not overtighten the bolts as mechanical stress to the front of display causes a brightness differences on the backlight. Maybe rubber rings could help alleviate this. 
- Currently, the display is soldered directly to the board. Maybe low-profile pinsockets would be an idea.
- Apparently the 3D model I used for the display was not entirely accurate, so a few millimeters at the bottom of the display are covered by the frontpanel.

## Things that are useful to know
- To unleash the module's potential, it is recommended that you have some programming experience and enjoy programming :)
- The USB-C needs to be soldered with care (also, make sure it's vertical!). I would not recommend this project for the absolute beginners!
- The USB-C port is not capable of powering external devices.

## Software (Arduino)
The board can be used with Arduino but needs a bit of work setting up as it's a custom board. Boards_txt_addendum.txt (I'm still figuring out whether I can make it compatible out-of-the-box) contains the information that you need to add to your own boards.txt file. pins_arduino.h is what you need to complete the definition of your custom board. Refer to pins_arduino and the schematic to learn the pin mapping (and which pins are pre-mapped).
Currently, these sketches can be found in the repo.

### Simple Scope
A simple sketch to test the functionality of the board with a naive approach. For every sample it captures it also updates the display, making it bottlenecked by the display. Great for visualizing CV, not fast enough for audio or precise measurements. No UI or triggering, just waves :)

### Display Speed Test
At first, the simple scope was horrendously slow so I wrote a small sketch to make some benchmarks. Discovered I was bitbanging SPI :D


## Software (CircuitPython/MicroPython)
Technically the board can work with Python but at this point I haven't invested the time to create a custom firmware.
