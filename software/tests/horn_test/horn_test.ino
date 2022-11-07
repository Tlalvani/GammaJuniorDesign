// Duelling Tones - Simultaneous tone generation.
// To mix the output of the signals to output to a small speaker (i.e. 8 Ohms or higher),
// simply use 1K Ohm resistors from each output pin and tie them together at the speaker.
// Don't forget to connect the other side of the speaker to ground!

// This example plays notes 'a' through 'g' sent over the Serial Monitor.
// 's' stops the current playing tone.  Use uppercase letters for the second.

#include <Tone.h>

int notes[] = { NOTE_A3,
                NOTE_B3,
                NOTE_C4,
                NOTE_D4,
                NOTE_E4,
                NOTE_F4,
                NOTE_G4 };

// You can declare the tones as an array
Tone notePlayer[2];

void setup(void)
{
  pinMode(15, INPUT); 
  Serial.begin(9600);
  notePlayer[0].begin(9);
//  notePlayer[1].begin(12);

}

void loop(void)
{
  char c;

  int input = 0;
  int val = digitalRead(15);
//  Serial.print("hello");
  Serial.print(val);
  if( val == 0){
    notePlayer[0].play(NOTE_A5);
  } else {
    notePlayer[0].stop();
  };
  

//  notePlayer[1].play(NOTE_D4);
//  delay(300);
//  notePlayer[1].stop();
//  notePlayer[0].play(NOTE_B2);
//  delay(300);
//  notePlayer[0].stop();
//  delay(100);
//  notePlayer[1].play(NOTE_B2);
//  delay(300);
//  notePlayer[1].stop();
}
