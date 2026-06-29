#pragma once
#include <string_view>
#include <string>
#include <filesystem>
#include <vector>

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

class Engine;
class Scene;

////////////////////

struct Buffer
{
	VkBuffer Handle = nullptr;
	VkDeviceMemory Memory = nullptr;
	VkDeviceSize Size = 0;
	VkBufferUsageFlagBits Usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
};

////////////////////

struct PushConstants
{
	//glm::mat4 ViewProjection = glm::mat4();
	//glm::mat4 Transform = glm::mat4();
	glm::vec2 Transform = glm::vec2();
};

////////////////////

class Renderer
{
public:
	void Render(const Scene& scene);

	void RenderTriangle();
	void RenderRectangle();

	// move drawing primitives and images here
	// leave UI windows for imgui
	void DrawImageQuad();

private:
	Renderer() {}

	void Init();
	void InitPipeline();
	void InitBuffers(const std::vector<glm::vec2>& vertices, const std::vector<uint16_t>& indices);

	void Shutdown();

	void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);

	VkShaderModule LoadShaderModule(const std::filesystem::path& path);

	////////////////////

	static inline const std::string shaderFolderPath = "../Assets/Shaders/";
	static inline const std::string vertexShaderPath = shaderFolderPath + "basic.vert.spirv";
	static inline const std::string fragmentShaderPath = shaderFolderPath + "basic.frag.spirv";

	VkPipeline m_Pipeline = nullptr;
	VkPipelineLayout m_Layout = nullptr;

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	PushConstants m_PushConstants = {};

	friend class Engine;
};