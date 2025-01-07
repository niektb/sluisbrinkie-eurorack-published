# Sluisbrinkie Toepler+
The Toepler+ is the streamlined successor to the Toepler. It is one-third of the size (some features have been removed to make this possible) and has a much faster processor (RP2040 instead of Atmega32u4). The size is 4 HP. 
The upgraded processor allows for complex synthesis algorithms. MIDI support also allows the module to be truly polyphonic. The uni-color LEDs have been replaced by 2 Addressable RGB LEDs. It is compatible with both Arduino and CircuitPython, offering a friendly programming environment.
All the interfaces are located at the front of the module, meaning that nothing has to be unscrewed and removed from the rack when you want to do some programming. In fact, it can also be programmed when the rack is still powered (although pops and clicks are to be expected if the outputs are connected).
The USB connector can also be used for USB MIDI (but it is not capable of powering external devices). The flash size is 128Mbit, meaning there is ample space for storing wavetables or short samples.

<img src ="https://github.com/user-attachments/assets/8fe94131-ea18-436b-a2a9-0236d4d08ec7" width="40%">
<img src ="https://github.com/user-attachments/assets/c14faffa-4de7-4ef0-a656-0506330a3b0a" width="40%">

<img src ="https://github.com/user-attachments/assets/3427c4f6-3299-434c-b761-ab89d997a5f0" width="40%">
<img src ="https://github.com/user-attachments/assets/820acfc7-2292-4b56-858f-38727a4a49fe" width="40%">

In case you bought a PCB (with assembled SMD components) and panel from me, there are a number of things you need to finish the build:
- 5x Thonkiconn mono jacks
- 1x Thonkiconn stereo jack
- 3x pushbuttons with at least 13mm shaft
- 2x 10kOhm Type B potentiometer (Bourns PVT09 series are used in my builds)
- 2x potentiometer knobs (see note below)
- 2x WS2812B RGB LEDs
- 1x USB-C UJ20-C-V-C-2-SMT-TR
- 1x 2x5 pinheader (for eurorack power supply)
- 2x 13p pinheader single row (for connecting back PCB to front PCB)
- M2.5 spacer and bolt to fixate frontpanel to front PCB (recommend to avoid stressing the PCB when plugging cables into the USB connector)

## Writing software
### Flashing the firmware
To be written.

### Writing software
To be written.

### A word about voltage levels
The voltage levels of the microcontroller are 0-3.3V. The Audio Output is DC-Coupled and ranges between 0V and 10V. The CV inputs are -5V to 5V and are mapped to the microcontroller range using attenuation and offset. 

## Things that are useful to know
To be written.
