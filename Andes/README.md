# Introduction
The Andes is the brainproduct of the people on the Sourcery Studios Discord. We love the Beaks but wished it had CV inputs. 
So we got to work and made an upgraded version of the [Beaks](https://github.com/SourceryOne/Beaks) / [$peaks](https://github.com/matias-levy/peaks) . Currently there are 3 variants: 8HP (SMD), 10HP (THT) and 12HP (THT). Below is a picture of the 8HP version.

<img src ="https://github.com/user-attachments/assets/674d43fb-ff0f-4d68-81c7-24f099ce9fd2" width="40%">
<img src ="https://github.com/user-attachments/assets/483541f6-250e-47a6-bef5-1da767e67116" width="40%">

# Revision log
As of the 19th of October, the prototypes of both the 8HP and 10HP are assembled and being tested. **We did discover a few mistakes so don't use the current version just yet!**
The layout of the 12HP is still work-in-progress. 


As of the 6th of November, a new revision of the Andes 8HP is made and this seems to have resolved the issue!

# Build Tips and other remarks
- A note with respect to the iBoms of the Andes 8HP, the female headers for connecting the STM Bluepill are not listed in the iBOM (the csv bom correctly shows them). They are 01x20 2.54mm female pinheaders (or pinsockets).
- Since the potentiometers on the Front PCB don't have a bushing, it's recommended to add spacers and bolts / nuts to fixate the Front PCB and frontpanel together. The hole size is M2.5.
- Green leds / knobs can be used but are fairly dim. So it's recommended to use the red color ór adjust the current limiting resistors to increase brightness.
- The buttons I use in the picture are the 'Black Small Unskirted D - Sifam Plastic Knob' from Thonk (for size reference).
- The USB port from the STM should be pointed towards the power connector.
