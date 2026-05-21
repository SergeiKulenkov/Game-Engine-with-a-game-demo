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
	glm::vec2 position = glm::vec2(0.f, 0.f);

	for (size_t i = 0; i < m_Boundaries.size(); i++)
	{
		newEntity = sharedScene->CreateEntity<Wall>();
		const std::shared_ptr<Wall> newWall = std::dynamic_pointer_cast<Wall>(newEntity);

		if (newWall != nullptr)
		{
			glm::vec2 size = glm::vec2(0.f, 0.f);
			if (i % 2 == 0) size.x = screenSize.x;
			else size.y = screenSize.y;

			position = boundaryPositions[i] * screenSize;
			if (boundaryPositions[i].x == 0) position.x += boundaryScreenOffset;
			else if (boundaryPositions[i].x == 1) position.x -= boundaryScreenOffset;
			if (boundaryPositions[i].y == 0) position.y += boundaryScreenOffset;
			else if (boundaryPositions[i].y == 1) position.y -= boundaryScreenOffset;

			newWall->Setup(position, size);
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