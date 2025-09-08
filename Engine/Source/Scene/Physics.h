#pragma once
#include <vector>
#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "Component/Collider.h"

class Scene;
class Entity;

////////////////////

struct Collision
{
	bool detected = false;
	std::weak_ptr<Entity> entity;
	glm::vec2 contact = glm::vec2(0.f, 0.f);
	glm::vec2 normal = glm::vec2(0.f, 0.f);
	float depth = 0.f;
};

////////////////////

struct Ray
{
	glm::vec2 origin = glm::vec2(0, 0);
	glm::vec2 direction = glm::vec2(1, 0);
	float length = 0.f;
};

struct RaycastHit
{
	glm::vec2 contactPoint = glm::vec2(0, 0);
	std::weak_ptr<Entity> entity;
};

////////////////////

class Physics
{
public:
	void Update(float deltaTime);

	size_t AddCollider(const std::shared_ptr<Collider>& collider);
	void RemoveCollider(size_t id) { m_Colliders.erase(m_Colliders.begin() + id); }

	bool Raycast(const Ray& ray, const std::shared_ptr<RaycastHit>& hitResult);
	bool Raycast(const glm::vec2& origin, const glm::vec2& direction, const float length, const std::shared_ptr<RaycastHit>& hitResult) { return Raycast(Ray(origin, direction, length), hitResult); }

private:
	Physics() {}

	~Physics() {}

	void Collide(const size_t indexA, const ShapeType shapeA, const size_t indexB, const ShapeType shapeB, const std::shared_ptr<Collision>& collision);
	void Resolve(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);

	void CheckRectangleVsRectangle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);

	void CheckCircleVsRectangle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);

	void CheckCircleVsCircle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);

	void ProjectVertices(const std::array<glm::vec2, 4>& vertices, const glm::vec2& axis, float& min, float& max);
	void ProjectCircle(const glm::vec2& center, const float radius, const glm::vec2& axis, float& min, float& max);
	glm::vec2 FindClosestPointOnRectangle(const glm::vec2& circleCenter, const std::array<glm::vec2, 4>& vertices);

	// uses Separating Axis Theorem
	bool CheckSAT(const std::array<glm::vec2, 4>& verticesA, const std::array<glm::vec2, 4>& verticesB, const std::shared_ptr<Collision>& collision);
	bool CheckSAT(const glm::vec2& circleCenter, const float radius, const std::array<glm::vec2, 4>& vertices, const std::shared_ptr<Collision>& collision);

	bool CheckCircles(const glm::vec2& centerA, const float radiusA, const glm::vec2& centerB, const float radiusB, const std::shared_ptr<Collision>& collision);

	////////////////////

	std::vector<std::weak_ptr<Collider>> m_Colliders;

	friend class Scene;
};