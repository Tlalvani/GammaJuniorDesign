/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

// Sead was here

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int led1 = 6;
int led2 = 7;
int led3 = 8;
int btn = 9;
int sensorValue = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(btn, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  int in = digitalRead(btn);
  if(in){
    digitalWrite(led1, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(led1, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);
    digitalWrite(led2, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(led2, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);  
    digitalWrite(led3, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(led3, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);   
  }
              // wait for a second
}
