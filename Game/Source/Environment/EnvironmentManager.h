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
	static constexpr uint8_t numberOfBoundaries = 4;
	static constexpr uint8_t numberOfInitialObstacles = 2;
	// values are multiplied by screen size
	static constexpr std::array<glm::vec2, numberOfBoundaries> boundaryPositions = { glm::vec2(0.5f, 0), glm::vec2(1, 0.5f), glm::vec2(0.5f, 1), glm::vec2(0, 0.5f) };

	static constexpr float boundaryThickness = 16.f;
	static constexpr float minObstaclePosition = 200.f;
	static constexpr float maxObstaclePosition = 800.f;
	static constexpr float minObstacleSize = 20.f;
	static constexpr float maxObstacleSize = 50.f;

	std::vector<std::weak_ptr<Entity>> m_Boundaries;
	std::vector<std::weak_ptr<Entity>> m_Obstacles;
};