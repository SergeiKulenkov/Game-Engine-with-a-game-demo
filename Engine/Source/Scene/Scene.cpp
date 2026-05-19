#include "Scene.h"
#include <imgui_internal.h>

#include "Component/Sprite.h"
#include "Component/Collider.h"
#include "Component/Rigidbody.h"

#include "../Input/InputManager.h"

////////////////////

Scene::~Scene()
{
	m_DrawList = nullptr;
}

void Scene::Start(bool displayDebugWindow)
{
	m_Physics.Start(GetScreenSize());

	if (displayDebugWindow)
	{
		m_DebugWindow = std::make_unique<DebugWindow>();
		RegisterDebugWindowField("Frame Time, ms", &m_FrameTime, 3);
	}
}

void Scene::Update(float deltaTime)
{
	m_Timer.Start();
	//ScopedTimer timer("update", true);

	for (auto& [id, entity] : m_Entities)
	{
		entity->Update(deltaTime);
	}

	m_Physics.Update(deltaTime);

	if (m_DebugWindow != nullptr)
	{
		if (!m_IsDebugPressed && InputManager::IsKeyDown(debugKey))
		{
			m_IsDebugPressed = true;
			m_DebugWindow->ChangeStatus();
		}
		else if (InputManager::IsKeyUp(debugKey))
		{
			m_IsDebugPressed = false;
		}
	}
}

void Scene::Render()
{
	//ScopedTimer timer("render", true);
	// GetBackgroundDrawList() doesn't return nullptr, so no need to check for it
	m_DrawList = ImGui::GetBackgroundDrawList();
	for (auto& [id, entity] : m_Entities)
	{
		if (entity->HasComponent<Sprite>())
		{
			entity->GetComponent<Sprite>()->Render(*m_DrawList);
		}

		entity->DrawDebug(m_RendererDebug);
	}

	m_FrameTime = m_Timer.ElapsedMilliseconds();
	if (m_DebugWindow != nullptr) m_DebugWindow->Render();
}

void Scene::DestroyEntity(const size_t id)
{
	if (m_Entities.at(id)->HasComponent<BoxCollider>())
	{
		m_Physics.RemoveCollider(m_Entities.at(id)->GetComponent<BoxCollider>()->GetId());
	}
	else if (m_Entities.at(id)->HasComponent<CircleCollider>())
	{
		m_Physics.RemoveCollider(m_Entities.at(id)->GetComponent<CircleCollider>()->GetId());
	}

	if (m_Entities.at(id)->HasComponent<Rigidbody>())
	{
		m_Physics.RemoveRigidbody(m_Entities.at(id)->GetComponent<Rigidbody>()->GetId());
	}

	m_Entities.erase(id);
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

void Scene::RegisterEditableDebugWindowField(const std::string& name, float* value, float max, float min, const uint8_t numberOfFractionalDigits)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterEditableField(name, value, max, min, numberOfFractionalDigits);
}

void Scene::RegisterDebugWindowField(const std::string& name, float* value, const uint8_t numberOfFractionalDigits)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterField(name, value, numberOfFractionalDigits);
}
