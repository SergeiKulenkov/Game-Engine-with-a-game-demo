#include "Collider.h"

#include <limits>

#include "Rigidbody.h"
#include "../../Utility/Utility.h"

////////////////////

void Collider::OnInit()
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

void Collider::SetDynamic(bool isDynamic, size_t rigidbodyId)
{
	m_IsDynamic = isDynamic;
	m_RigidbodyId = rigidbodyId;

	if (!m_IsDynamic) m_AABB = GetAABB();
}

void Collider::RegisterCollider(const size_t colliderType)
{
	const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
	ASSERT_ENTITY_SHARED_PTR(sharedEntity);
	m_Id = sharedEntity->RegisterCollider(colliderType);
}

size_t Collider::GetRigidbodyId() const
{
	ASSERT_COLLIDER_DYNAMIC(m_IsDynamic);
	return m_RigidbodyId;
}

////////////////////

void BoxCollider::OnInit()
{
	Collider::OnInit();
	RegisterCollider(typeid(BoxCollider).hash_code());
}

std::array<glm::vec2, 4> BoxCollider::GetVertices() const
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

AABB BoxCollider::GetAABB()
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

////////////////////

void CircleCollider::OnInit()
{
	Collider::OnInit();
	RegisterCollider(typeid(CircleCollider).hash_code());
}

AABB CircleCollider::GetAABB()
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