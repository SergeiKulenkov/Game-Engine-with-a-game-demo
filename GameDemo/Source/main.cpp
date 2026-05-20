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

	Game::Get().SetSceneDefaultSize(glm::vec2(windowWidth, windowHeight));
	engine.InitScene(Game::Get().GetScene());
	Game::Get().Start();
	engine.RunScene();

	Game::Shutdown();
	return 0;
}