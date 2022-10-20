#include <TFT_HX8357.h>
#include <User_Setup.h>

#include "Game.h"

Game *game;
void setup()
{
  Serial.begin(115200);
  game = new Game;
}

void loop()
{
  game->runGame();
}
