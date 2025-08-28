#include "Scene.h"
#include <imgui_internal.h>

#include "Component/Sprite.h"
#include "../Timer/Timer.h"

////////////////////

void Scene::Update(float deltaTime)
{
	Timer timer("scene update");

	for (auto& [id, entity] : m_Entities)
	{
		entity->Update(deltaTime);
	}

	m_Physics.Update(deltaTime);
}

void Scene::Render()
{
	// GetBackgroundDrawList() doesn't return nullptr, so no need to check for it
	m_DrawList = ImGui::GetBackgroundDrawList();
	for (auto& [id, entity] : m_Entities)
	{
		if (entity->HasComponent<Sprite>())
		{
			entity->GetComponent<Sprite>().Render(*m_DrawList);
		}

		entity->DrawDebug(m_RendererDebug);
	}
}

glm::vec2 Scene::GetScreenSize() const
{
	glm::vec2 size = glm::vec2(0, 0);
	if (ImGui::GetCurrentWindowRead() != nullptr)
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		size = glm::vec2(windowSize.x, windowSize.y);
	}
	else size = m_DefauleScreenSize;

	return size;
}