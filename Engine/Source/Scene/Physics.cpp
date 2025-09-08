#include "Physics.h"
#include <limits>

////////////////////

void Physics::Update(float deltaTime)
{
	for (size_t indexA = 0; indexA < m_Colliders.size() - 1; indexA++)
	{
		const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
		assert(sharedColliderA && "Can't get Collider's shared pointer because it's no longer valid.");
		if (!sharedColliderA->IsEnabled()) continue;

		for (size_t indexB = indexA + 1; indexB < m_Colliders.size(); indexB++)
		{
			const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
			assert(sharedColliderB && "Can't get Collider's shared pointer because it's no longer valid.");

			if (!sharedColliderB->IsEnabled() ||
				(!sharedColliderA->IsDynamic() && !sharedColliderB->IsDynamic()))
			{
				continue;
			}

			std::shared_ptr<Collision> collision = std::make_shared<Collision>();
			Collide(indexA, sharedColliderA->GetType(), indexB, sharedColliderB->GetType(), collision);
			if (collision->detected) Resolve(indexA, indexB, collision);
		}
	}
}

size_t Physics::AddCollider(const std::shared_ptr<Collider>& collider)
{
	m_Colliders.push_back(collider);
	return m_Colliders.size() - 1;
}

bool Physics::Raycast(const Ray& ray, const std::shared_ptr<RaycastHit>& hitResult)
{
	bool result = false;
	const bool infiniteRay = ((ray.length == std::numeric_limits<float>::infinity()) || (ray.length == std::numeric_limits<float>::max()));
	const glm::vec2 rayEnd = infiniteRay ? (ray.origin + ray.direction) : (ray.origin + ray.direction * ray.length);

	for (size_t i = 0; i < m_Colliders.size(); i++)
	{
		const std::shared_ptr<Collider> sharedColliderA = m_Colliders[i].lock();
		assert(sharedColliderA && "Can't get Collider's shared pointer because it's no longer valid.");
		if (!sharedColliderA->IsEnabled()) continue;

		const ShapeType shape = sharedColliderA->GetType();
		if (shape == ShapeType::Box)
		{
			const std::shared_ptr<BoxCollider> rectangle = std::dynamic_pointer_cast<BoxCollider>(sharedColliderA);
			const std::array<glm::vec2, 4> vertices = rectangle->GetVertices();

			for (size_t j = 0; j < vertices.size(); j++)
			{
				const glm::vec2 nextVertex = vertices[(j + 1) % vertices.size()];
				// formula for line-line intersection
				const float denominator = (vertices[j].x - nextVertex.x) * (ray.origin.y - rayEnd.y) - (vertices[j].y - nextVertex.y) * (ray.origin.x - rayEnd.x);
				if (denominator == 0) continue;

				const float t = ((vertices[j].x - ray.origin.x) * (ray.origin.y - rayEnd.y) - (vertices[j].y - ray.origin.y) * (ray.origin.x - rayEnd.x)) / denominator;
				const float u = -((vertices[j].x - nextVertex.x) * (vertices[j].y - ray.origin.y) - (vertices[j].y - nextVertex.y) * (vertices[j].x - ray.origin.x)) / denominator;
				if ((t >= 0.f) && (t <= 1.f))
				{
					if ((!infiniteRay && (u >= 0.f) && (u <= 1.0f)) ||
						(infiniteRay && ((u >= 0.f))))
					{
						if (result)
						{
							const glm::vec2 newContactPoint = glm::vec2(vertices[j].x + t * (nextVertex.x - vertices[j].x), vertices[j].y + t * (nextVertex.y - vertices[j].y));
							const glm::vec2 diff1 = glm::abs(ray.origin - newContactPoint);
							const glm::vec2 diff2 = glm::abs(ray.origin - hitResult->contactPoint);
							// avoiding square root for calculating distance
							if (glm::dot(diff1, diff1) < glm::dot(diff2, diff2))
							{
								hitResult->contactPoint = newContactPoint;
							}
						}
						else
						{
							hitResult->contactPoint = glm::vec2(vertices[j].x + t * (nextVertex.x - vertices[j].x), vertices[j].y + t * (nextVertex.y - vertices[j].y));
							hitResult->entity = rectangle->GetEntity();
							result = true;
						}
					}
				}
			}
		}
		else if (shape == ShapeType::Circle)
		{
			const std::shared_ptr<CircleCollider> circle = std::dynamic_pointer_cast<CircleCollider>(sharedColliderA);
			const float radius = circle->GetRadius();
			const glm::vec2 center = circle->GetPosition();
			const glm::vec2 directionToCircle = ray.origin - center;
			const glm::vec2 rayDirection = rayEnd - ray.origin;

			const float a = glm::dot(rayDirection, rayDirection);
			const float b = 2 * glm::dot(directionToCircle, rayDirection);
			const float c = glm::dot(directionToCircle, directionToCircle) - radius * radius;
			float discriminant = b * b - 4 * a * c;

			if (discriminant >= 0)
			{
				discriminant = glm::sqrt(discriminant);
				// don't need the second solution cause this one is always the closest
				const float solution1 = (-b - discriminant) / (2 * a);
				if ((solution1 >= 0) && (solution1 <= 1) ||
					(infiniteRay && (solution1 > 1)))
				{
					hitResult->contactPoint = ray.origin + solution1 * rayDirection;
					hitResult->entity = circle->GetEntity();
					result = true;
				}
			}
		}
	}

	return result;
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

void Physics::Resolve(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision)
{
	const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
	assert(sharedColliderA && "Can't get Collider's shared pointer because it's no longer valid.");
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	assert(sharedColliderB && "Can't get Collider's shared pointer because it's no longer valid.");

	if (sharedColliderA->IsDynamic() && !sharedColliderB->IsDynamic())
	{
		sharedColliderA->MoveEntity((-1.f) * collision->normal * collision->depth);
		collision->entity = sharedColliderB->GetEntity();
		sharedColliderA->OnCollision(collision);
	}
	else if (sharedColliderB->IsDynamic() && !sharedColliderA->IsDynamic())
	{
		sharedColliderB->MoveEntity(collision->normal * collision->depth);
		collision->entity = sharedColliderA->GetEntity();
		sharedColliderA->OnCollision(collision);
	}
	else
	{
		sharedColliderA->MoveEntity((-1.f) * collision->normal * collision->depth / 2.f);
		collision->entity = sharedColliderB->GetEntity();
		sharedColliderA->OnCollision(collision);

		std::shared_ptr<Collision> collision2 = std::make_shared<Collision>(*collision);
		sharedColliderB->MoveEntity(collision2->normal * collision2->depth / 2.f);
		collision2->entity = sharedColliderA->GetEntity();
		sharedColliderB->OnCollision(collision2);
	}
}

void Physics::CheckRectangleVsRectangle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision)
{
	const std::shared_ptr<Collider> sharedColliderA = m_Colliders[indexA].lock();
	assert(sharedColliderA && "Can't get Collider's shared pointer because it's no longer valid.");
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	assert(sharedColliderB && "Can't get Collider's shared pointer because it's no longer valid.");

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
	assert(sharedColliderA && "Can't get Collider's shared pointer because it's no longer valid.");
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	assert(sharedColliderB && "Can't get Collider's shared pointer because it's no longer valid.");

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
	assert(sharedColliderA && "Can't get Collider's shared pointer because it's no longer valid.");
	const std::shared_ptr<Collider> sharedColliderB = m_Colliders[indexB].lock();
	assert(sharedColliderB && "Can't get Collider's shared pointer because it's no longer valid.");

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
	float axisDepth = 0.f;
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

		if (minA >= maxB || minB >= maxA)
		{
			collided = false;
			break;
		}
		else
		{
			axisDepth = glm::min(maxB - minA, maxA - minB);
			if (axisDepth < collision->depth)
			{
				collision->depth = axisDepth;
				collision->normal = axis;
			}
		}
	}

	return collided;
}

bool Physics::CheckSAT(const glm::vec2& circleCenter, const float radius, const std::array<glm::vec2, 4>& vertices, const std::shared_ptr<Collision>& collision)
{
	collision->depth = std::numeric_limits<float>::max();
	bool collided = true;

	glm::vec2 axis = glm::vec2(0, 0);
	glm::vec2 edge = glm::vec2(0, 0);
	float axisDepth = 0.f;
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

		if (minA >= maxB || minB >= maxA)
		{
			collided = false;
			break;
		}
		else
		{
			axisDepth = glm::min(maxB - minA, maxA - minB);
			if (axisDepth < collision->depth)
			{
				collision->depth = axisDepth;
				collision->normal = axis;
			}
		}
	}

	axis = glm::normalize(FindClosestPointOnRectangle(circleCenter, vertices) - circleCenter);
	ProjectVertices(vertices, axis, minA, maxA);
	ProjectCircle(circleCenter, radius, axis, minB, maxB);

	if (minA >= maxB || minB >= maxA)
	{
		collided = false;
	}
	else
	{
		axisDepth = glm::min(maxB - minA, maxA - minB);
		if (axisDepth < collision->depth)
		{
			collision->depth = axisDepth;
			collision->normal = axis;
		}
	}

	return collided;
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