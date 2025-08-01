#include "Game/Game.h"

////////////////////

int main()
{
	Game::Init("Game Template");
	Game::Get().Run();
	Game::Shutdown();

	return 0;
}