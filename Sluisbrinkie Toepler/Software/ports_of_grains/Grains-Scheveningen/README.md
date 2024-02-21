# "Scheveningen" 

###
2024 - 02 - 21: Port for Sluisbrinkie Toepler. Port is released under the same license as the original software.
CV 1: Base Pitch
CV 2: Amount of Wavefolding
CV 3: Pitch of second oscillator
CV 4: CV Input (for Base Pitch)
###

An alternative Firmware for the AE Modular GRAINS module by tangible waves and the GRAINS Eurorack module by Ginkosynthese.

https://www.tangiblewaves.com/store/p86/GRAINS.html

and

https://www.ginkosynthese.com/

This is a modern take on West Coast Synthesis made in The Hague by Kassen Oud (https://www.facebook.com/kassen.oud). 

We use two sine waves, the first we'll call the "master", the second the
"slave". The master just tracks the pitch CV. The slave is some set
interval above the master. In addition the slave is hard-synced to the
master. These two are ring-modulating each other. This is followed by a
wave-folder; a classical "West Coast" style effect which is a bit like a
clip, except once the wave hits the maximum excursion it "folds" back
towards the centre. Of these two effects, the ring-modulator and the
wave-wrapper, the first is more mellow and smooth, while the second tends
to be more up-front. By combining those, and modulating them, we can get a
rich pallet of timbres from a relatively simple module like the Grains.

  
To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
 
## Note

This firmware works, but there are still a few things that can be improved. Please play around with it and post your comments on the forum at
https://forum.aemodular.com

# Usage of this Firmware with GRAINS:

__Inputs__

* IN1 / Pot1: Offset of the slave oscillator's pitch, relative to the master's, if controlled via CV the Pot will control the depth of CV control 
* IN2 / Pot2: Controls an internal Wavefolder, if controlled via CV the Pot will control the depth of CV control 
* IN3 / Pot3: Controls the pitch of the oscillator, if controlled via CV the pot will add to the CV pitch

__Outputs__

* OUT:        Audio out of the oscillator

__Caution!__ Use at your own risk  (according to GNU General Public License v2 '12. No Warrenty')

# Tutorial Video 
The video by The 5th Volt explains in detail how to use this firmware. 
[![GRAINS Scheveningen](https://res.cloudinary.com/marcomontalbano/image/upload/v1611316374/video_to_markdown/images/youtube--5bPDJzVtN6k-c05b58ac6eb4c4700831b2b3070cd403.jpg)](https://youtu.be/5bPDJzVtN6k "GRAINS Scheveningen")

---
This program in combination with the hardware it is applied to can produce harsh and loud 
frequencies that may be of harm to speakers or your ears! Permanent hearing loss may result 
from exposure to sound at high volumes. Use as low a volume as possible.

'GRAINS Scheveningen' is an experimental Firmware for the AE Modular GRAINS module by tangible waves

Copyright (C) 2021  Kassen Oud (https://www.facebook.com/kassen.oud)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
