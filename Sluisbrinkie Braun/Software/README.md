# Sluisbrinkie Braun - PlatformIO Version

This is the PlatformIO version of the Sluisbrinkie Braun project, converted from the original Arduino IDE project.

## Features

- Custom RP2040-based board definition
- ST7789 TFT display support
- 4-channel oscilloscope functionality
- Rotary encoder for time base control
- Easy compilation and upload with PlatformIO

## Building and Uploading

1. Install PlatformIO Core or PlatformIO IDE extension for VS Code
2. Open this folder in PlatformIO
3. Build: `pio run`
4. Upload: `pio run -t upload`

## Project Structure

- `src/main.cpp` - Main oscilloscope application
- `src/encoder.cpp` - Rotary encoder implementation
- `include/encoder.h` - Encoder header file
- `boards/sluisbrinkie_braun.json` - Custom board definition
- `variants/sluisbrinkie_braun/pins_arduino.h` - Pin definitions
- `platformio.ini` - PlatformIO configuration

## Hardware

- RP2040 microcontroller running at 133MHz
- ST7789 320x240 TFT display
- Rotary encoder for time base control
- 4 analog input channels (CV1-CV4)
- 2 output channels (OUT1-OUT2)

## Pin Mapping

- Display SPI1: SCK=10, MOSI=11, CS=9, DC=8, RST=15 (Uses SPI1 hardware interface)
- Encoder: A=24, B=25
- Analog inputs: CV1=26, CV2=27, CV3=28, CV4=29
- Outputs: OUT1=0, OUT2=1
- Switch: SW1=8

## Important: SPI1 Usage

This project uses **SPI1** (the second hardware SPI interface) on the RP2040, not the default SPI. This is critical because:

- The hardware is wired to SPI1 pins (SCK=10, MOSI=11, CS=9)
- The code correctly uses `SPI1.setSCK()`, `SPI1.setTX()`, `SPI1.setCS()` and `SPI1.begin()`
- The display library is initialized with `Adafruit_ST7789(&SPI1, ...)`
- Using the default SPI would not work with this hardware design

## Platform Configuration

The PlatformIO configuration uses:
- Maxgerhardt's RP2040 platform for proper Arduino-Pico core support
- Earlephilhower's arduino-pico framework (not mbed) for full SPI1 functionality
- Custom build flags for pin definitions

## Original Arduino IDE Files

The original Arduino IDE project files are preserved in the `Software/` directory:
- Custom board definitions (`boards_txt_addendum.txt`)
- makeboards.py additions (`makeboards_py_addendum.txt`)
- Original pin definitions

## Advantages of PlatformIO Version

1. **No manual board file editing** - Platform packages handle Arduino-Pico core automatically
2. **Proper SPI1 support** - Uses earlephilhower framework with full SPI1 functionality
3. **Automatic library management** - Dependencies are automatically downloaded
4. **Better IDE integration** - Full IntelliSense and debugging support
5. **Simplified compilation** - Single command compilation without setup
6. **Version control friendly** - All dependencies and settings are self-contained