#include "Game.h"

////////////////////

namespace Random
{
	uint32_t RandomNumber(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}
}

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
