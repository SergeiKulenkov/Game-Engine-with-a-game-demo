#pragma once
#include <vector>
#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "Component/Collider.h"
#include "../Utility/DataStructures/QuadTree.h"
#include "../Utility/DataStructures/SpatialHashGrid.h"

class Scene;
class Entity;
class Rigidbody;

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
	bool Raycast(const Ray& ray, RaycastHit& hitResult);
	bool Raycast(const glm::vec2& origin, const glm::vec2& direction, const float length, RaycastHit& hitResult) { return Raycast(Ray(origin, direction, length), hitResult); }

private:
	Physics();
	~Physics() {}

	void Start(const glm::vec2& screenSize);
	void Update(float deltaTime);

	// returns collider's Id
	size_t AddCollider(const std::weak_ptr<Collider>& collider);
	void RemoveCollider(const size_t id);

	// return rigidbody's Id
	size_t AddRigidbody(const std::weak_ptr<Rigidbody>& rigidbody);
	void RemoveRigidbody(const size_t id);

	// using a dynamic quad tree, so should be O(log n)
	void QuadTreeCollisionDetection();
	// check every collider against every other collider, O(n^2)
	void SimpleCollisionDetections();

	// check collisions using a spatial hash grid
	// the code is the same as for a quad tree but it needs an unordered_set for query results
	//void SpatialHashGridCollisions();

	void Collide(const size_t indexA, const ShapeType shapeA, const size_t indexB, const ShapeType shapeB, Collision& collision);
	void ResolveCollision(const size_t indexA, const size_t indexB, Collision& collision);
	float CalculateImpulseMagnitude(const float restitution, const float dotVelocityNormal, const float inverseMassSum);

	bool CheckAABBOverlap(const AABB& boxA, const AABB& boxB);

	void CheckRectangleVsRectangle(const size_t indexA, const size_t indexB, Collision& collision);

	void CheckCircleVsRectangle(const size_t indexA, const size_t indexB, Collision& collision);

	void CheckCircleVsCircle(const size_t indexA, const size_t indexB, Collision& collision);

	void ProjectVertices(const std::array<glm::vec2, 4>& vertices, const glm::vec2& axis, float& min, float& max);
	void ProjectCircle(const glm::vec2& center, const float radius, const glm::vec2& axis, float& min, float& max);
	glm::vec2 FindClosestPointOnRectangle(const glm::vec2& circleCenter, const std::array<glm::vec2, 4>& vertices);

	// uses Separating Axis Theorem
	bool CheckSAT(const std::array<glm::vec2, 4>& verticesA, const std::array<glm::vec2, 4>& verticesB, Collision& collision);
	bool CheckSAT(const glm::vec2& circleCenter, const float radius, const std::array<glm::vec2, 4>& vertices, Collision& collision);
	void GetDepthAndNormal(const float axisDepth, const glm::vec2& axis, Collision& collision);

	bool CheckCircles(const glm::vec2& centerA, const float radiusA, const glm::vec2& centerB, const float radiusB, Collision& collision);

	bool RaycastAgainstBox(const std::shared_ptr<BoxCollider>& box, const glm::vec2& rayOrigin, const glm::vec2& rayEnd, const bool infiniteRay, RaycastHit& hitResult);
	bool RaycastAgainstCircle(const std::shared_ptr<CircleCollider>& circle, const glm::vec2& rayOrigin, const glm::vec2& rayEnd, const bool infiniteRay, RaycastHit& hitResult);

	////////////////////

	static constexpr uint16_t defaultObjectsCount = 100;

	std::vector<std::weak_ptr<Collider>> m_Colliders;
	std::vector<std::weak_ptr<Rigidbody>> m_Rigidbodies;
	
	QuadTree<size_t> m_QuadTree = QuadTree<size_t>(glm::vec2(0.f, 0.f), glm::vec2(1920.f, 1080.f));
	std::vector<size_t> m_QuadTreeQueryResult;

	friend class Scene;
};