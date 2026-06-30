#include "Renderer.h"

#include <array>
#include <fstream>
#include <imgui_impl_vulkan.h>
#include "glm/gtc/matrix_transform.hpp"

#include "../Scene/Scene.h"
#include "../Engine/Engine.h"
#include "../Utility/Utility.h"

////////////////////

static uint32_t GetMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
{
	uint32_t type = 0xFFFFFFFF;
	VkPhysicalDevice physicalDevice = Engine::GetPhysicalDevice();

	VkPhysicalDeviceMemoryProperties prop;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &prop);
	for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
	{
		if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
		{
			type = i;
			break;
		}
	}

	return type;
}

////////////////////

void Renderer::Init()
{
	// this can be done with a pipeline builder
	InitPipeline(&m_Pipeline, &m_Layout, rectangleVertexShaderPath, rectangleFragmentShaderPath);
	InitPipeline(&m_PipelineCircle, &m_LayoutCircle, circleVertexShaderPath, circleFragmentShaderPath);

	// TODO: device lost error when creating buffers every frame
	const std::vector<glm::vec2> vertices = {
			glm::vec2(-0.5f, -0.5f),
			glm::vec2(-0.5f,  0.5f),
			glm::vec2( 0.5f,  0.5f),
			glm::vec2( 0.5f, -0.5f),
	};
	const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
	InitBuffers(vertices, indices);
}

void Renderer::InitPipeline(VkPipeline* pipeline, VkPipelineLayout* layout, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	VkDevice device = Engine::GetDevice();
	VkRenderPass renderPass = GetWindowData()->RenderPass;

	VkPushConstantRange pushConstantRange;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstants);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layout_info.pPushConstantRanges = &pushConstantRange;
	layout_info.pushConstantRangeCount = 1;
	check_vk_result(vkCreatePipelineLayout(device, &layout_info, nullptr, layout));

	VkPipelineInputAssemblyStateCreateInfo input_assembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkVertexInputBindingDescription binding_description;
	binding_description.binding = 0;
	binding_description.stride = sizeof(glm::vec2);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attribute_descriptions;
	attribute_descriptions.location = 0;
	attribute_descriptions.binding = binding_description.binding;
	attribute_descriptions.format = VK_FORMAT_R32G32_SFLOAT;
	attribute_descriptions.offset = 0;

	VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	vertex_input.vertexBindingDescriptionCount = 1;
	vertex_input.pVertexBindingDescriptions = &binding_description;
	vertex_input.vertexAttributeDescriptionCount = 1;
	vertex_input.pVertexAttributeDescriptions = &attribute_descriptions;

	VkPipelineRasterizationStateCreateInfo raster{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	raster.cullMode = VK_CULL_MODE_BACK_BIT;
	raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
	raster.lineWidth = 1.0f;

	VkPipelineColorBlendAttachmentState blend_attachment{};
	blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo blend{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	blend.attachmentCount = 1;
	blend.pAttachments = &blend_attachment;

	VkPipelineViewportStateCreateInfo viewport{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewport.viewportCount = 1;
	viewport.scissorCount = 1;

	VkPipelineDepthStencilStateCreateInfo depth_stencil{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

	VkPipelineMultisampleStateCreateInfo multisample{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	std::array<VkDynamicState, 2> dynamics{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamic{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamic.dynamicStateCount = static_cast<uint32_t>(dynamics.size()),
	dynamic.pDynamicStates = dynamics.data();

	std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};
	shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shader_stages[0].module = LoadShaderModule(vertexShaderPath);
	shader_stages[0].pName = "main";

	shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shader_stages[1].module = LoadShaderModule(fragmentShaderPath);
	shader_stages[1].pName = "main";

	VkGraphicsPipelineCreateInfo pipe{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipe.stageCount = static_cast<uint32_t>(shader_stages.size());
	pipe.pStages = shader_stages.data();
	pipe.pVertexInputState = &vertex_input;
	pipe.pInputAssemblyState = &input_assembly;
	pipe.pViewportState = &viewport;
	pipe.pRasterizationState = &raster;
	pipe.pMultisampleState = &multisample;
	pipe.pDepthStencilState = &depth_stencil;
	pipe.pColorBlendState = &blend;
	pipe.pDynamicState = &dynamic;
	pipe.layout = *layout;
	pipe.renderPass = renderPass;

	check_vk_result(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, pipeline));
	vkDestroyShaderModule(device, shader_stages[0].module, nullptr);
	vkDestroyShaderModule(device, shader_stages[1].module, nullptr);
}

void Renderer::InitBuffers(const std::vector<glm::vec2>& vertices, const std::vector<uint16_t>& indices)
{
	VkDevice device = Engine::GetDevice();
	uint64_t verticesMemory = sizeof(glm::vec2) * vertices.size();
	uint64_t indicesMemory = sizeof(uint16_t) * indices.size();

	m_VertexBuffer.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	CreateOrResizeBuffer(m_VertexBuffer, verticesMemory);

	m_IndexBuffer.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	CreateOrResizeBuffer(m_IndexBuffer, indicesMemory);

	glm::vec2* vbMemory;
	check_vk_result(vkMapMemory(device, m_VertexBuffer.Memory, 0, verticesMemory, 0, (void**)&vbMemory));
	memcpy(vbMemory, vertices.data(), static_cast<size_t>(verticesMemory));

	uint16_t* ibMemory;
	check_vk_result(vkMapMemory(device, m_IndexBuffer.Memory, 0, indicesMemory, 0, (void**)&ibMemory));
	memcpy(ibMemory, indices.data(), static_cast<size_t>(indicesMemory));

	VkMappedMemoryRange range[2] = {};
	range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range[0].memory = m_VertexBuffer.Memory;
	range[0].size = VK_WHOLE_SIZE;
	range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range[1].memory = m_IndexBuffer.Memory;
	range[1].size = VK_WHOLE_SIZE;

	check_vk_result(vkFlushMappedMemoryRanges(device, 2, range));
	vkUnmapMemory(device, m_VertexBuffer.Memory);
	vkUnmapMemory(device, m_IndexBuffer.Memory);
}

void Renderer::Shutdown()
{
	VkDevice device = Engine::GetDevice();
	vkDestroyPipeline(device, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(device, m_Layout, nullptr);

	vkDestroyPipeline(device, m_PipelineCircle, nullptr);
	vkDestroyPipelineLayout(device, m_LayoutCircle, nullptr);

	vkDestroyBuffer(device, m_VertexBuffer.Handle, nullptr);
	vkFreeMemory(device, m_VertexBuffer.Memory, nullptr);

	vkDestroyBuffer(device, m_IndexBuffer.Handle, nullptr);
	vkFreeMemory(device, m_IndexBuffer.Memory, nullptr);
}

void Renderer::CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize)
{
	VkDevice device = Engine::GetDevice();

	if (buffer.Handle != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, buffer.Handle, nullptr);
	}
	if (buffer.Handle != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, buffer.Memory, nullptr);
	}

	VkBufferCreateInfo bufferCI = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferCI.size = newSize;
	bufferCI.usage = buffer.Usage;
	bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	check_vk_result(vkCreateBuffer(device, &bufferCI, nullptr, &buffer.Handle));

	VkMemoryRequirements req;
	vkGetBufferMemoryRequirements(device, buffer.Handle, &req);

	VkMemoryAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	alloc_info.allocationSize = req.size;
	alloc_info.memoryTypeIndex = GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
	check_vk_result(vkAllocateMemory(device, &alloc_info, nullptr, &buffer.Memory));

	check_vk_result(vkBindBufferMemory(device, buffer.Handle, buffer.Memory, 0));
	buffer.Size = req.size;
}

VkShaderModule Renderer::LoadShaderModule(const std::filesystem::path& path)
{
	VkShaderModule shaderModule = nullptr;
	std::ifstream stream(path, std::ios::binary);
	if (stream)
	{
		stream.seekg(0, std::ios_base::end);
		std::streampos size = stream.tellg();
		stream.seekg(0, std::ios_base::beg);

		std::vector<char> buffer(size);
		if (!stream.read(buffer.data(), size))
		{
			Debug::Log("Could not read file! {}", path.string());
		}

		stream.close();

		VkShaderModuleCreateInfo shaderModuleCI{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		shaderModuleCI.pCode = (uint32_t*)buffer.data();
		shaderModuleCI.codeSize = buffer.size();

		check_vk_result(vkCreateShaderModule(Engine::GetDevice(), &shaderModuleCI, nullptr, &shaderModule));
	}
	else Debug::Log("Could not open file! {}", path.string());

	return shaderModule;
}

void Renderer::Render(const Scene& scene)
{
	// seems like imgui maps all objects into one buffer
	// in ImGui_ImplVulkan_RenderDrawData():
	// align buffer size for both vertex and index buffers
	// Create of resize for both
	// map memory, memcpy using the draw lists, flush, unmap
	// then ImGui_ImplVulkan_SetupRenderState() does vk cmd bind buffers and vk cmd set viewport
	// and vk cmd push constants and vk cmd bind descriptor sets
	// then a loop through all draw lists with vk cmd set scissor and finally vk cmd draw indexed

	RenderRectangle();
	RenderCircle();

	// get entities from Scene? or get Sprites from Scene?
	// what about drawing debug primitives? then get all entities to pass this* to them?
	// for rendering image quads just need some data from a Sprite
	// 
}

void Renderer::RenderCircle()
{
	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	auto windowData = GetWindowData();

	float viewportWidth = static_cast<float>(windowData->Width);
	float viewportHeight = static_cast<float>(windowData->Height);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineCircle);

	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.Handle, &offset);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Handle, offset, VK_INDEX_TYPE_UINT16);

	vkCmdPushConstants(commandBuffer, m_LayoutCircle, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstantsCircle);

	VkViewport vp{};
	vp.y = viewportHeight;
	vp.width = viewportWidth;
	vp.height = -viewportHeight;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &vp);

	VkRect2D scissor{};
	scissor.extent.width = windowData->Width;
	scissor.extent.height = windowData->Height;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}

void Renderer::RenderRectangle()
{
	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	auto windowData = GetWindowData();

	float viewportWidth = static_cast<float>(windowData->Width);
	float viewportHeight = static_cast<float>(windowData->Height);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.Handle, &offset);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Handle, offset, VK_INDEX_TYPE_UINT16);

	glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), m_CameraPosition);
	m_PushConstants.ViewProjection = glm::perspectiveFov(glm::radians(45.0f), viewportWidth, viewportHeight, 0.1f, 1000.0f)
		* glm::inverse(cameraTransform);

	m_QuadPosition.x += 0.005f;
	m_PushConstants.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_QuadPosition, 0.f));
	//m_PushConstants.Transform = glm::translate(glm::mat4(1.0f), m_CubePosition)
	//	* glm::eulerAngleXYZ(glm::radians(m_CubeRotation.x), glm::radians(m_CubeRotation.y), glm::radians(m_CubeRotation.z));
	// multiply by scale

	vkCmdPushConstants(commandBuffer, m_Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstants);

	VkViewport vp{};
	vp.y = viewportHeight;
	vp.width = viewportWidth;
	vp.height = -viewportHeight;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &vp);

	VkRect2D scissor{};
	scissor.extent.width = windowData->Width;
	scissor.extent.height = windowData->Height;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}
