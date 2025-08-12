#include "Game.h"

////////////////////

static Game* s_Instance = nullptr;

Game::Game()
{
	m_Scene->CreateEntity<Player>();
}

Game::~Game()
{
}

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
