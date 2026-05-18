#pragma once
#include <vector>
#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "Component/Collider.h"
#include "Component/Rigidbody.h"
#include "../Utility/DataStructures/Quadtree.h"
#include "../Utility/DataStructures/SpatialHashGrid.h"

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
	glm::vec2 origin = glm::vec2(0.f, 0.f);
	glm::vec2 direction = glm::vec2(1.f, 0.f);
	float length = 0.f;
};

struct RaycastHit
{
	glm::vec2 contactPoint = glm::vec2(0.f, 0.f);
	std::weak_ptr<Entity> entity;
};

////////////////////

class Physics
{
public:
	size_t AddCollider(const std::weak_ptr<Collider>& collider);
	void RemoveCollider(const size_t id);

	size_t AddRigidbody(const std::weak_ptr<Rigidbody>& rigidbody);
	void RemoveRigidbody(const size_t id);

	bool Raycast(const Ray& ray, const std::shared_ptr<RaycastHit>& hitResult);
	bool Raycast(const glm::vec2& origin, const glm::vec2& direction, const float length, const std::shared_ptr<RaycastHit>& hitResult) { return Raycast(Ray(origin, direction, length), hitResult); }

private:
	Physics() {}
	~Physics() {}

	void Start(const glm::vec2& screenSize);

	void Update(float deltaTime);

	void SpatialHashGridCollisions();
	// using a static quad tree (it's rebuilt every frame)
	void QuadTreeCollisionDetection();
	// check every collider against every other collider, O(n^2)
	void SimpleCollisionDetections();

	void Collide(const size_t indexA, const ShapeType shapeA, const size_t indexB, const ShapeType shapeB, const std::shared_ptr<Collision>& collision);
	void ResolveCollision(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);
	float CalculateImpulseMagnitude(const float restitution, const float dotVelocityNormal, const float inverseMassSum);

	bool CheckAABBOverlap(const AABB& boxA, const AABB& boxB);

	void CheckRectangleVsRectangle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);

	void CheckCircleVsRectangle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);

	void CheckCircleVsCircle(const size_t indexA, const size_t indexB, const std::shared_ptr<Collision>& collision);

	void ProjectVertices(const std::array<glm::vec2, 4>& vertices, const glm::vec2& axis, float& min, float& max);
	void ProjectCircle(const glm::vec2& center, const float radius, const glm::vec2& axis, float& min, float& max);
	glm::vec2 FindClosestPointOnRectangle(const glm::vec2& circleCenter, const std::array<glm::vec2, 4>& vertices);

	// uses Separating Axis Theorem
	bool CheckSAT(const std::array<glm::vec2, 4>& verticesA, const std::array<glm::vec2, 4>& verticesB, const std::shared_ptr<Collision>& collision);
	bool CheckSAT(const glm::vec2& circleCenter, const float radius, const std::array<glm::vec2, 4>& vertices, const std::shared_ptr<Collision>& collision);
	void GetDepthAndNormal(const float axisDepth, const glm::vec2& axis, const std::shared_ptr<Collision>& collision);

	bool CheckCircles(const glm::vec2& centerA, const float radiusA, const glm::vec2& centerB, const float radiusB, const std::shared_ptr<Collision>& collision);

	bool RaycastAgainstBox(const std::shared_ptr<BoxCollider>& box, const glm::vec2& rayOrigin, const glm::vec2& rayEnd, const bool infiniteRay, RaycastHit& hitResult);
	bool RaycastAgainstCircle(const std::shared_ptr<CircleCollider>& circle, const glm::vec2& rayOrigin, const glm::vec2& rayEnd, const bool infiniteRay, RaycastHit& hitResult);

	////////////////////

	std::vector<std::weak_ptr<Collider>> m_Colliders;
	std::vector<std::weak_ptr<Rigidbody>> m_Rigidbodies;
	
	QuadTree<size_t> m_QuadTree = QuadTree<size_t>(glm::vec2(0.f, 0.f), glm::vec2(1920.f, 1080.f));
	SpatialHashGrid m_SpatialHashGrid = SpatialHashGrid(glm::vec2(1920, 1080));
	std::unordered_set<size_t> m_QueryResults;

	friend class Scene;
};