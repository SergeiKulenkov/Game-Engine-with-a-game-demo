#pragma once
#include <Scene/Scene.h>

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

	std::shared_ptr<Scene> GetScene() { return m_Scene; }

private:
	Game();

	std::shared_ptr<Scene> m_Scene = std::make_shared<Scene>();
};