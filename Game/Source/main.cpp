#include <Engine/Engine.h>
#include <Input/InputManager.h>
#include "Game/Game.h"

////////////////////

static constexpr std::string_view windowTitle = "Game Template";
static constexpr uint16_t windowWidth = 1920;
static constexpr uint16_t windowHeight = 1080;

////////////////////

int main()
{
	Engine engine(windowTitle, windowWidth, windowHeight);
	Game::Init();
	InputManager inputManager;
	inputManager.Init(engine.GetWindow());

	engine.InitScene(Game::Get().GetScene());
	engine.RunScene();

	Game::Shutdown();
	return 0;
}