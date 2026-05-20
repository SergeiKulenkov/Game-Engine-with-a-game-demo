#pragma once
#include <vector>
#include <array>
#include <memory>

#include "Scene/Entity.h"
#include "Obstacle.h"

////////////////////

class EnvironmentManager : public Entity
{
public:
	EnvironmentManager();
	~EnvironmentManager() {}

private:
	virtual void OnInit() override;

	////////////////////

	static constexpr uint8_t numberOfInitialBoundaries = 4;
	static constexpr uint8_t numberOfInitialObstacles = 200;
	// values are multiplied by screen size
	static constexpr std::array<glm::vec2, numberOfInitialBoundaries> boundaryPositions = { glm::vec2(0.5f, 0), glm::vec2(1, 0.5f), glm::vec2(0.5f, 1), glm::vec2(0, 0.5f) };

	static constexpr float boundaryThickness = 16.f;

	std::vector<std::weak_ptr<Entity>> m_Boundaries;
	std::vector<std::weak_ptr<Obstacle>> m_Obstacles;
};