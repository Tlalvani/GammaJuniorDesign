#include <SD.h>
#include "TMRpcm.h"

TMRpcm tmrpcm;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  tmrpcm.speakerPin = 9;
  Serial.begin(9600);
  if (!SD.begin(53)) {
  Serial.println("SD fail");
  return;
  }
  Serial.println("SD play");
  tmrpcm.setVolume(1);
  
  
}

void loop() {

Serial.println(SD.exists("horn.wav"));
tmrpcm.play("horn.wav");
delay(10000);
}


//Blink D2
//D8 active low
