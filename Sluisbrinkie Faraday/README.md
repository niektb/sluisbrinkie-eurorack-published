# Sluisbrinkie Faraday

## Description
The Faraday (named after a certain Scientist) is an all-analog VCA/VCF combo using the SSI2164. It consists of three sections: VCA, SSM2040-style VCF and SVF VCF. The schematic can be found in this directory.

### VCA
The top section is a traditional VCA. The potmeter acts as offset control. The output of the VCA is normalled to the input of the SSM2040-style filter (the connection can be broken by inserting a jack into the VCF input).

### SSM2040-style VCF
The middle section is a 1-pole filter modeled after the revered SSM2040 VCF (which usually is a 4-pole filter but has a similar gain cell structure and the same type of asymmetric clipping). It's very colorful and can double as an overdrive when you overload the input. The uncolored input range is about 1Vpp. Above that, distortion will gradually be added. As it's a 1-pole filter, it's not capable of resonating (and therefore there is no resonance control).

### SVF (State Variable Filter) VCF
Finally the bottom section is a 2-pole State Variable Filter with high-pass, band-pass and low-pass filter outputs. The HPF and LPF outputs can be mixed externally to obtain a notch filter. The resonance on the bottom section has a wide Q factor so a bigger portion of the signal is amplified, and it doesn't reduce the bass (so be careful, signals can get loud). It is not capable of self-oscillating.

## Pictures
<img src ="https://github.com/user-attachments/assets/fa58cf46-f778-4b80-a235-95806e45d92b" width="40%"> <img src ="https://github.com/user-attachments/assets/15bee6b6-4270-4b7d-ac7f-dbeae1bd82ae" width="40%">
<img src ="https://github.com/user-attachments/assets/9026515a-5394-4326-84b6-ff71c241498d" width="40%">

## BOM
In case you bought a PCB (with assembled SMD components) and panel from me, there are a number of things you need to finish the build:

- 1x SSI2614 (SMD, this needs to be soldered manually)
- 11x Thonkiconn mono jacks
- 1x 100kOhm Type A potentiometer (Bourns PVT09 series are used in my personal builds)
- 3x 10kOhm Type B potentiometer (Bourns PVT09 series are used in my personal builds)
- 2x 100kOhm Type B potentiometer (Bourns PVT09 series are used in my personal builds)
- 6x potentiometer knobs
- 1x 2x5 pinheader (for eurorack power supply)
- 1x 10-pins ribbon cable (for eurorack power supply)


