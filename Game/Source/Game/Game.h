#pragma once
#include <random>

#include <Scene/Scene.h>

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

	static void Init();

	static void Shutdown();

	static Game& Get();

	std::shared_ptr<Scene> GetScene() { return m_Scene; }

private:
	Game();

	std::shared_ptr<Scene> m_Scene = std::make_shared<Scene>();
};