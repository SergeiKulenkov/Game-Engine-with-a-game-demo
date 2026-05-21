#include "Obstacle.h"

#include <Scene/Component/Sprite.h>
#include <Scene/Component/Rigidbody.h>
#include <Utility/Utility.h>

////////////////////

void Obstacle::Setup(const ShapeType type, const glm::vec2& screenSize)
{
	AddComponent<Transform>(glm::vec2(Random::RandomInRange<float>(screenOffset, screenSize.x - screenOffset),
									Random::RandomInRange<float>(screenOffset, screenSize.y - screenOffset)));
	AddComponent<Sprite>(spritePath);

	if (type == ShapeType::Box)
	{
		AddComponent<BoxCollider>(glm::vec2(Random::RandomInRange<float>(minSize, maxSize),
											Random::RandomInRange<float>(minSize, maxSize)));
	}
	else if (type == ShapeType::Circle)
	{
		AddComponent<CircleCollider>(Random::RandomInRange<float>(minSize / 2.f, maxSize / 2.f));
	}

	std::shared_ptr<Rigidbody> rigidbody = AddComponent<Rigidbody>(mass, linearDamping, restitution);
	rigidbody->SetLinearVelocity(glm::vec2(Random::RandomInRange<float>(minVelocity, maxVelocity),
										Random::RandomInRange<float>(minVelocity, maxVelocity)));

	bool positive = Random::RandomInRange<int>(0, 1);
	if (!positive) rigidbody->GetLinearVelocity() *= -1;
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