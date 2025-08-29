#include "Scene.h"
#include <imgui_internal.h>

#include "Component/Sprite.h"
#include "../Input/InputManager.h"

////////////////////

void Scene::Start(bool displayDebugWindow)
{
	if (displayDebugWindow)
	{
		m_DebugWindow = std::make_unique<DebugWindow>();
		RegisterDebugWindowField("Frame Time, ms", &m_FrameTime, false, 3);
	}
}

void Scene::Update(float deltaTime)
{
	m_Timer.Start();
	ScopedTimer timer("update", true);

	for (auto& [id, entity] : m_Entities)
	{
		entity->Update(deltaTime);
	}

	m_Physics.Update(deltaTime);

	if (m_DebugWindow)
	{
		if (!m_IsTabPressed && InputManager::IsKeyDown(KeyCode::Tab))
		{
			m_IsTabPressed = true;
			m_DebugWindow->ChangeStatus();
		}
		else if (InputManager::IsKeyUp(KeyCode::Tab))
		{
			m_IsTabPressed = false;
		}
	}
}

void Scene::Render()
{
	ScopedTimer timer("render", true);
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

	m_FrameTime = m_Timer.ElapsedMilliseconds();
	if (m_DebugWindow) m_DebugWindow->Render();
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

void Scene::RegisterDebugWindowField(const std::string& name, float* value, bool editable, const uint8_t numberOfFractionalDigits)
{
	if (m_DebugWindow) m_DebugWindow->RegisterField(name, value, editable, numberOfFractionalDigits);
}
