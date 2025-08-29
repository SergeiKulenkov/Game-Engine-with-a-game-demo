#pragma once
#include <vector>
#include <array>
#include <memory>

#include <glm/glm.hpp>

class Scene;
class Entity;
class Collider;

////////////////////

struct Collision
{
	bool detected = false;
	Entity* entity = nullptr;
	glm::vec2 contact = glm::vec2(0.f, 0.f);
	glm::vec2 normal = glm::vec2(0.f, 0.f);
	float depth = 0.f;
};

////////////////////

class Physics
{
public:
	void Update(float deltaTime);

	void AddCollider(Collider& collider) { m_Colliders.push_back(&collider); }

private:
	Physics() {}

	~Physics() {}

	void Collide(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);
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

	std::vector<Collider*> m_Colliders;

	friend class Scene;
};