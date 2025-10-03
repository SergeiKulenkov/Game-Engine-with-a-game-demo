#pragma once
#include <vector>
#include <array>
#include <memory>

#include <Scene/Entity.h>
#include <Scene/Scene.h>

////////////////////

class EnvironmentManager : public Entity
{
public:
	EnvironmentManager();

	~EnvironmentManager() {}

	virtual void OnInit() override;

	virtual void DrawDebug(const RendererDebug& rendererDebug) override;

private:
	static constexpr uint8_t numberOfInitialBoundaries = 4;
	static constexpr uint8_t numberOfInitialObstacles = 100;
	// values are multiplied by screen size
	static constexpr std::array<glm::vec2, numberOfInitialBoundaries> boundaryPositions = { glm::vec2(0.5f, 0), glm::vec2(1, 0.5f), glm::vec2(0.5f, 1), glm::vec2(0, 0.5f) };

	static constexpr float boundaryThickness = 16.f;
	static constexpr float minObstaclePosition = 125.f;
	static constexpr float maxObstaclePosition = 925.f;
	static constexpr float minObstacleSize = 25.f;
	static constexpr float maxObstacleSize = 47.f;

	static constexpr float obstacleMass = 0.25f;
	static constexpr float obstacleLinearDamping = 0.1f;
	static constexpr float obstacleRestitution = 0.9f;
	static constexpr float minObstacleVelocity = 85.f;
	static constexpr float maxObstacleVelocity = 115.f;

	std::vector<std::weak_ptr<Entity>> m_Boundaries;
	std::vector<std::weak_ptr<Entity>> m_Obstacles;
};