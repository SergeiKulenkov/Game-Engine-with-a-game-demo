#pragma once
#include <string_view>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

class Engine;
class Scene;
class Camera;
class Image;

////////////////////

struct Buffer
{
	VkBuffer handle = nullptr;
	VkDeviceMemory memory = nullptr;
	VkDeviceSize size = 0;
	VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
};

////////////////////

struct PushConstants
{
	glm::mat4 viewProjection = glm::mat4();
	glm::mat4 transform = glm::mat4();
};

////////////////////

struct Vertex
{
	glm::vec2 position = glm::vec2(0.f, 0.f);
	glm::vec2 textureCoord = glm::vec2(0.f, 0.f);
	//glm::vec3 colour = glm::vec3(0.f, 0.f, 0.f);
};

////////////////////

class Renderer
{
public:
	void Render(const Scene& scene);

	void RenderCircle(const glm::vec2& quadPosition, const glm::vec2& quadScale, const float quadAngle);
	void RenderRectangle(const glm::vec2& quadPosition, const glm::vec2& quadScale, const float quadAngle);

	void DrawImageQuad();

private:
	Renderer() {}

	void Init();
	void InitPipeline(VkPipeline* pipeline, VkPipelineLayout* layout, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void InitBuffers(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
	void InitDescriptors();

	void Shutdown();

	void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);

	VkShaderModule LoadShaderModule(const std::filesystem::path& path);

	void BeginScene(const Camera& camera);

	////////////////////

	static constexpr uint16_t vertexNumberForRectangle = 4;

	static inline const std::string shaderFolderPath = "../Assets/Shaders/";
	static inline const std::string rectangleVertexShaderPath = shaderFolderPath + "rectangle.vert.spirv";
	static inline const std::string rectangleFragmentShaderPath = shaderFolderPath + "rectangle.frag.spirv";
	static inline const std::string circleVertexShaderPath = shaderFolderPath + "circle.vert.spirv";
	static inline const std::string circleFragmentShaderPath = shaderFolderPath + "circle.frag.spirv";

	static constexpr std::string_view texturePath = "../Assets/testTexture.png";

	// TODO: pipeline should be a separate object with all this data
	// and so its initialization should be separated from this class
	// so like m_PipelinePrimitiveRectangle, m_PipelineTextured
	VkPipeline m_Pipeline = nullptr;
	VkPipelineLayout m_Layout = nullptr;
	VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;
	VkDescriptorSet m_DescriptorSet = nullptr;
	PushConstants m_PushConstants = {};

	VkPipeline m_PipelineCircle = nullptr;
	VkPipelineLayout m_LayoutCircle = nullptr;
	PushConstants m_PushConstantsCircle = {};

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;

	glm::vec2 m_QuadPosition = glm::vec2(0.4f, 0.6f);
	float m_QuadAngle = 0.f;
	glm::vec2 m_QuadScale = glm::vec2(1.f, 1.f); // also like a size for primitives

	std::shared_ptr<Image> m_Image;

	friend class Engine;
};