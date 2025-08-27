#pragma once
#include <imgui.h>
#include <glm/glm.hpp>

#include "Entity.h"
#include "Physics.h"

////////////////////

struct Colour
{
	static constexpr uint32_t white = 4294967295;
	static constexpr uint32_t black = 4278190080;
	static constexpr uint32_t red = 4278190335;
	static constexpr uint32_t blue = 4294901760;
	static constexpr uint32_t green = 4278255360;
	static constexpr uint32_t yellow = 4278255615;
	static constexpr uint32_t purple = 4294901960;
	static constexpr uint32_t orange = 4278226175;
	static constexpr uint32_t pink = 4289331455;
	static constexpr uint32_t lime = 4278255510;
};

////////////////////

struct RendererDebug
{
	void DrawLine(const glm::vec2& start, const glm::vec2& end, const uint32_t colour, const float thickness = 1.0f) const
	{
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), colour, thickness);
	}

	void DrawRectangle(const glm::vec2& upperLeftPosition, const glm::vec2& lowerRightPosition, const uint32_t colour, const float thickness = 1.0f) const
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(upperLeftPosition.x, upperLeftPosition.y), ImVec2(lowerRightPosition.x, lowerRightPosition.y), colour, 0.f, 0, thickness);
	}

	void DrawCircle(const glm::vec2& center, const float radius, const uint32_t colour, const int numberOfSegments = 0, const float thickness = 1.0f) const
	{
		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(center.x, center.y), radius, colour, numberOfSegments, thickness);
	}
};

////////////////////

class Scene
{
public:
	Scene() {}

	~Scene()
	{
		m_DrawList = nullptr;
	}

	void Clear() { m_Entities.clear(); }

	void Update(float deltaTime);

	void Render();

	// the returned pointer should be used to set up components
	// to store the newly created Entity it should be converted to a weak_ptr
	template<typename T>
	std::shared_ptr<Entity> CreateEntity()
	{
		static_assert(std::is_base_of_v<Entity, T>, "T must be of type Entity.");

		const size_t id = m_Entities.size() + 1;
		m_Entities.emplace(id, std::make_shared<T>());
		m_Entities[id]->Init(id, this);

		return m_Entities[id];
	}

	void RemoveEntity(const size_t id) { m_Entities.erase(id); }

	void RegisterCollider(Collider& collider) { m_Physics.AddCollider(collider); }

	glm::vec2 GetScreenSize() const;

	void SetDefaultScreenSize(const glm::vec2& size) { m_DefauleScreenSize = size; }

private:
	ImDrawList* m_DrawList = nullptr;

	RendererDebug m_RendererDebug;

	Physics m_Physics;

	glm::vec2 m_DefauleScreenSize = glm::vec2(0 , 0);

	std::unordered_map<size_t, std::shared_ptr<Entity>> m_Entities;
};