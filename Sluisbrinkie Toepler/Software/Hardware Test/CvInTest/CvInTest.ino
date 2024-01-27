/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground through 220 ohm resistor

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/

//To convert the port names of the ATMEGA32U4 to Arduino Micro pin numbers
#define PC7 13
#define PD6 12
#define PB7 11
#define PB6 10
#define PB5 9
#define PB4 8
#define PE6 7
#define PD7 6
#define PC6 5
#define PD4 4
#define PD0 3
#define PD1 2
#define PD3 0
#define PD2 1
#define PB0 17
#define PF7 A0
#define PF6 A1
#define PF5 A2
#define PF4 A3
#define PF1 A4
#define PF0 A5

// Actual pin mappings
#define CV4_PIN PF7
#define CV3_PIN PF6
#define CV2_PIN PF5
#define CV1_PIN PF4

#define OUT1A PB5
#define OUT1B PB6
#define TROUT1 PF1

#define BUTTON1 PD6
#define BUTTON2 PD4

#define LED_BUILTIN PC7
#define LED1 PC7
#define LED2 PC6
#define LED3 PD7
#define LED_RX PB0
#define LED_TX PD5

#define TR1_PIN PE6
#define TR2_PIN PD1

#define MIDI_CTRL PB4
#define MIDI_RX PD3
#define MIDI_TX PD2

int sensorValue1 = 0;  // value read from the pot
int outputValue1 = 0;  // value output to the PWM (analog out)
int sensorValue2 = 0;  // value read from the pot
int outputValue2 = 0;  // value output to the PWM (analog out)
int sensorValue3 = 0;  // value read from the pot
int outputValue3 = 0;  // value output to the PWM (analog out)
int sensorValue4 = 0;  // value read from the pot
int outputValue4 = 0;  // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED_RX, OUTPUT);
}

void loop() {
  // read the analog in value:
  sensorValue1 = analogRead(CV1_PIN);
  sensorValue2 = analogRead(CV2_PIN);
  sensorValue3 = analogRead(CV3_PIN);
  sensorValue4 = analogRead(CV4_PIN);

  // map it to the range of the analog out:
  outputValue1 = map(sensorValue1, 0, 1023, 0, 255);
  outputValue2 = map(sensorValue2, 0, 1023, 0, 255);
  outputValue3 = map(sensorValue3, 0, 1023, 0, 255);
  outputValue4 = map(sensorValue4, 0, 1023, 0, 255);

  // change the analog out value:
  analogWrite(LED1, outputValue1);
  analogWrite(LED2, outputValue2);
  analogWrite(LED3, outputValue3);
  analogWrite(LED_RX, outputValue4);


  // print the results to the Serial Monitor:
  Serial.print("sensor1 = ");
  Serial.print(sensorValue1);
  Serial.print("\t output1 = ");
  Serial.println(outputValue1);
  Serial.print("sensor2 = ");
  Serial.print(sensorValue2);
  Serial.print("\t output2 = ");
  Serial.println(outputValue2);
  Serial.print("sensor3 = ");
  Serial.print(sensorValue3);
  Serial.print("\t output3 = ");
  Serial.println(outputValue3);
  Serial.print("sensor4 = ");
  Serial.print(sensorValue4);
  Serial.print("\t output4 = ");
  Serial.println(outputValue4);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(100);
}
