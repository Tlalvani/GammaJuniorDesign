#include "Game.h"

Game *game;
void setup()
{
  Serial.begin(9600);
  game = new Game;
}

void loop()
{
  game->runGame();
}
