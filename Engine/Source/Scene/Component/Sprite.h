#pragma once
#include <memory>
#include <array>
#include <string_view>

#include <glm/glm.hpp>
#include <imgui.h>

#include "Component.h"
#include "Transform.h"
#include "../../Image/Image.h"
#include "../../Utility/Utility.h"

////////////////////

class Sprite : public Component
{
public:
	Sprite(Entity* entity, const std::string_view imagePath)
		: Component(entity)
	{
		Init(imagePath);
	}

	~Sprite() {}

	void Init(const std::string_view imagePath)
	{
		m_Image = std::make_shared<Image>(imagePath);
		m_HalfSize = glm::vec2(static_cast<float>(m_Image->GetWidth() / 2), static_cast<float>(m_Image->GetHeight() / 2));

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
		const PairCosSin pairCosSin = Vector::GetCosAndSinFromVector(m_TransformData->rotation);
		glm::vec2 rotated = glm::vec2(0, 0);

		for (ImVec2& position : m_ImageQuadPositions)
		{
			rotated = Vector::Rotate(glm::vec2(position.x - m_TransformData->position.x, position.y - m_TransformData->position.y), pairCosSin.cos, pairCosSin.sin);
			position.x = m_TransformData->position.x + rotated.x;
			position.y = m_TransformData->position.y + rotated.y;
		}
	}

	////////////////////

	static constexpr std::array<ImVec2, 4> m_ImageQuadUVs = { ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1) };

	std::shared_ptr<Image> m_Image;
	std::array<ImVec2, 4> m_ImageQuadPositions = { ImVec2(), ImVec2(), ImVec2(), ImVec2() };

	glm::vec2 m_HalfSize = glm::vec2(0, 0);
	std::shared_ptr<TransformData> m_TransformData = std::make_shared<TransformData>();
};