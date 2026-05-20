#include "EnvironmentManager.h"

////////////////////

EnvironmentManager::EnvironmentManager()
{
	m_Boundaries.resize(numberOfInitialBoundaries);
	m_Obstacles.resize(numberOfInitialObstacles);
}

void EnvironmentManager::OnInit()
{
	const std::shared_ptr<Scene> sharedScene = m_Scene.lock();
	ASSERT_SCENE_SHARED_PTR(sharedScene);
	const glm::vec2 screenSize = sharedScene->GetScreenSize();

	std::shared_ptr<Entity> newEntity;
	for (size_t i = 0; i < m_Boundaries.size(); i++)
	{
		newEntity = sharedScene->CreateEntity<Entity>();
		//newEntity->AddComponent<Transform>(boundaryPositions[i] * screenSize);
		//if (i % 2 == 0)
		//{
		//	newEntity->AddComponent<BoxCollider>(glm::vec2(screenSize.x, boundaryThickness));
		//}
		//else
		//{
		//	newEntity->AddComponent<BoxCollider>(glm::vec2(boundaryThickness, screenSize.y));
		//}

		m_Boundaries[i] = newEntity;
	}

	for (size_t i = 0; i < m_Obstacles.size(); i++)
	{
		newEntity = sharedScene->CreateEntity<Obstacle>();
		const std::shared_ptr<Obstacle> newObstacle = std::dynamic_pointer_cast<Obstacle>(newEntity);

		if (newObstacle != nullptr)
		{
			if (i % 2 == 0) newObstacle->Setup(ShapeType::Box);
			else newObstacle->Setup(ShapeType::Circle);
			m_Obstacles[i] = newObstacle;
		}
	}
}