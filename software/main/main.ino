#include <TFT_HX8357.h>
#include <User_Setup.h>

#include "Game.h"

Game *game;
Shifter *shifter;
TurnSignal *turn;
void setup()
{
  Serial.begin(115200);
  game = new Game();

}

void loop()
{
  
  pinMode(8, INPUT);
  digitalWrite (2,1);
  game->runGame();
  
}
