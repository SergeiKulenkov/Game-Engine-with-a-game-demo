#pragma once
#include <random>

#include <glm/glm.hpp>

////////////////////

template<typename T>
concept Integral = std::is_integral_v<T> && !std::is_same_v<T, bool>;

template<typename T>
concept Floating = std::is_floating_point_v<T> && !std::is_same_v<T, bool>;

////////////////////

namespace Random
{
	uint32_t RandomNumber(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

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

namespace Vector
{
	glm::vec2 Rotate(const glm::vec2& vector, const float angle)
	{
		const float cos = glm::cos(angle);
		const float sin = glm::sin(angle);
		const float newX = vector.x * cos - vector.y * sin;
		const float newY = vector.x * sin + vector.y * cos;
		return glm::vec2(newX, newY);
	}
}