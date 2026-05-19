#include "Obstacle.h"

#include <Scene/Component/Rigidbody.h>
#include <Utility/Utility.h>

////////////////////

void Obstacle::Setup(const ShapeType type)
{
	AddComponent<Transform>(glm::vec2(Random::RandomInRange<float>(minObstaclePosition, maxObstaclePosition),
		Random::RandomInRange<float>(minObstaclePosition, maxObstaclePosition)));

	if (type == ShapeType::Box)
	{
		AddComponent<BoxCollider>(glm::vec2(Random::RandomInRange<float>(minObstacleSize, maxObstacleSize),
			Random::RandomInRange<float>(minObstacleSize, maxObstacleSize)));
	}
	else if (type == ShapeType::Circle)
	{
		AddComponent<CircleCollider>(Random::RandomInRange<float>(minObstacleSize, maxObstacleSize));
	}

	bool positive = Random::RandomInRange<int>(0, 1);
	std::shared_ptr<Rigidbody> newRB = AddComponent<Rigidbody>(obstacleMass, obstacleLinearDamping, obstacleRestitution);
	newRB->SetLinearVelocity(glm::vec2(Random::RandomInRange<float>(minObstacleVelocity, maxObstacleVelocity),
		Random::RandomInRange<float>(minObstacleVelocity, maxObstacleVelocity)));
	if (!positive) newRB->GetLinearVelocity() *= -1;
}

void Obstacle::Update(float deltaTime)
{
	m_Colour = Colour::green;
}

void Obstacle::DrawDebug(const RendererDebug& rendererDebug)
{
	if (HasComponent<BoxCollider>())
	{
		const AABB boundingBox = GetComponent<BoxCollider>()->GetAABB();
		rendererDebug.DrawRectangle(boundingBox.min, boundingBox.max, m_Colour);
	}
	else if (HasComponent<CircleCollider>())
	{
		rendererDebug.DrawCircle(GetComponent<Transform>()->GetPosition(), GetComponent<CircleCollider>()->GetRadius(), m_Colour);
	}
}
