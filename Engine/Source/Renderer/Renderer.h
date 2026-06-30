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
	glm::mat4 ViewProjection = glm::mat4();
	glm::mat4 Transform = glm::mat4();
};

////////////////////

class Renderer
{
public:
	void Render(const Scene& scene);

	void RenderCircle();
	void RenderRectangle();

	// move drawing primitives and images here
	// leave UI windows for imgui
	void DrawImageQuad();

private:
	Renderer() {}

	void Init();
	void InitPipeline(VkPipeline* pipeline, VkPipelineLayout* layout, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void InitBuffers(const std::vector<glm::vec2>& vertices, const std::vector<uint16_t>& indices);

	void Shutdown();

	void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);

	VkShaderModule LoadShaderModule(const std::filesystem::path& path);

	////////////////////

	static inline const std::string shaderFolderPath = "../Assets/Shaders/";
	static inline const std::string rectangleVertexShaderPath = shaderFolderPath + "rectangle.vert.spirv";
	static inline const std::string rectangleFragmentShaderPath = shaderFolderPath + "rectangle.frag.spirv";
	static inline const std::string circleVertexShaderPath = shaderFolderPath + "circle.vert.spirv";
	static inline const std::string circleFragmentShaderPath = shaderFolderPath + "circle.frag.spirv";

	VkPipeline m_Pipeline = nullptr;
	VkPipelineLayout m_Layout = nullptr;

	VkPipeline m_PipelineCircle = nullptr;
	VkPipelineLayout m_LayoutCircle = nullptr;

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	PushConstants m_PushConstants = {};
	PushConstants m_PushConstantsCircle = {};

	glm::vec2 m_QuadPosition = glm::vec2(-0.5f, 0.6f);
	glm::vec2 m_Rotation = glm::vec2(0.f, 0.f);
	glm::vec3 m_CameraPosition = glm::vec3(0, 0, 3.f);

	friend class Engine;
};