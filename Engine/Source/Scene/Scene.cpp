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
	m_Camera.Init(this);

	if (displayDebugWindow)
	{
		m_DebugWindow = std::make_unique<DebugWindow>();
		RegisterDebugFieldFloat("Frame Time, ms", &m_FrameTime, 3);
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
	m_Camera.UpdateViewProjection();

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

	if (m_DebugWindow != nullptr) m_DebugWindow->Render();
}

void Scene::DestroyEntity(const size_t id)
{
	m_Entities.at(id)->OnDestroy();
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

void Scene::RegisterEditableDebugFieldFloat(const std::string& name, float* value, float max, float min, const uint8_t numberOfFractionalDigits)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterEditableFieldFloat(name, value, max, min, numberOfFractionalDigits);
}

void Scene::RegisterDebugFieldFloat(const std::string& name, float* value, const uint8_t numberOfFractionalDigits)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterFieldFloat(name, value, numberOfFractionalDigits);
}

void Scene::RegisterEditableDebugFieldInteger(const std::string& name, int* value, int max, int min)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterEditableFieldInteger(name, value, max, min);
}

void Scene::RegisterDebugFieldInteger(const std::string& name, int* value)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterFieldInteger(name, value);
}

void Scene::RegisterCheckbox(const std::string_view& name, bool* activated)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterCheckbox(name, activated);
}

void Scene::RegisterRadioButton(const std::string_view& sectionTitle, uint8_t* activatedIndex, const std::vector<std::string_view>& labels)
{
	if (m_DebugWindow != nullptr) m_DebugWindow->RegisterRadioButton(sectionTitle, activatedIndex, labels);
}
