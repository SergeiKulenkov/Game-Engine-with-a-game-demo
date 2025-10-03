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
	inline uint32_t RandomNumber(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	template<Integral T>
	inline T RandomInRange(const T min, const T max)
	{
		thread_local std::mt19937 generator(std::random_device{} ());
		std::uniform_int_distribution<T> distribution(min, max);
		return distribution(generator);
	}

	template<Floating T>
	inline T RandomInRange(const T min, const T max)
	{
		thread_local std::mt19937 generator(std::random_device{} ());
		std::uniform_real_distribution<T> distribution(min, max);
		return distribution(generator);
	}
}

////////////////////

struct PairCosSin
{
	float cos = 0.f;
	float sin = 0.f;
};

////////////////////

namespace Vector
{
	inline glm::vec2 Rotate(const glm::vec2& vector, const float cos, const float sin)
	{
		return glm::vec2(vector.x * cos - vector.y * sin, 
						vector.x * sin + vector.y * cos);
	}

	inline glm::vec2 Rotate(const glm::vec2& vector, const PairCosSin pairCosSin)
	{
		return glm::vec2(vector.x * pairCosSin.cos - vector.y * pairCosSin.sin,
						vector.x * pairCosSin.sin + vector.y * pairCosSin.cos);
	}

	inline glm::vec2 Rotate(const glm::vec2& vector, const float angle)
	{
		const float cos = glm::cos(angle);
		const float sin = glm::sin(angle);
		return glm::vec2(vector.x * cos - vector.y * sin,
						vector.x * sin + vector.y * cos);
	}

	inline PairCosSin GetCosAndSinFromVector(const glm::vec2& vector)
	{
		const float length = glm::length(vector);
		return PairCosSin(vector.x / length, vector.y / length);
	}

	inline bool IsFirstPointCloser(const glm::vec2& origin, const glm::vec2& first, const glm::vec2& second)
	{
		const glm::vec2 diff1 = glm::abs(origin - first);
		const glm::vec2 diff2 = glm::abs(origin - second);
		// avoiding square root for calculating distance
		return glm::dot(diff1, diff1) < glm::dot(diff2, diff2);
	}
}