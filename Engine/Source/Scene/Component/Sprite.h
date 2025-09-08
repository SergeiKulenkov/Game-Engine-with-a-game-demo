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
	Sprite(const std::string_view imagePath)
	{
		m_Image = std::make_shared<Image>(imagePath);
		m_HalfSize = glm::vec2(static_cast<float>(m_Image->GetWidth() / 2), static_cast<float>(m_Image->GetHeight() / 2));
	}

	~Sprite() {}

	virtual void OnInit() override
	{
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		assert(sharedEntity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");
		// Entity must have a Transform to render a Sprite
		assert(sharedEntity->HasComponent<Transform>() && "Tranform Component is not present.");
		m_TransformData = sharedEntity->GetComponent<Transform>()->GetTransformData();
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
		glm::vec2 rotated = glm::vec2(0.f, 0.f);

		for (ImVec2& position : m_ImageQuadPositions)
		{
			rotated = Vector::Rotate(glm::vec2(position.x - m_TransformData->position.x, position.y - m_TransformData->position.y), pairCosSin);
			position.x = m_TransformData->position.x + rotated.x;
			position.y = m_TransformData->position.y + rotated.y;
		}
	}

	////////////////////

	static constexpr std::array<ImVec2, 4> m_ImageQuadUVs = { ImVec2(0.f, 0.f), ImVec2(1.f, 0.f), ImVec2(1.f, 1.f), ImVec2(0.f, 1.f) };

	std::shared_ptr<Image> m_Image;
	std::array<ImVec2, 4> m_ImageQuadPositions = { ImVec2(0.f, 0.f), ImVec2(0.f, 0.f), ImVec2(0.f, 0.f), ImVec2(0.f, 0.f) };

	glm::vec2 m_HalfSize = glm::vec2(0.f, 0.f);
	std::shared_ptr<TransformData> m_TransformData = std::make_shared<TransformData>();
};