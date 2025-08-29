#pragma once
#include <vector>
#include <array>
#include <memory>

#include <Scene/Entity.h>
#include <Scene/Scene.h>
#include <Scene/Component/Collider.h>
#include <Utility/Utility.h>

////////////////////

class EnvironmentManager : public Entity
{
public:
	EnvironmentManager()
	{
		m_Boundaries.resize(numberOfBoundaries);
		m_Obstacles.resize(numberOfInitialObstacles);
	}

	~EnvironmentManager() {}

	virtual void OnInit() override
	{
		std::shared_ptr<Entity> newEntity;
		const glm::vec2 screenSize = m_Scene->GetScreenSize();

		for (size_t i = 0; i < m_Boundaries.size(); i++)
		{
			newEntity = m_Scene->CreateEntity<Entity>();
			newEntity->AddComponent<Transform>(boundaryPositions[i] * screenSize);
			if (i % 2 == 0)
			{
				newEntity->AddComponent<BoxCollider>(glm::vec2(screenSize.x, boundaryThickness));
			}
			else
			{
				newEntity->AddComponent<BoxCollider>(glm::vec2(boundaryThickness, screenSize.y));
			}

			m_Boundaries[i] = newEntity;
		}
		for (size_t i = 0; i < m_Obstacles.size(); i++)
		{
			newEntity = m_Scene->CreateEntity<Entity>();
			newEntity->AddComponent<Transform>(glm::vec2(Random::RandomInRange<float>(minObstaclePosition, maxObstaclePosition),
														Random::RandomInRange<float>(minObstaclePosition, maxObstaclePosition)));
			if (i % 2 == 0)
			{
				newEntity->AddComponent<BoxCollider>(glm::vec2(Random::RandomInRange<float>(minObstacleSize, maxObstacleSize),
																Random::RandomInRange<float>(minObstacleSize, maxObstacleSize)), true);
			}
			else
			{
				newEntity->AddComponent<CircleCollider>(Random::RandomInRange<float>(minObstacleSize, maxObstacleSize), true);
			}

			m_Obstacles[i] = newEntity;
		}
	}

	virtual void DrawDebug(const RendererDebug& rendererDebug) override
	{
		for (std::weak_ptr<Entity>& weakBoundary : m_Boundaries)
		{
			std::shared_ptr<Entity> boundary = weakBoundary.lock();
			if (boundary)
			{
				const std::array<glm::vec2, 4> vertices = boundary->GetComponent<BoxCollider>().GetVertices();
				rendererDebug.DrawRectangle(vertices[0], vertices[2], Colour::green);
			}
		}
		for (size_t i = 0; i < m_Obstacles.size(); i++)
		{
			std::shared_ptr<Entity> obstacle = m_Obstacles[i].lock();
			if (obstacle)
			{
				if (i % 2 == 0)
				{
					const std::array<glm::vec2, 4> vertices = obstacle->GetComponent<BoxCollider>().GetVertices();
					rendererDebug.DrawRectangle(vertices[0], vertices[2], Colour::green);
				}
				else
				{
					rendererDebug.DrawCircle(obstacle->GetComponent<Transform>().GetPosition(), obstacle->GetComponent<CircleCollider>().GetRadius(), Colour::green);
				}
			}
		}
	}

private:
	static constexpr uint8_t numberOfBoundaries = 4;
	static constexpr uint8_t numberOfInitialObstacles = 2;
	static constexpr float boundaryThickness = 16.f;
	static constexpr float minObstaclePosition = 200.f;
	static constexpr float maxObstaclePosition = 800.f;
	static constexpr float minObstacleSize = 30.f;
	static constexpr float maxObstacleSize = 60.f;
	// values are multiplied by screen size
	static constexpr std::array<glm::vec2, numberOfBoundaries> boundaryPositions = { glm::vec2(0.5f, 0), glm::vec2(1, 0.5f), glm::vec2(0.5f, 1), glm::vec2(0, 0.5f) };

	std::vector<std::weak_ptr<Entity>> m_Boundaries;
	std::vector<std::weak_ptr<Entity>> m_Obstacles;
};