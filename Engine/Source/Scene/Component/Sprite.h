#pragma once
#include <memory>
#include <array>
#include <string_view>

#include <glm/glm.hpp>
#include <imgui.h>

#include "Component.h"
#include "Transform.h"
#include "../../Renderer/Image.h"
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

	void Render(ImDrawList& drawList)
	{
		UpdateImageQuad();
		RotateImageQuad();

		// TODO: pass to the Renderer
		// through a Renderer* which can be passed here instead of the drawList??
		// but the Scene probably doesn't need the Renderer
		// or the Scene can pass Sprites to the Renderer which sounds less performant
		drawList.AddImageQuad((ImTextureID)m_Image->GetDescriptorSet(), m_ImageQuadPositions[0], m_ImageQuadPositions[1], m_ImageQuadPositions[2], m_ImageQuadPositions[3],
			m_ImageQuadUVs[0], m_ImageQuadUVs[1], m_ImageQuadUVs[2], m_ImageQuadUVs[3]);
	}

	glm::vec2 GetSize() const { return m_HalfSize * 2.f; }

protected:
	virtual void OnInit() override
	{
		// Entity must have a Transform to render a Sprite
		ASSERT_ENTITY_NULLPTR(m_Entity);
		ASSERRT_HAS_TRANSFORM(m_Entity->HasComponent<Transform>());
		m_TransformData = m_Entity->GetComponent<Transform>()->GetTransformData();
	}

	virtual void OnRemove() {}

private:
	void UpdateImageQuad()
	{
		const std::shared_ptr<TransformData> tranform = m_TransformData.lock();
		ASSERT_TRANSFORM_SHARED_PTR(tranform);
		m_ImageQuadPositions[0] = ImVec2(tranform->position.x - m_HalfSize.x, tranform->position.y - m_HalfSize.y);
		m_ImageQuadPositions[1] = ImVec2(tranform->position.x + m_HalfSize.x, tranform->position.y - m_HalfSize.y);
		m_ImageQuadPositions[2] = ImVec2(tranform->position.x + m_HalfSize.x, tranform->position.y + m_HalfSize.y);
		m_ImageQuadPositions[3] = ImVec2(tranform->position.x - m_HalfSize.x, tranform->position.y + m_HalfSize.y);
	}

	void RotateImageQuad()
	{
		const std::shared_ptr<TransformData> tranform = m_TransformData.lock();
		ASSERT_TRANSFORM_SHARED_PTR(tranform);
		const PairCosSin pairCosSin = Vector::GetCosAndSinFromVector(tranform->rotation);
		glm::vec2 rotated = glm::vec2(0.f, 0.f);

		for (ImVec2& position : m_ImageQuadPositions)
		{
			rotated = Vector::Rotate(glm::vec2(position.x - tranform->position.x, position.y - tranform->position.y), pairCosSin);
			position.x = tranform->position.x + rotated.x;
			position.y = tranform->position.y + rotated.y;
		}
	}

	////////////////////

	static constexpr std::array<ImVec2, 4> m_ImageQuadUVs = { ImVec2(0.f, 0.f), ImVec2(1.f, 0.f), ImVec2(1.f, 1.f), ImVec2(0.f, 1.f) };

	std::shared_ptr<Image> m_Image;
	std::array<ImVec2, 4> m_ImageQuadPositions = { ImVec2(0.f, 0.f), ImVec2(0.f, 0.f), ImVec2(0.f, 0.f), ImVec2(0.f, 0.f) };

	glm::vec2 m_HalfSize = glm::vec2(0.f, 0.f);
	std::weak_ptr<TransformData> m_TransformData;
};