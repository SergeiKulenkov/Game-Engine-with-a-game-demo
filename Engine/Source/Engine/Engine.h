#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include <functional>

#include <vulkan/vulkan.h>
#include <imgui.h>

#include "../Entity/Entity.h"

struct GLFWwindow;

void check_vk_result(VkResult err);

////////////////////

class Engine
{
public:
	Engine() = delete;
	Engine(std::string_view name);

	~Engine();

	Engine(const Engine& engine) = delete;
	Engine& operator=(const Engine& engine) = delete;

	void Run();

	void AddEntity(std::shared_ptr<Entity> object) { m_Entites.emplace_back(object); }

	GLFWwindow* GetWindow() { return m_Window; }

	ImVec2 GetWindowSize() const { return ImVec2(width, height); }

	static VkInstance GetInstance();
	static VkPhysicalDevice GetPhysicalDevice();
	static VkDevice GetDevice();

	static VkCommandBuffer GetCommandBuffer(bool begin);
	static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

	static void SubmitResourceFree(std::function<void()>&& func);

private:
	void Init(std::string_view name = "");

	void Shutdown();

	static constexpr uint16_t width = 1920;
	static constexpr uint16_t height = 1080;
	static constexpr ImVec4 clearColour = ImVec4(135 / 255.f, 138 / 255.f, 131 / 255.f, 1.00f);

	bool m_Running = false;

	float m_TimeStep = 0.0f;
	float m_FrameTime = 0.0f;
	float m_LastFrameTime = 0.0f;

	GLFWwindow* m_Window = nullptr;
	std::vector<std::shared_ptr<Entity>> m_Entites;
};