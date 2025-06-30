// BrickBreaker.cpp : Defines the entry point for the application.
//

#include "BrickBreaker.h"
#include "Game/Game.hpp"


int main()
{
	Game* game = new Game();
	game->Start();
}
