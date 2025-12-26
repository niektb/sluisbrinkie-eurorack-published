# Description
I needed a PSU that could source more current than the one I already had, and I liked the idea of implementing USB-C PD (because I almost always have a USB-C adapter lying around somewhere). I found the PD_Micro project and liked the idea of using that as a starting point (as I often use 32U4s). I added Isolated DC-DC converters, an eFuse, a WS2812B led for status indication and made some more changes to make it work for my purpose.

Library pre-requisite: https://github.com/kcl93/fusb302_arduino 
This library needs a minor modification to select the correct I2C Address for the FUSB302. Line 76 from PD_UFP.cpp needs to be changed to 0x24;

The boards have been tested and are fully functional but the software still has limited features (for example, current monitoring is indicated by a LED, meaning it's only a very rough indication).

![20241030_210543 (1)](https://github.com/user-attachments/assets/8d877898-b6c1-404c-9cc7-23559f694691)
