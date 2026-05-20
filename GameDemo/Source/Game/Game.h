#pragma once
#include "CurrentScene.h"

////////////////////

class Game
{
public:
	~Game();

	static void Init();

	static void Shutdown();

	static Game& Get();

	void Start();

	void SetSceneDefaultSize(const glm::vec2& size) { m_Scene->SetDefaultScreenSize(size); }

	std::shared_ptr<Scene> GetScene();

private:
	Game();

	std::shared_ptr<CurrentScene> m_Scene = std::make_shared<CurrentScene>();
};