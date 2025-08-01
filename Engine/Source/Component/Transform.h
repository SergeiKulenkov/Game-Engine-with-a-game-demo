#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "Component.h"

////////////////////

struct TransformData
{
	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 rotation = glm::vec2(1, 0);
	glm::vec2 scale = glm::vec2(1, 1);
};

////////////////////

class Transform : public Component
{
public:
	Transform(Entity* entity)
		: Component(entity)
	{}

	Transform(Entity* entity, const glm::vec2& position)
		: Component(entity)
	{
		m_TransformData->position = position;
	}

	Transform(Entity* entity, const glm::vec2& position, const glm::vec2& rotation)
		: Component(entity)
	{
		m_TransformData->position = position;
		m_TransformData->position = position;
	}

	Transform(Entity* entity, const TransformData& transformData)
		: Component(entity), m_TransformData(std::make_shared<TransformData>(transformData))
	{}

	Transform(Entity* entity, const Transform& transform)
		: Component(entity), m_TransformData(std::make_shared<TransformData>(transform.m_TransformData))
	{}

	virtual ~Transform() override {}

	TransformData GetTransformData() const { return *m_TransformData.get(); }
	std::shared_ptr<TransformData> GetTransformData() { return m_TransformData; }
	void SetTransformData(const TransformData& transformData) { m_TransformData = std::make_shared<TransformData>(transformData); }

	glm::vec2 GetPosition() const { return m_TransformData->position; }
	glm::vec2& GetPosition() { return m_TransformData->position; }
	void SetPosition(const glm::vec2& position) { m_TransformData->position = position; }

	glm::vec2 GetRotation() const { return m_TransformData->rotation; }
	glm::vec2& GetRotation() { return m_TransformData->rotation; }
	void SetRotation(const glm::vec2& rotation) { m_TransformData->rotation = rotation; }

	glm::vec2 GetScale() const { return m_TransformData->scale; }
	glm::vec2& GetScale() { return m_TransformData->scale; }
	void SetScale(const glm::vec2& scale) { m_TransformData->scale = scale; }

private:
	std::shared_ptr<TransformData> m_TransformData = std::make_shared<TransformData>();
};