#include "PinDef.h"     // Include definitions for the pin assignments
#include "mcp4822.h"    // Include the library for the MCP4822 DAC (Digital-to-Analog Converter)
#include "Arduino.h"    // Include the Arduino core library
#include <SPI.h>        // Include the SPI library for SPI communication
#include "digitalin.h"  // Include the library for handling digital inputs
#include "adenvelope.h" // Include the library for AD envelope generation
#include "analogin.h"   // Include the library for handling analog inputs

// Create an instance of the Mcp4822 class for the DAC, using chip select pin CS1
Mcp4822 dacAB = Mcp4822(CS1);

// Create instances of the DigitalIn class for gate and switch inputs
DigitalIn GATEA = DigitalIn(GATEA_PIN);
DigitalIn GATEB = DigitalIn(GATEB_PIN);
DigitalIn SWITCHA = DigitalIn(SWITCH1);
DigitalIn SWITCHB = DigitalIn(SWITCH2);

// Variables to keep track of whether we are waiting for the end of a pulse for each gate
int waitForEOCA = false;  // false = not waiting or pulse finished, true = waiting for pulse to end.
int waitForEORB = false;  // false = not waiting or pulse finished, true = waiting for pulse to end.

// Variables to store the start time of the pulse for each gate
uint32_t StartTimeEOCA = 0;
uint32_t StartTimeEORB = 0;

// Constant defining the pulse length in milliseconds
const uint8_t pulseLength = 10;  // ms

// Create instances of the AnalogIn class for the control voltage (CV) inputs
AnalogIn cv1(CV2_PIN, 1, 5000, true);    // speed
AnalogIn cv2(CV1_PIN, 0.01, 0.99, false);  // shape
AnalogIn cv3(CV4_PIN, 1, 5000, true);    // speed
AnalogIn cv4(CV3_PIN, 0.01, 0.99, false);  // shape

// Create instances of the ADEnvelope class for envelope generation
ADEnvelope envA(1000, 1000);  // Attack and decay times for envelope A
ADEnvelope envB(1000, 1000);  // Attack and decay times for envelope B

void setup() {
  // Set pin modes for digital pins
  pinMode(CS1, OUTPUT);
  pinMode(CV1_PIN, INPUT);
  pinMode(CV2_PIN, INPUT);
  pinMode(CV3_PIN, INPUT);
  pinMode(CV4_PIN, INPUT);
  pinMode(CLKOUT_PIN, OUTPUT);
  pinMode(EORB_PIN, OUTPUT);
  pinMode(EOCA_PIN, OUTPUT);

  // Ensure the DAC is not selected by setting the chip select pin high
  digitalWrite(CS1, HIGH);

  // Initialize SPI communication and the serial monitor
  SPI.begin();
  Serial.begin(9600);

  // Enable or disable looping for the envelopes based on the switch states
  envA.enableLoop(digitalRead(SWITCH1) == 0);
  envB.enableLoop(digitalRead(SWITCH2) == 0);

  // Start the attack phase for both envelopes
  envA.startAttack();
  envB.startAttack();
}

void loop() {
  // Handle switch A state changes
  switch (SWITCHA.TaskFunction()) {
    case 0:
      envA.enableLoop(LOW);  // Disable looping for envelope A
      break;
    case 1:
      envA.enableLoop(HIGH); // Enable looping for envelope A
      break;
    default:
      break;
  }

  // Handle switch B state changes
  switch (SWITCHB.TaskFunction()) {
    case 0:
      envB.enableLoop(LOW);  // Disable looping for envelope B
      break;
    case 1:
      envB.enableLoop(HIGH); // Enable looping for envelope B
      break;
    default:
      break;
  }

  // Handle gate A state changes
  switch (GATEA.TaskFunction()) {
    case 0:
      envA.startDecay();  // Start decay phase for envelope A
      break;
    case 1:
      envA.startAttack(); // Start attack phase for envelope A
      break;
    default:
      break;
  }

  // Handle gate B state changes
  switch (GATEB.TaskFunction()) {
    case 0:
      envB.startDecay();  // Start decay phase for envelope B
      break;
    case 1:
      envB.startAttack(); // Start attack phase for envelope B
      break;
    default:
      break;
  }

  // Check if envelope A has reached the end of the cycle
  if (envA.getEOCStatus()) {
    digitalWrite(EOCA_PIN, HIGH);  // Set EOCA pin high
    StartTimeEOCA = millis();      // Record the start time
    waitForEOCA = true;            // Set the flag to wait for the end of the pulse
    envA.resetEOCStatus();         // Reset the end-of-cycle status
  }

  // Check if envelope B has reached the end of the release phase
  if (envB.getEORStatus()) {
    digitalWrite(EORB_PIN, HIGH);  // Set EORB pin high
    StartTimeEORB = millis();      // Record the start time
    waitForEORB = true;            // Set the flag to wait for the end of the pulse
    envB.resetEORStatus();         // Reset the end-of-release status
  }

  // Turn off EOCA pin after the pulse length has elapsed
  if (waitForEOCA && ((millis() - StartTimeEOCA) >= pulseLength)) {
    waitForEOCA = false;
    digitalWrite(EOCA_PIN, LOW);
  }

  // Turn off EORB pin after the pulse length has elapsed
  if (waitForEORB && ((millis() - StartTimeEORB) >= pulseLength)) {
    waitForEORB = false;
    digitalWrite(EORB_PIN, LOW);
  }

  // Update envelope A parameters if the CV values have changed
  if (cv1.hasValueChanged() || cv2.hasValueChanged()) {
    envA.setAttack(cv1.getCurrentValue() * cv2.getCurrentValue());
    envA.setDecay(cv1.getCurrentValue() * (1 - cv2.getCurrentValue()));
  }

  // Update envelope B parameters if the CV values have changed
  if (cv3.hasValueChanged() || cv4.hasValueChanged()) {
    envB.setAttack(cv3.getCurrentValue() * cv4.getCurrentValue());
    envB.setDecay(cv3.getCurrentValue() * (1 - cv4.getCurrentValue()));
  }

  // Map envelope A output to DAC range and write to DAC channel 1
  int outputA = map(envA.taskFunction(), 0, 255, 2048, 4095);  // unipolar only
  dacAB.writeDac1(outputA);

  // Map envelope B output to DAC range and write to DAC channel 2
  int outputB = map(envB.taskFunction(), 0, 255, 2048, 4095);  // unipolar only
  dacAB.writeDac2(outputB);
}