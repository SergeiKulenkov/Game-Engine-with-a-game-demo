#pragma once
#include <random>

#include <Engine/Engine.h>

#include "../Player/Player.h"

////////////////////

template<typename T>
concept Integral = std::is_integral_v<T> && !std::is_same_v<T, bool>;

template<typename T>
concept Floating = std::is_floating_point_v<T> && !std::is_same_v<T, bool>;

namespace Random
{
	uint32_t RandomNumber(uint32_t input);

	template<Integral T>
	T RandomInRange(const T min, const T max)
	{
		thread_local std::mt19937 generator(std::random_device{} ());
		std::uniform_int_distribution<T> distribution(min, max);
		return distribution(generator);
	}

	template<Floating T>
	T RandomInRange(const T min, const T max)
	{
		thread_local std::mt19937 generator(std::random_device{} ());
		std::uniform_real_distribution<T> distribution(min, max);
		return distribution(generator);
	}
}

////////////////////

class Game
{
public:
	~Game();

	static void Init(std::string_view name = "");

	static void Shutdown();

	static Game& Get();

	void Run();

private:
	Game(std::string_view name);

	std::unique_ptr<Engine> m_Engine;

	std::shared_ptr<Player> m_Player = std::make_shared<Player>();
};