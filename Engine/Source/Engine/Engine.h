#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include <functional>

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "../Scene/Scene.h"
#include "../Renderer/Renderer.h"

struct GLFWwindow;

void check_vk_result(VkResult err);

ImGui_ImplVulkanH_Window* GetWindowData();

////////////////////

class Engine
{
public:
	Engine() = delete;
	Engine(std::string_view name, const uint16_t width, const uint16_t height);

	~Engine();

	Engine(const Engine& engine) = delete;
	Engine& operator=(const Engine& engine) = delete;

	void RunScene();

	void InitScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene; }

	GLFWwindow* GetWindow() { return m_Window; }

	static VkInstance GetInstance();
	static VkPhysicalDevice GetPhysicalDevice();
	static VkDevice GetDevice();
	static VkCommandBuffer GetActiveCommandBuffer();

	static VkDescriptorSet AllocateSecriptorSet(VkDescriptorSetLayout descriptorSetLayout);
	static void AllocateSecriptorSets(VkDescriptorSetLayout descriptorSetLayout, const uint32_t count, std::vector<VkDescriptorSet>& result);

	static VkCommandBuffer GetCommandBuffer(bool begin);
	static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

	static void SubmitResourceFree(std::function<void()>&& func);

private:
	void Init(std::string_view name, const uint16_t width, const uint16_t height);

	void Shutdown();

	////////////////////

	static constexpr ImVec4 clearColour = ImVec4(135 / 255.f, 138 / 255.f, 131 / 255.f, 1.00f);

	bool m_Running = false;

	float m_TimeStep = 0.0f;
	// in seconds
	float m_FrameTime = 0.0f;
	float m_EllapsedTime = 0.0f;

	GLFWwindow* m_Window = nullptr;

	std::shared_ptr<Scene> m_Scene = nullptr;

	Renderer m_Renderer;
};