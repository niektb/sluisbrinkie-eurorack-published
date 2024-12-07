# Sluisbrinkie Edison

The Edison is named after the well-known scientist. Now you might be looking at the module and wondering what the link is to the scientist but Edison is the man who patented the Phonograph, a device that could be used for the recording and playback of sound, just like this module! You can record audio into the internal recording buffer, via the built-in microphone or the jack input, and play it back. It's a 6HP-wide module and is built around the ISD1820. It can produce a wide spectrum of sounds ranging from tasty lofi to absolute glitch fests. 

<img src ="https://github.com/user-attachments/assets/314e5d49-fae8-406e-9831-80244e792315" width="40%">
<img src ="https://github.com/user-attachments/assets/16af7431-11fd-4067-9678-b37c11320b7a" width="40%">

In case you bought a kit, all SMD components are already soldered onto the board. To finish the build, a number of components are required: 
- 6x Thonkiconn mono jacks
- 2x 100kOhm potentiometer
- 1x 3mm LED (color of choice)
- 3x sub-miniature toggle switches
- 1x 2x5 pinheader (for eurorack power supply)
- 1x ISD1820 
- IC socket for ISD1820
- 1x electret microphone (make sure to buy one with long enough leads, in order to mount it level with the frontplate)

When asssembling, you will notice a couple of jumpers. These give the option to customize the behaviour of the ISD chip. 
<img src ="https://github.com/user-attachments/assets/aa963616-249d-49de-9f42-a43a4baeafcb" width="40%">
  1. The first one is mandatory to solder for a correctly functioning microphone. I bridged 1 and 2 to ensure that the microphone is muted when not recording (to avoid feedback in the Feedthrough mode)
  2. The second one can be used to enable the record button (I found that I sometimes accidentally touch the record button and thereby clearing the recorded buffer). Normally it would be recommended to solder this jumper.
  3. Number 3 gives control over the Automatic Gain. it can be left floating to enable AGC but it can also be forced high or low to get a fixed gain.
  4. Number 4 can be used to create a ramp on the AGC (causing a fade in).
