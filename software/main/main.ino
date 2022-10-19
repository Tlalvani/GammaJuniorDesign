#include "DigitalSensor.h"
DigitalSensor d0(14);
void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(d0.getReading());
}
