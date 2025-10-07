#include "Physics.h"
#include <limits>

#include "../Utility/Utility.h"

#define ASSERT_COLLIDER_SHARED_PTR(collider) assert(collider && "Can't get Collider's shared pointer because it's no longer valid.");
#define ASSERT_RIGIDBODY_SHARED_PTR(rigidbody) assert(rigidbody && "Can't get Rigidbody's shared pointer because it's no longer valid.");

////////////////////

void Physics::Update(float deltaTime)
{
	for (std::weak_ptr<Rigidbody>& rigidbody : m_Rigidbodies)
	{
		const std::shared_ptr<Rigidbody> sharedRigidbody = rigidbody.lock();
		ASSERT_RIGIDBODY_SHARED_PTR(sharedRigidbody);
		sharedRigidbody->Update(deltaTime);
	}

	// check collisions
	for (size_t indexA = 0; indexA < m_Colliders.size() - 1; indexA++)
	{
		const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
		ASSERT_COLLIDER_SHARED_PTR(sharedColliderA);
		if (!sharedColliderA->IsEnabled()) continue;

		for (size_t indexB = indexA + 1; indexB < m_Colliders.size(); indexB++)
		{
			const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
			ASSERT_COLLIDER_SHARED_PTR(sharedColliderB);

			if (!sharedColliderB->IsEnabled() ||
				(!sharedColliderA->IsDynamic() && !sharedColliderB->IsDynamic()))
			{
				continue;
			}

			if (CheckAABBOverlap(sharedColliderA->GetAABB(), sharedColliderB->GetAABB()))
			{
				std::shared_ptr<Collision> collision = std::make_shared<Collision>();
				Collide(indexA, sharedColliderA->GetType(), indexB, sharedColliderB->GetType(), collision);
				if (collision->detected) ResolveCollision(indexA, indexB, collision);
			}
		}
	}
}

size_t Physics::AddCollider(const std::weak_ptr<Collider>& collider)
{
	m_Colliders.push_back(collider);
	return m_Colliders.size() - 1;
}

size_t Physics::AddRigidbody(const std::weak_ptr<Rigidbody>& rigidbody)
{
	m_Rigidbodies.push_back(rigidbody);
	return m_Rigidbodies.size() - 1;;
}

bool Physics::Raycast(const Ray& ray, const std::shared_ptr<RaycastHit>& hitResult)
{
	hitResult->contactPoint = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	bool hasIntersected = false;
	bool isIntersecting = false;
	RaycastHit currentHitResult;
	size_t resultIndex = 0;

	const bool infiniteRay = ((ray.length == std::numeric_limits<float>::infinity()) || (ray.length == std::numeric_limits<float>::max()));
	const glm::vec2 rayEnd = infiniteRay ? (ray.origin + ray.direction) : (ray.origin + ray.direction * ray.length);

	for (size_t i = 0; i < m_Colliders.size(); i++)
	{
		const std::shared_ptr<Collider> sharedColliderA = m_Colliders[i].lock();
		ASSERT_COLLIDER_SHARED_PTR(sharedColliderA);
		if (!sharedColliderA->IsEnabled()) continue;

		const ShapeType shape = sharedColliderA->GetType();
		if (shape == ShapeType::Box)
		{
			isIntersecting = RaycastAgainstBox(std::dynamic_pointer_cast<BoxCollider>(sharedColliderA), ray.origin, rayEnd, infiniteRay, currentHitResult);
			
		}
		else if (shape == ShapeType::Circle)
		{
			isIntersecting = RaycastAgainstCircle(std::dynamic_pointer_cast<CircleCollider>(sharedColliderA), ray.origin, rayEnd, infiniteRay, currentHitResult);
		}

		if (isIntersecting)
		{
			if (!Vector::IsFirstPointCloser(ray.origin, hitResult->contactPoint, currentHitResult.contactPoint))
			{
				hitResult->contactPoint = currentHitResult.contactPoint;
				resultIndex = i;
				hasIntersected = true;
			}
		}
	}

	// better to set entity once at the end, than multiple times inside the loop
	if (hasIntersected) hitResult->entity = m_Colliders[resultIndex].lock()->GetEntity();

	return hasIntersected;
}

bool Physics::RaycastAgainstBox(const std::shared_ptr<BoxCollider>& box, const glm::vec2& rayOrigin, const glm::vec2& rayEnd, const bool infiniteRay, RaycastHit& hitResult)
{
	bool isIntersecting = false;
	const std::array<glm::vec2, 4> vertices = box->GetVertices();

	for (size_t i = 0; i < vertices.size(); i++)
	{
		const glm::vec2 nextVertex = vertices[(i + 1) % vertices.size()];
		// formula for line-line intersection
		const float denominator = (vertices[i].x - nextVertex.x) * (rayOrigin.y - rayEnd.y) - (vertices[i].y - nextVertex.y) * (rayOrigin.x - rayEnd.x);
		if (denominator == 0) continue;

		const float t = ((vertices[i].x - rayOrigin.x) * (rayOrigin.y - rayEnd.y) - (vertices[i].y - rayOrigin.y) * (rayOrigin.x - rayEnd.x)) / denominator;
		const float u = -((vertices[i].x - nextVertex.x) * (vertices[i].y - rayOrigin.y) - (vertices[i].y - nextVertex.y) * (vertices[i].x - rayOrigin.x)) / denominator;
		if ((t >= 0.f) && (t <= 1.f))
		{
			if ((!infiniteRay && (u >= 0.f) && (u <= 1.0f)) ||
				(infiniteRay && ((u >= 0.f))))
			{
				if (isIntersecting)
				{
					const glm::vec2 newContactPoint = glm::vec2(vertices[i].x + t * (nextVertex.x - vertices[i].x), vertices[i].y + t * (nextVertex.y - vertices[i].y));
					if (!Vector::IsFirstPointCloser(rayOrigin, hitResult.contactPoint, newContactPoint))
					{
						hitResult.contactPoint = newContactPoint;
					}
				}
				else
				{
					hitResult.contactPoint = glm::vec2(vertices[i].x + t * (nextVertex.x - vertices[i].x), vertices[i].y + t * (nextVertex.y - vertices[i].y));
					isIntersecting = true;
				}
			}
		}
	}

	return isIntersecting;
}

bool Physics::RaycastAgainstCircle(const std::shared_ptr<CircleCollider> circle, const glm::vec2& rayOrigin, const glm::vec2& rayEnd, const bool infiniteRay, RaycastHit& hitResult)
{
	bool isIntersecting = false;
	const glm::vec2 directionToCircle = rayOrigin - circle->GetPosition();
	const glm::vec2 rayEndOffset = rayEnd - rayOrigin;

	const float a = glm::dot(rayEndOffset, rayEndOffset);
	const float b = 2 * glm::dot(directionToCircle, rayEndOffset);
	const float c = glm::dot(directionToCircle, directionToCircle) - circle->GetRadius() * circle->GetRadius();
	float discriminant = b * b - 4 * a * c;

	if (discriminant >= 0)
	{
		discriminant = glm::sqrt(discriminant);
		// don't need the second solution cause this one is always the closest
		const float solution1 = (-b - discriminant) / (2 * a);
		if ((solution1 >= 0) && (solution1 <= 1) ||
			(infiniteRay && (solution1 > 1)))
		{
			hitResult.contactPoint = rayOrigin + solution1 * rayEndOffset;
			isIntersecting = true;
		}
	}

	return isIntersecting;
}

void Physics::Collide(const size_t indexA, const ShapeType shapeA, const size_t indexB, const ShapeType shapeB, const std::shared_ptr<Collision>& collision)
{
	if (shapeA == ShapeType::Box)
	{
		if (shapeB == ShapeType::Box)
		{
			CheckRectangleVsRectangle(indexA, indexB, collision);
		}
		else if (shapeB == ShapeType::Circle)
		{
			CheckCircleVsRectangle(indexB, indexA, collision);
		}
	}
	else if (shapeA == ShapeType::Circle)
	{
		if (shapeB == ShapeType::Box)
		{
			CheckCircleVsRectangle(indexA, indexB, collision);
		}
		else if (shapeB == ShapeType::Circle)
		{
			CheckCircleVsCircle(indexA, indexB, collision);
		}
	}
}

void Physics::ResolveCollision(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision)
{
	const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderA);
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderB);

	if (sharedColliderA->IsDynamic() && !sharedColliderB->IsDynamic())
	{
		const std::shared_ptr<Rigidbody> sharedRBA = m_Rigidbodies[sharedColliderA->GetRigidbodyId()].lock();
		ASSERT_RIGIDBODY_SHARED_PTR(sharedRBA);
		sharedRBA->MoveEntity(collision->normal * collision->depth * (-1.f));

		// if it's > 0 it means they're moving in the same direction
		const float dot = glm::dot(-sharedRBA->GetLinearVelocity(), collision->normal);
		if (dot <= 0.f)
		{
			const float j = CalculateImpulseMagnitude(sharedRBA->GetRestitution(), dot, sharedRBA->GetInverseMass());
			sharedRBA->GetLinearVelocity() -= sharedRBA->GetInverseMass() * j * collision->normal;
		}

		collision->entity = sharedColliderB->GetEntity();
		sharedColliderA->OnCollision(collision);
	}
	else if (sharedColliderB->IsDynamic() && !sharedColliderA->IsDynamic())
	{
		const std::shared_ptr<Rigidbody> sharedRBB = m_Rigidbodies[sharedColliderB->GetRigidbodyId()].lock();
		ASSERT_RIGIDBODY_SHARED_PTR(sharedRBB);
		sharedRBB->MoveEntity(collision->normal * collision->depth);

		const float dot = glm::dot(sharedRBB->GetLinearVelocity(), collision->normal);
		if (dot <= 0.f)
		{
			const float j = CalculateImpulseMagnitude(sharedRBB->GetRestitution(), dot, sharedRBB->GetInverseMass());
			sharedRBB->GetLinearVelocity() += sharedRBB->GetInverseMass() * j * collision->normal;
		}

		collision->entity = sharedColliderA->GetEntity();
		sharedColliderB->OnCollision(collision);
	}
	else
	{
		const std::shared_ptr<Rigidbody> sharedRBA = m_Rigidbodies[sharedColliderA->GetRigidbodyId()].lock();
		ASSERT_RIGIDBODY_SHARED_PTR(sharedRBA);
		const std::shared_ptr<Rigidbody> sharedRBB = m_Rigidbodies[sharedColliderB->GetRigidbodyId()].lock();
		ASSERT_RIGIDBODY_SHARED_PTR(sharedRBB);
		std::shared_ptr<Collision> collisionB = std::make_shared<Collision>(*collision);
		sharedRBA->MoveEntity(collision->normal * (collision->depth / 2.f) * (-1.f));
		sharedRBB->MoveEntity(collisionB->normal * (collisionB->depth / 2.f));

		const float dot = glm::dot((sharedRBB->GetLinearVelocity() - sharedRBA->GetLinearVelocity()), collision->normal);
		if (dot <= 0.f)
		{
			const float j = CalculateImpulseMagnitude(glm::min(sharedRBA->GetRestitution(), sharedRBB->GetRestitution()), dot, sharedRBA->GetInverseMass() + sharedRBB->GetInverseMass());
			sharedRBA->GetLinearVelocity() -= sharedRBA->GetInverseMass() * j * collision->normal;
			sharedRBB->GetLinearVelocity() += sharedRBA->GetInverseMass() * j * collision->normal;
		}

		collision->entity = sharedColliderB->GetEntity();
		sharedColliderA->OnCollision(collision);

		collisionB->entity = sharedColliderA->GetEntity();
		sharedColliderB->OnCollision(collisionB);
	}
}

float Physics::CalculateImpulseMagnitude(const float restitution, const float dotVelocityNormal, const float inverseMassSum)
{
	float j = -(1.f + restitution) * dotVelocityNormal;
	return j /= inverseMassSum;
}

bool Physics::CheckAABBOverlap(const AABB& boxA, const AABB& boxB)
{
	bool result = true;
	if (boxA.max.x <= boxB.min.x || boxB.max.x <= boxA.min.x ||
		boxA.max.y <= boxB.min.y || boxB.max.y <= boxA.min.y)
	{
		result = false;
	}

	return result;
}

void Physics::CheckRectangleVsRectangle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision)
{
	const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderA);
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderB);

	const std::shared_ptr<BoxCollider> rectangleA = std::dynamic_pointer_cast<BoxCollider>(sharedColliderA);
	const std::shared_ptr<BoxCollider> rectangleB = std::dynamic_pointer_cast<BoxCollider>(sharedColliderB);

	const std::array<glm::vec2, 4> verticesA = rectangleA->GetVertices();
	const std::array<glm::vec2, 4> verticesB = rectangleB->GetVertices();

	bool collided = CheckSAT(verticesA, verticesB, collision);
	if (collided) collided = CheckSAT(verticesB, verticesA, collision);

	if (collided)
	{
		collision->detected = true;
		const glm::vec2 positionA = rectangleA->GetPosition();
		const glm::vec2 positionB = rectangleB->GetPosition();

		if (glm::dot(positionB - positionA, collision->normal) < 0.f)
		{
			collision->normal *= -1;
		}
	}
}

void Physics::Physics::CheckCircleVsRectangle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision)
{
	const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderA);
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderB);

	const std::shared_ptr<CircleCollider> circle = std::dynamic_pointer_cast<CircleCollider>(sharedColliderA);
	const std::shared_ptr<BoxCollider> rectangle = std::dynamic_pointer_cast<BoxCollider>(sharedColliderB);

	bool collided = CheckSAT(circle->GetPosition(), circle->GetRadius(), rectangle->GetVertices(), collision);
	if (collided)
	{
		collision->detected = true;
		const glm::vec2 positionA = circle->GetPosition();
		const glm::vec2 positionB = rectangle->GetPosition();
		float dotProduct = 0.f;
		if (indexA < indexB)
		{
			dotProduct = glm::dot(positionB - positionA, collision->normal);
		}
		else
		{
			dotProduct = glm::dot(positionA - positionB, collision->normal);
		}

		if (dotProduct < 0.f)
		{
			collision->normal *= -1;
		}
	}
}

void Physics::Physics::CheckCircleVsCircle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision)
{
	const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderA);
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	ASSERT_COLLIDER_SHARED_PTR(sharedColliderB);

	const std::shared_ptr<CircleCollider> circleA = std::dynamic_pointer_cast<CircleCollider>(sharedColliderA);
	const std::shared_ptr<CircleCollider> circleB = std::dynamic_pointer_cast<CircleCollider>(sharedColliderB);

	const glm::vec2 centerA = circleA->GetPosition();
	const glm::vec2 centerB = circleB->GetPosition();
	const float radiusA = circleA->GetRadius();
	const float radiusB = circleB->GetRadius();

	collision->detected = CheckCircles(centerA, radiusA, centerB, radiusB, collision);
}

void Physics::ProjectVertices(const std::array<glm::vec2, 4>& vertices, const glm::vec2& axis, float& min, float& max)
{
	min = std::numeric_limits<float>::max();
	max = -std::numeric_limits<float>::max();
	for (const glm::vec2& vertex : vertices)
	{
		const float projection = glm::dot(vertex, axis);
		if (projection < min)
		{
			min = projection;
		}
		if (projection > max)
		{
			max = projection;
		}
	}
}

void Physics::ProjectCircle(const glm::vec2& center, const float radius, const glm::vec2& axis, float& min, float& max)
{
	const glm::vec2 distanceOnAxis = axis * radius;
	const glm::vec2 rightBorder = center + distanceOnAxis;
	const glm::vec2 leftBorder = center - distanceOnAxis;

	min = glm::dot(rightBorder, axis);
	max = glm::dot(leftBorder, axis);
	if (min > max)
	{
		float tmp = min;
		min = max;
		max = tmp;
	}
}

glm::vec2 Physics::FindClosestPointOnRectangle(const glm::vec2& circleCenter, const std::array<glm::vec2, 4>& vertices)
{
	glm::vec2 result = glm::vec2(0, 0);
	float minDistance = std::numeric_limits<float>::max();
	float distance = 0.f;

	for (const glm::vec2& vertex : vertices)
	{
		distance = glm::distance(vertex, circleCenter);
		if (distance < minDistance)
		{
			minDistance = distance;
			result = vertex;
		}
	}

	return result;
}

bool Physics::CheckSAT(const std::array<glm::vec2, 4>& verticesA, const std::array<glm::vec2, 4>& verticesB, const std::shared_ptr<Collision>& collision)
{
	collision->depth = std::numeric_limits<float>::max();
	bool collided = true;

	glm::vec2 axis = glm::vec2(0, 0);
	glm::vec2 edge = glm::vec2(0, 0);
	float minA = 0.f;
	float maxA = 0.f;
	float minB = 0.f;
	float maxB = 0.f;

	for (size_t i = 0; i < verticesA.size(); i++)
	{
		edge = verticesA[(i + 1) % verticesA.size()] - verticesA[i];
		// normal of each edge
		axis = glm::normalize(glm::vec2(-edge.y, edge.x));

		ProjectVertices(verticesA, axis, minA, maxA);
		ProjectVertices(verticesB, axis, minB, maxB);

		if ((minA >= maxB) || (minB >= maxA))
		{
			collided = false;
			break;
		}
		else GetDepthAndNormal(glm::min(maxB - minA, maxA - minB), axis, collision);
	}

	return collided;
}

bool Physics::CheckSAT(const glm::vec2& circleCenter, const float radius, const std::array<glm::vec2, 4>& vertices, const std::shared_ptr<Collision>& collision)
{
	collision->depth = std::numeric_limits<float>::max();
	bool collided = true;

	glm::vec2 axis = glm::vec2(0, 0);
	glm::vec2 edge = glm::vec2(0, 0);
	float minA = 0.f;
	float maxA = 0.f;
	float minB = 0.f;
	float maxB = 0.f;

	for (size_t i = 0; i < vertices.size(); i++)
	{
		edge = vertices[(i + 1) % vertices.size()] - vertices[i];
		// normal of each edge
		axis = glm::normalize(glm::vec2(-edge.y, edge.x));

		ProjectVertices(vertices, axis, minA, maxA);
		ProjectCircle(circleCenter, radius, axis, minB, maxB);

		if ((minA >= maxB) || (minB >= maxA))
		{
			collided = false;
			break;
		}
		else GetDepthAndNormal(glm::min(maxB - minA, maxA - minB), axis, collision);
	}

	axis = glm::normalize(FindClosestPointOnRectangle(circleCenter, vertices) - circleCenter);
	ProjectVertices(vertices, axis, minA, maxA);
	ProjectCircle(circleCenter, radius, axis, minB, maxB);

	if (minA >= maxB || minB >= maxA)
	{
		collided = false;
	}
	else GetDepthAndNormal(glm::min(maxB - minA, maxA - minB), axis, collision);

	return collided;
}

void Physics::GetDepthAndNormal(const float axisDepth, const glm::vec2& axis, const std::shared_ptr<Collision>& collision)
{
	if (axisDepth < collision->depth)
	{
		collision->depth = axisDepth;
		collision->normal = axis;
	}
}

bool Physics::CheckCircles(const glm::vec2& centerA, const float radiusA, const glm::vec2& centerB, const float radiusB, const std::shared_ptr<Collision>& collision)
{
	bool collided = false;

	const float distance = glm::distance(centerA, centerB);
	const float radiiSum = radiusA + radiusB;

	if (distance < radiiSum)
	{
		collided = true;
		collision->normal = glm::normalize(centerB - centerA);
		collision->depth = radiiSum - distance;
	}

	return collided;
}