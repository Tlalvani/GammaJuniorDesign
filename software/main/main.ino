#include <TFT_HX8357.h>
#include <User_Setup.h>

#include "Game.h"

Game *game;
Shifter *shifter;
TurnSignal *turn;
void setup()
{
  Serial.begin(115200);
  //game = new Game();
  shifter = new Shifter();
  //turn = new TurnSignal();
  
}

void loop()
{
  
  pinMode(8, INPUT);
  digitalWrite (2,1);
  int val1 = -1;
  int val2 = shifter->detectChange();
  if(val1 != -1){
    Serial.print("turnSignal: ");
    Serial.println(val1);
  }

  if(val2 != -1){
    Serial.print("shifter: ");
    Serial.println(val2);
  }
   //  Serial.println(digitalRead(8))
  //game->getUserAction();
  //Serial.println(String(digitalRead(18)) + String(digitalRead(17)) + String(digitalRead(16)));
}
