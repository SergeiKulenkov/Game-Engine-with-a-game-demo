#pragma once
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/Component/Collider.h"
#include "Utility/Utility.h"

////////////////////

class Obstacle : public Entity
{
public:
	Obstacle() {}	
	~Obstacle() {}

	void ChangeColour(const uint32_t colour) { m_Colour = colour; }

	void Setup(const ShapeType type)
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

protected:
	virtual void Update(float deltaTime) override
	{
		m_Colour = Colour::green;
	}

	virtual void DrawDebug(const RendererDebug& rendererDebug) override
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

private:
	static constexpr float minObstaclePosition = 100.f;
	static constexpr float maxObstaclePosition = 950.f;
	static constexpr float minObstacleSize = 18.f;
	static constexpr float maxObstacleSize = 30.f;

	static constexpr float minObstacleVelocity = 10.f;
	static constexpr float maxObstacleVelocity = 12.f;
	static constexpr float obstacleMass = 0.25f;
	static constexpr float obstacleLinearDamping = 0.1f;
	static constexpr float obstacleRestitution = 0.9f;

	uint32_t m_Colour = Colour::green;
};