#include "Game.h"
#include "../Environment/EnvironmentManager.h"
#include "../Player/Player.h"

////////////////////

static Game* s_Instance = nullptr;

Game::Game()
{}

Game::~Game()
{}

void Game::Init()
{
	assert(!s_Instance);
	s_Instance = new Game();
}

void Game::Shutdown()
{
	delete s_Instance;
	s_Instance = nullptr;
}

Game& Game::Get()
{
	assert(s_Instance);
	return *s_Instance;
}

std::shared_ptr<Scene> Game::GetScene()
{
	return std::dynamic_pointer_cast<Scene>(m_Scene);
}

void Game::Start()
{
#ifdef DEBUG
	m_Scene->Start(true);
#else
	m_Scene->Start(false);
#endif
	m_Scene->CreateEntity<Player>();
	m_Scene->CreateEntity<EnvironmentManager>();
}
