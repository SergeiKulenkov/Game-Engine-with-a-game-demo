#pragma once
#include <memory>
#include <array>

#include "Component.h"
#include "Transform.h"
#include "../Entity.h"
#include "../../Utility/Utility.h"

struct Collsion;

////////////////////

enum class ShapeType
{
	Undefined,
	Box,
	Circle,
};

////////////////////

class Collider : public Component
{
public:
	Collider() = delete;

	virtual void OnCollision(const std::shared_ptr<Collision>& other)
	{
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		assert(sharedEntity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");
		sharedEntity->OnCollision(other);
	}

	bool IsDynamic() const { return m_Dynamic; }
	bool IsEnabled() const { return m_Enabled; }

	void Disable() { m_Enabled = false; }
	void Enable() { m_Enabled = true; }

	ShapeType GetType() const { return m_Type; }
	size_t GetId() const { return m_Id; }

	glm::vec2 GetPosition() const { return m_TransformData->position; }
	glm::vec2& GetPosition() { return m_TransformData->position; }

	glm::vec2 GetRotation() const { return m_TransformData->rotation; }
	glm::vec2& GetRotation() { return m_TransformData->rotation; }

	void SetPosition(const glm::vec2& position) { m_TransformData->position = position; }
	void SetRotation(const glm::vec2& rotation) { m_TransformData->rotation= rotation; }

	void MoveEntity(const glm::vec2& amount)
	{
		m_TransformData->position += amount;
	}

protected:
	Collider(bool isStatic, bool enabled = true)
		: m_Dynamic(isStatic)
	{}

	virtual ~Collider() {};

	virtual void OnInit() override
	{
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		assert(sharedEntity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");

		// Entity must have a Transform to use a Collider
		assert(sharedEntity->HasComponent<Transform>() && "Tranform Component is not present.");
		m_TransformData = sharedEntity->GetComponent<Transform>()->GetTransformData();
	}

	////////////////////

	size_t m_Id;

	bool m_Dynamic = false;

	bool m_Enabled = true;

	std::shared_ptr<TransformData> m_TransformData = std::make_shared<TransformData>();

	ShapeType m_Type = ShapeType::Undefined;

private:
};

////////////////////

class BoxCollider : public Collider
{
public:
	BoxCollider(const glm::vec2& size, bool isDynamic = false, bool enabled = true)
		: Collider(isDynamic, enabled), m_Size(size)
	{
		m_Type = ShapeType::Box;
	}

	virtual void OnInit() override
	{
		Collider::OnInit();
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		assert(sharedEntity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");
		m_Id = sharedEntity->RegisterCollider(typeid(BoxCollider).hash_code());
	}

	glm::vec2 GetSize() const { return m_Size; }
	glm::vec2& GetSize() { return m_Size; }
	void SetSize(const glm::vec2& size) { m_Size = size; }

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

private:
	glm::vec2 m_Size = glm::vec2(0.f, 0.f);
};

////////////////////

class CircleCollider : public Collider
{
public:
	CircleCollider(const float radius = 0.f, bool isDynamic = false, bool enabled = true)
		: Collider(isDynamic, enabled), m_Radius(radius)
	{
		m_Type = ShapeType::Circle;
	}

	virtual void OnInit() override
	{
		Collider::OnInit();
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		assert(sharedEntity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");
		m_Id = sharedEntity->RegisterCollider(typeid(CircleCollider).hash_code());
	}

	float GetRadius() const { return m_Radius; }
	float& GetRadius() { return m_Radius; }
	void SetRadius(const float radius) { m_Radius = radius; }

private:
	float m_Radius = 0.f;
};