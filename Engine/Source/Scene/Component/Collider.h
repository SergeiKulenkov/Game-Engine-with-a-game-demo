#pragma once
#include <memory>
#include <array>

#include "Component.h"
#include "Transform.h"
#include "../Entity.h"

struct Collsion;
class Rigidbody;
class Entity;

#define ASSERT_COLLIDER_DYNAMIC(isDynamic) assert(isDynamic && "This collider is not dynamic, so doesn't have a rigidbody connected to it.");

////////////////////

enum class ShapeType
{
	Undefined,
	Box,
	Circle,
};

////////////////////

struct AABB
{
	AABB(const glm::vec2& min, const glm::vec2& max)
		: min(min), max(max)
	{}

	glm::vec2 min;
	glm::vec2 max;
};

////////////////////

class Collider : public Component
{
public:
	Collider() = delete;

	virtual void OnCollision(const std::shared_ptr<Collision>& other)
	{
		if (!m_Entity.expired())
		{
			const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
			ASSERT_ENTITY_SHARED_PTR(sharedEntity);
			sharedEntity->OnCollision(other);
		}
	}

	bool IsDynamic() const { return m_IsDynamic; }
	bool IsEnabled() const { return m_Enabled; }

	void Disable() { m_Enabled = false; }
	void Enable() { m_Enabled = true; }

	ShapeType GetType() const { return m_Type; }
	size_t GetId() const { return m_Id; }

	size_t GetRigidbodyId() const;

	glm::vec2 GetPosition() const { return m_TransformData->position; }
	glm::vec2& GetPosition() { return m_TransformData->position; }
	void SetPosition(const glm::vec2& position) { m_TransformData->position = position; }

	// change the relative position
	//void MoveCollider(const glm::vec2& amount);

	virtual AABB GetAABB() = 0;

protected:
	Collider(bool enabled = true)
		: m_Enabled(enabled), m_AABB(glm::vec2(0, 0), glm::vec2(0, 0))
	{}

	virtual ~Collider() {}

	virtual void OnInit() override;
	virtual void OnRemove() override;

	void SetDynamic(bool isDynamic, size_t rigidbodyId);

	void RegisterCollider(const size_t colliderType);

	////////////////////

	size_t m_Id = 0;
	ShapeType m_Type = ShapeType::Undefined;

	bool m_IsDynamic = false;
	bool m_Enabled = true;

	std::shared_ptr<TransformData> m_TransformData = std::make_shared<TransformData>();
	AABB m_AABB;

private:
	size_t m_RigidbodyId = 0;

	friend class Rigidbody;
};

////////////////////

class BoxCollider : public Collider
{
public:
	BoxCollider(const glm::vec2& size, bool enabled = true)
		: Collider(enabled), m_Size(size)
	{
		m_Type = ShapeType::Box;
	}

	virtual void OnInit() override;

	glm::vec2 GetSize() const { return m_Size; }
	glm::vec2& GetSize() { return m_Size; }
	void SetSize(const glm::vec2& size) { m_Size = size; }

	glm::vec2 GetRotation() const { return m_TransformData->rotation; }
	glm::vec2& GetRotation() { return m_TransformData->rotation; }
	void SetRotation(const glm::vec2& rotation) { m_TransformData->rotation = rotation; }

	std::array<glm::vec2, 4> GetVertices() const;
	virtual AABB GetAABB() override;

private:
	glm::vec2 m_Size = glm::vec2(0.f, 0.f);
};

////////////////////

class CircleCollider : public Collider
{
public:
	CircleCollider(const float radius = 0.f, bool enabled = true)
		: Collider(enabled), m_Radius(radius)
	{
		m_Type = ShapeType::Circle;
	}

	virtual void OnInit() override;

	float GetRadius() const { return m_Radius; }
	float& GetRadius() { return m_Radius; }
	void SetRadius(const float radius) { m_Radius = radius; }

	virtual AABB GetAABB() override;

private:
	float m_Radius = 0.f;
};