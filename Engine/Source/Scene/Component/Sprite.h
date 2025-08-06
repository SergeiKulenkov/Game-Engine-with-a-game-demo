#pragma once
#include <memory>
#include <array>
#include <string_view>

#include <glm/glm.hpp>
#include <imgui.h>

#include "Component.h"
#include "Transform.h"
#include "../../Image/Image.h"

////////////////////

class Sprite : public Component
{
public:
	Sprite(Entity* entity)
		: Component(entity)
	{}

	Sprite(Entity* entity, const std::string_view imagePath)
		: Component(entity)
	{
		Init(imagePath);
	}

	~Sprite() {}

	void Init(const std::string_view imagePath)
	{
		m_Image = std::make_shared<Image>(imagePath);
		m_Size = glm::vec2(static_cast<float>(m_Image->GetWidth()), static_cast<float>(m_Image->GetHeight()));
		m_HalfSize = glm::vec2(m_Size.x / 2.f, m_Size.y / 2.f);

		// Entity must have a Transform to render a Sprite
		assert(m_Entity->HasComponent<Transform>() && "Tranform Component is not present.");
		m_TransformData = m_Entity->GetComponent<Transform>().GetTransformData();
	}

	void Render(ImDrawList& drawList)
	{
		UpdateImageQuad();
		RotateImageQuad();

		drawList.AddImageQuad((ImTextureID)m_Image->GetDescriptorSet(), m_ImageQuadPositions[0], m_ImageQuadPositions[1], m_ImageQuadPositions[2], m_ImageQuadPositions[3],
			m_ImageQuadUVs[0], m_ImageQuadUVs[1], m_ImageQuadUVs[2], m_ImageQuadUVs[3]);
	}

protected:

private:
	void UpdateImageQuad()
	{
		m_ImageQuadPositions[0] = ImVec2(m_TransformData->position.x - m_HalfSize.x, m_TransformData->position.y - m_HalfSize.y);
		m_ImageQuadPositions[1] = ImVec2(m_TransformData->position.x + m_HalfSize.x, m_TransformData->position.y - m_HalfSize.y);
		m_ImageQuadPositions[2] = ImVec2(m_TransformData->position.x + m_HalfSize.x, m_TransformData->position.y + m_HalfSize.y);
		m_ImageQuadPositions[3] = ImVec2(m_TransformData->position.x - m_HalfSize.x, m_TransformData->position.y + m_HalfSize.y);
	}

	void RotateImageQuad()
	{
		const float length = glm::length(m_TransformData->rotation);
		const float cos = m_TransformData->rotation.x / length;
		const float sin = m_TransformData->rotation.y / length;

		for (ImVec2& position : m_ImageQuadPositions)
		{
			const ImVec2 oldPosition = position;
			position.x = m_TransformData->position.x + cos * (oldPosition.x - m_TransformData->position.x) - sin * (oldPosition.y - m_TransformData->position.y);
			position.y = m_TransformData->position.y + sin * (oldPosition.x - m_TransformData->position.x) + cos * (oldPosition.y - m_TransformData->position.y);
		}
	}

	////////////////////

	static constexpr std::array<ImVec2, 4> m_ImageQuadUVs = { ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1) };

	std::shared_ptr<Image> m_Image;
	std::array<ImVec2, 4> m_ImageQuadPositions = { ImVec2(), ImVec2(), ImVec2(), ImVec2() };

	glm::vec2 m_Size = glm::vec2(0, 0);
	glm::vec2 m_HalfSize = glm::vec2(0, 0);

	std::shared_ptr<TransformData> m_TransformData = std::make_shared<TransformData>();
};