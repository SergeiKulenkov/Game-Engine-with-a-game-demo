#pragma once
#include <string_view>
#include <string>
#include <filesystem>
#include <vector>
#include <array>
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

struct Vertex
{
	glm::vec2 position = glm::vec2(0.f, 0.f);
	glm::vec2 textureCoord = glm::vec2(0.f, 0.f);
};

////////////////////

struct VertexCircle
{
	glm::vec2 position = glm::vec2(0.f, 0.f);
	float thickness = 0.5f;
	glm::vec4 colour = glm::vec4(0.f, 0.f, 0.f, 1.0f);
};

////////////////////

enum class ObjectType
{
	TEXTURED,
	PRIMITIVE_CIRCLE,
	PRIMITIVE_RECTANGLE,
};

////////////////////

class Renderer
{
public:
	void Render(const Scene& scene);

	void RenderCircle(const glm::vec2& quadPosition, const glm::vec2& quadScale, const glm::vec4 colour);
	void RenderCircle(const glm::vec2& quadPosition, const glm::vec2& quadScale, const std::array<glm::vec4, 4>& colours);

	void RenderRectangle(const glm::vec2& quadPosition, const glm::vec2& quadScale, const float quadAngle);

	//void DrawImageQuad();

private:
	Renderer() {}

	void Init();
	void InitPipeline(VkPipeline* pipeline, VkPipelineLayout* layout, const ObjectType type, const uint16_t count, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void InitBuffers(Buffer& vertexBuffer, VertexCircle* vertices, VkDeviceSize vertexMemory, Buffer& indexBuffer, uint16_t* indices, VkDeviceSize indeexMemory);
	void InitDescriptors();

	void Shutdown();

	void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);

	VkShaderModule LoadShaderModule(const std::filesystem::path& path);

	void BeginScene(const Camera& camera);
	void EndScene();

	////////////////////

	static constexpr uint16_t vertexNumberForRectangle = 4;
	static constexpr uint16_t maxCircles = 1000;
	static constexpr uint16_t maxIndices = maxCircles * 6;

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
	Buffer m_VertexBuffer;
	std::array<Buffer, 2> m_IndexBuffer;

	VkPipeline m_PipelineCircle = nullptr;
	VkPipelineLayout m_LayoutCircle = nullptr;
	std::array<Buffer, 2> m_VertexBufferCircle;

	glm::vec2 m_QuadPosition = glm::vec2(-0.3f, -0.7f);
	float m_QuadAngle = 0.f;
	glm::vec2 m_QuadScale = glm::vec2(1.f, 1.f); // also like a size for primitives

	std::shared_ptr<Image> m_Image;

	VertexCircle* m_VerticesCirclePtr = nullptr;
	VertexCircle* m_VerticesCircleBase = nullptr;
	uint16_t* m_Indices = nullptr;
	uint16_t m_CirclesVertexCount = 0;
	uint16_t m_CirclesIndexCount = 0;

	friend class Engine;
};