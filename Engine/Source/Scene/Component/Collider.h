#pragma once
#include <memory>
#include <array>
#include <limits>

#include "Component.h"
#include "Transform.h"
#include "../Entity.h"
#include "../../Utility/Utility.h"

struct Collsion;
class Rigidbody;

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
	{ }

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
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		ASSERT_ENTITY_SHARED_PTR(sharedEntity);
		sharedEntity->OnCollision(other);
	}

	bool IsDynamic() const { return m_IsDynamic; }
	bool IsEnabled() const { return m_Enabled; }

	void Disable() { m_Enabled = false; }
	void Enable() { m_Enabled = true; }

	ShapeType GetType() const { return m_Type; }
	size_t GetId() const { return m_Id; }

	size_t GetRigidbodyId() const
	{
		ASSERT_COLLIDER_DYNAMIC(m_IsDynamic);
		return m_RigidbodyId;
	}

	glm::vec2 GetPosition() const { return m_TransformData->position; }
	glm::vec2& GetPosition() { return m_TransformData->position; }
	void SetPosition(const glm::vec2& position) { m_TransformData->position = position; }

	void MoveCollider(const glm::vec2& amount)
	{
		// change the relative position
	}

	virtual AABB GetAABB() = 0;

protected:
	Collider(bool enabled = true)
		: m_Enabled(enabled), m_AABB(glm::vec2(0, 0), glm::vec2(0, 0))
	{}

	virtual ~Collider() {}

	virtual void OnInit() override
	{
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		ASSERT_ENTITY_SHARED_PTR(sharedEntity);

		// Entity must have a Transform to use a Collider
		ASSERRT_HAS_TRANSFORM(sharedEntity->HasComponent<Transform>());
		m_TransformData = sharedEntity->GetComponent<Transform>()->GetTransformData();

		if (!m_IsDynamic)
		{
			if (sharedEntity->HasComponent<Rigidbody>()) m_IsDynamic = true;
			else m_AABB = GetAABB();
		}
	}

	void SetDynamic(bool isDynamic, size_t rigidbodyId)
	{
		m_IsDynamic = isDynamic;
		m_RigidbodyId = rigidbodyId;

		if (!m_IsDynamic) m_AABB = GetAABB();
	}

	////////////////////

	size_t m_Id = 0;

	bool m_IsDynamic = false;

	bool m_Enabled = true;

	std::shared_ptr<TransformData> m_TransformData = std::make_shared<TransformData>();

	ShapeType m_Type = ShapeType::Undefined;

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

	virtual void OnInit() override
	{
		Collider::OnInit();
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		ASSERT_ENTITY_SHARED_PTR(sharedEntity);
		m_Id = sharedEntity->RegisterCollider(typeid(BoxCollider).hash_code());
	}

	glm::vec2 GetSize() const { return m_Size; }
	glm::vec2& GetSize() { return m_Size; }
	void SetSize(const glm::vec2& size) { m_Size = size; }

	glm::vec2 GetRotation() const { return m_TransformData->rotation; }
	glm::vec2& GetRotation() { return m_TransformData->rotation; }
	void SetRotation(const glm::vec2& rotation) { m_TransformData->rotation = rotation; }

	std::array<glm::vec2, 4> GetVertices() const
	{
		std::array<glm::vec2, 4> vertices;
		vertices[0] = glm::vec2(m_TransformData->position.x - m_Size.x / 2.f, m_TransformData->position.y - m_Size.y / 2.f);
		vertices[1] = glm::vec2(m_TransformData->position.x + m_Size.x / 2.f, m_TransformData->position.y - m_Size.y / 2.f);
		vertices[2] = glm::vec2(m_TransformData->position.x + m_Size.x / 2.f, m_TransformData->position.y + m_Size.y / 2.f);
		vertices[3] = glm::vec2(m_TransformData->position.x - m_Size.x / 2.f, m_TransformData->position.y + m_Size.y / 2.f);

		const PairCosSin pairCosSin = Vector::GetCosAndSinFromVector(m_TransformData->rotation);
		for (glm::vec2& vertex : vertices)
		{
			vertex = m_TransformData->position + Vector::Rotate(vertex - m_TransformData->position, pairCosSin);
		}

		return vertices;
	}

	virtual AABB GetAABB() override
	{
		if (m_IsDynamic ||
			(!m_IsDynamic && m_AABB.min == glm::vec2(0, 0) && m_AABB.max == glm::vec2(0, 0)))
		{
			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();

			for (const glm::vec2& vertex : GetVertices())
			{
				if (vertex.x < minX) minX = vertex.x;
				if (vertex.x > maxX) maxX = vertex.x;
				if (vertex.y < minY) minY = vertex.y;
				if (vertex.y > maxY) maxY = vertex.y;
			}

			m_AABB = AABB(glm::vec2(minX, minY), glm::vec2(maxX, maxY));
		}

		return m_AABB;
	}

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

	virtual void OnInit() override
	{
		Collider::OnInit();
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		ASSERT_ENTITY_SHARED_PTR(sharedEntity);
		m_Id = sharedEntity->RegisterCollider(typeid(CircleCollider).hash_code());
	}

	float GetRadius() const { return m_Radius; }
	float& GetRadius() { return m_Radius; }
	void SetRadius(const float radius) { m_Radius = radius; }

	virtual AABB GetAABB() override
	{
		if (m_IsDynamic ||
			(!m_IsDynamic && m_AABB.min == glm::vec2(0, 0) && m_AABB.max == glm::vec2(0, 0)))
		{
			const float minX = m_TransformData->position.x - m_Radius;
			const float maxX = m_TransformData->position.x + m_Radius;
			const float minY = m_TransformData->position.y - m_Radius;
			const float maxY = m_TransformData->position.y + m_Radius;
			m_AABB = AABB(glm::vec2(minX, minY), glm::vec2(maxX, maxY));
		}
		return m_AABB;
	}

private:
	float m_Radius = 0.f;
};