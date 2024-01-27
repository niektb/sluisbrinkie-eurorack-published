# Sluisbrinkie Toepler Software
This folder contains example firmwares for the Sluisbrinkie Toepler.

## Hardware Test
This folder contains sketches to help check the hardware assembly for errors.
### CvInTest
This firmware maps CV1-4 to LED1-3 and LED RX (on the bottomside).
### TriggerTest
This firmware toggles the Trigger Out at a 1 second interval. It also blinks LED3 to reflect this. TriggerIn 1 and 2 are mapped to LED1 and LED2 respectively. Patch TriggerOut to both TriggerIns to verify that that section of the circuitry is working.

## Hagiwo Additive
This firmware is a port from Hagiwo's Additive Synth. See this youtube link for a demonstration: https://youtu.be/W8T1MqH5lBw?si=IQTcm9-iFZWvhHCK&t=71
## Ports of Grains
This folder contains a number of programs ported over from the Ginkosynthese Grains.
### grainsV21
This is somekind of Granular Formant Oscillator. Perfect for 'Ooooh' and 'Aaaaah' ;)
### jgb-RZ1-drums-V2-2
This is a lofi emulation of the Casio RZ1 drum machine. See this link for a short demonstration: https://youtube.com/shorts/vMh-8B2sCWI?si=By4kS0sKtcdMR4sM
### roundhouse
Roundhouse is a lofi 909 emulation.
