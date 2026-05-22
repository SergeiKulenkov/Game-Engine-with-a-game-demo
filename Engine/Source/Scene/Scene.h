#pragma once
#include <imgui.h>
#include <glm/glm.hpp>

#include "Entity.h"
#include "Physics.h"
#include "../Input/KeyCodes.h"
#include "../Utility/Timer.h"
#include "../Utility/DebugWindow.h"

class Collider;
class Rigidbody;
class Engine;

template<typename T>
concept EntityType = std::is_base_of_v<Entity, T>;

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

class Scene : public std::enable_shared_from_this<Scene>
{
public:
	virtual ~Scene();

	// the returned pointer should be used to set up components
	// to store the newly created Entity it should be converted to a weak_ptr
	template<EntityType T>
	std::shared_ptr<Entity> CreateEntity()
	{
		const size_t id = m_Entities.size();
		m_Entities.emplace(id, std::make_shared<T>());
		m_Entities.at(id)->Init(id, shared_from_this());

		return m_Entities.at(id);
	}

	// an Entity should be deleted by using this method
	// because Scene owns entities, deleting them some other way won't work
	void DestroyEntity(const size_t id);

	bool Raycast(const Ray& ray, const std::shared_ptr<RaycastHit>& hitResult) { return m_Physics.Raycast(ray, hitResult); }
	bool Raycast(const glm::vec2& origin, const glm::vec2& direction, const float length, const std::shared_ptr<RaycastHit>& hitResult) { return m_Physics.Raycast(origin, direction, length, hitResult); }

	glm::vec2 GetScreenSize() const;

	void RegisterEditableDebugWindowField(const std::string& name, float* value, float max = 10.f, float min = 0.f, const uint8_t numberOfFractionalDigits = 1);
	void RegisterDebugWindowField(const std::string& name, float* value, const uint8_t numberOfFractionalDigits = 1);

protected:
	Scene() {}

	void Start(bool displayDebugWindow = false);

	void Clear() { m_Entities.clear(); }

	void SetFrameTime(const float frameTime) { m_FrameTime = frameTime; }

	void Update(float deltaTime);

	void Render();

	void SetDefaultScreenSize(const glm::vec2& size) { m_DefauleScreenSize = size; }

	size_t RegisterCollider(const std::shared_ptr<Collider>& collider) { return m_Physics.AddCollider(collider); }
	void UnRegisterCollider(const size_t id) { m_Physics.RemoveCollider(id); }

	size_t RegisterRigidbody(const std::shared_ptr<Rigidbody>& rigidbody) { return m_Physics.AddRigidbody(rigidbody); }
	void UnRegisterRigidbody(const size_t id) { m_Physics.RemoveRigidbody(id); }

private:
	static constexpr KeyCode debugKey = KeyCode::GraveAccent;

	bool m_IsDebugPressed = false;
	RendererDebug m_RendererDebug;
	float m_FrameTime = 0.f;

	ImDrawList* m_DrawList = nullptr;

	Physics m_Physics;

	std::unique_ptr<DebugWindow> m_DebugWindow = nullptr;

	Timer m_Timer;

	glm::vec2 m_DefauleScreenSize = glm::vec2(0.f , 0.f);

	std::unordered_map<size_t, std::shared_ptr<Entity>> m_Entities;

	friend class Engine;
	friend class Entity;
};