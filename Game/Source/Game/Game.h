#pragma once
#include <Scene/Scene.h>

#include "../Player/Player.h"

////////////////////

class Game
{
public:
	~Game();

	static void Init();

	static void Shutdown();

	static Game& Get();

	std::shared_ptr<Scene> GetScene() { return m_Scene; }

private:
	Game();

	std::shared_ptr<Scene> m_Scene = std::make_shared<Scene>();
};