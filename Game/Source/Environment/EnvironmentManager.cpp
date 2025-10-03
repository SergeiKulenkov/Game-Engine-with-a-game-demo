#include "EnvironmentManager.h"

#include <Scene/Component/Collider.h>
#include <Scene/Component/Rigidbody.h>
#include <Utility/Utility.h>

////////////////////

EnvironmentManager::EnvironmentManager()
{
	m_Boundaries.resize(numberOfInitialBoundaries);
	m_Obstacles.resize(numberOfInitialObstacles);
}

void EnvironmentManager::OnInit()
{
	std::shared_ptr<Entity> newEntity;
	const std::shared_ptr<Scene> sharedScene = m_Scene.lock();
	assert(sharedScene && "This Entity's reference to the Scene is null");
	const glm::vec2 screenSize = sharedScene->GetScreenSize();

	for (size_t i = 0; i < m_Boundaries.size(); i++)
	{
		newEntity = sharedScene->CreateEntity<Entity>();
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
		bool positive = true;
		newEntity = sharedScene->CreateEntity<Entity>();
		newEntity->AddComponent<Transform>(glm::vec2(Random::RandomInRange<float>(minObstaclePosition, maxObstaclePosition),
			Random::RandomInRange<float>(minObstaclePosition, maxObstaclePosition)));

		if (i % 2 == 0)
		{
			positive = false;
			newEntity->AddComponent<BoxCollider>(glm::vec2(Random::RandomInRange<float>(minObstacleSize, maxObstacleSize),
												Random::RandomInRange<float>(minObstacleSize, maxObstacleSize)), true);
		}
		else
		{
			newEntity->AddComponent<CircleCollider>(Random::RandomInRange<float>(minObstacleSize, maxObstacleSize), true);
		}

		std::shared_ptr<Rigidbody> newRB = newEntity->AddComponent<Rigidbody>(obstacleMass, obstacleLinearDamping, obstacleRestitution);
		newRB->SetLinearVelocity(glm::vec2(Random::RandomInRange<float>(minObstacleVelocity, maxObstacleVelocity),
											Random::RandomInRange<float>(minObstacleVelocity, maxObstacleVelocity)));
		if (!positive) newRB->GetLinearVelocity() *= -1;

		m_Obstacles[i] = newEntity;
	}
}

void EnvironmentManager::DrawDebug(const RendererDebug& rendererDebug)
{
	for (std::weak_ptr<Entity>& weakBoundary : m_Boundaries)
	{
		std::shared_ptr<Entity> boundary = weakBoundary.lock();
		if (boundary)
		{
			const std::array<glm::vec2, 4> vertices = boundary->GetComponent<BoxCollider>()->GetVertices();
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
				const std::array<glm::vec2, 4> vertices = obstacle->GetComponent<BoxCollider>()->GetVertices();
				rendererDebug.DrawRectangle(vertices[0], vertices[2], Colour::green);
			}
			else
			{
				rendererDebug.DrawCircle(obstacle->GetComponent<Transform>()->GetPosition(), obstacle->GetComponent<CircleCollider>()->GetRadius(), Colour::green);
			}
		}
	}
}