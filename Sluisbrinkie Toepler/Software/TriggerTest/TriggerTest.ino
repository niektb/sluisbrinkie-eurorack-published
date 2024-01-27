#include "PinDef.h"

int TROUT = false;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(TROUT1, OUTPUT);


  pinMode(TR1_PIN, INPUT_PULLUP);
  pinMode(TR2_PIN, INPUT_PULLUP);
}

void loop() {
  int sensorValue1 = digitalRead(TR1_PIN);
  int sensorValue2 = digitalRead(TR2_PIN);

  digitalWrite(LED1, sensorValue1);
  digitalWrite(LED2, sensorValue2);

  TROUT = !TROUT;
  digitalWrite(TROUT1, TROUT);
  digitalWrite(LED3, TROUT);

  // print the results to the Serial Monitor:
  Serial.print("Trigger In 1 = ");
  Serial.print(sensorValue1);
  Serial.print("\t Trigger In 2 = ");
  Serial.print(sensorValue2);
  Serial.print("\t Trigger Out = ");
  Serial.println(TROUT);

  delay(1000);
}
