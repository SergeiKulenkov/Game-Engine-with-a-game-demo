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
		newEntity = sharedScene->CreateEntity<Wall>();
		const std::shared_ptr<Wall> newWall = std::dynamic_pointer_cast<Wall>(newEntity);

		if (newWall != nullptr)
		{
			glm::vec2 size = glm::vec2(0.f, 0.f);
			if (i % 2 == 0) size.x = screenSize.x;
			else size.y = screenSize.y;

			newWall->Setup(boundaryPositions[i] * screenSize, size);
			m_Boundaries[i] = newWall;
		}
	}

	for (size_t i = 0; i < m_Obstacles.size(); i++)
	{
		newEntity = sharedScene->CreateEntity<Obstacle>();
		const std::shared_ptr<Obstacle> newObstacle = std::dynamic_pointer_cast<Obstacle>(newEntity);

		if (newObstacle != nullptr)
		{
			if (i % 2 == 0) newObstacle->Setup(ShapeType::Box, screenSize);
			else newObstacle->Setup(ShapeType::Circle, screenSize);
			m_Obstacles[i] = newObstacle;
		}
	}
}