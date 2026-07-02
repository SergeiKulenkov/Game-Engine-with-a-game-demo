#include "Renderer.h"

#include <array>
#include <fstream>
#include <imgui_impl_vulkan.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "../Scene/Scene.h"
#include "../Engine/Engine.h"
#include "../Utility/Utility.h"
#include "Camera.h"
#include "Image.h"

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
	m_Image = std::make_shared<Image>(texturePath);

	//  TODO: implement a pipeline builder for this
	InitDescriptors();
	InitPipeline(&m_Pipeline, &m_Layout, rectangleVertexShaderPath, rectangleFragmentShaderPath);
	InitPipeline(&m_PipelineCircle, &m_LayoutCircle, circleVertexShaderPath, circleFragmentShaderPath);

	{
		Vertex newVertex;
		std::vector<Vertex> vertices;
		vertices.reserve(vertexNumberForRectangle);

		newVertex.position = glm::vec2(-0.5f, -0.5f);
		newVertex.textureCoord = glm::vec2(0.0f, 0.0f);
		//newVertex.colour = glm::vec3(161.f / 255.f, 80.f / 255.f, 230.f / 255.f);
		vertices.emplace_back(newVertex);

		newVertex.position = glm::vec2(-0.5f, 0.5f);
		newVertex.textureCoord = glm::vec2(0.0f, 0.0f);
		//newVertex.colour = glm::vec3(252.f / 255.f, 195.f / 255.f, 40.f / 255.f);
		vertices.emplace_back(newVertex);

		newVertex.position = glm::vec2(0.5f, 0.5f);
		newVertex.textureCoord = glm::vec2(0.0f, 0.0f);
		//newVertex.colour = glm::vec3(45.f / 255.f, 115.f / 255.f, 225.f / 255.f);
		vertices.emplace_back(newVertex);

		newVertex.position = glm::vec2(0.5f, -0.5f);
		newVertex.textureCoord = glm::vec2(0.0f, 0.0f);
		//newVertex.colour = glm::vec3(0.f / 255.f, 255.f / 255.f, 0.f / 255.f);
		vertices.emplace_back(newVertex);

		const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
		InitBuffers(vertices, indices);
	}
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
	layout_info.setLayoutCount = 1;
	layout_info.pSetLayouts = &m_DescriptorSetLayout;
	check_vk_result(vkCreatePipelineLayout(device, &layout_info, nullptr, layout));

	VkPipelineInputAssemblyStateCreateInfo input_assembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkVertexInputBindingDescription binding_description;
	binding_description.binding = 0;
	binding_description.stride = sizeof(Vertex);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions;
	attribute_descriptions[0].location = 0;
	attribute_descriptions[0].binding = binding_description.binding;
	attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_descriptions[0].offset = 0;
	
	attribute_descriptions[1].location = 1;
	attribute_descriptions[1].binding = binding_description.binding;
	attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[1].offset = static_cast<uint32_t>(offsetof(Vertex, textureCoord));

	VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	vertex_input.vertexBindingDescriptionCount = 1;
	vertex_input.pVertexBindingDescriptions = &binding_description;
	vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
	vertex_input.pVertexAttributeDescriptions = attribute_descriptions.data();

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

void Renderer::InitBuffers(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
	VkDevice device = Engine::GetDevice();
	uint64_t verticesMemory = sizeof(Vertex) * vertices.size();
	uint64_t indicesMemory = sizeof(uint16_t) * indices.size();

	m_VertexBuffer.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	CreateOrResizeBuffer(m_VertexBuffer, verticesMemory);

	m_IndexBuffer.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	CreateOrResizeBuffer(m_IndexBuffer, indicesMemory);

	Vertex* vbMemory;
	check_vk_result(vkMapMemory(device, m_VertexBuffer.memory, 0, verticesMemory, 0, (void**)&vbMemory));
	memcpy(vbMemory, vertices.data(), static_cast<size_t>(verticesMemory));

	uint16_t* ibMemory;
	check_vk_result(vkMapMemory(device, m_IndexBuffer.memory, 0, indicesMemory, 0, (void**)&ibMemory));
	memcpy(ibMemory, indices.data(), static_cast<size_t>(indicesMemory));

	VkMappedMemoryRange range[2] = {};
	range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range[0].memory = m_VertexBuffer.memory;
	range[0].size = VK_WHOLE_SIZE;
	range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range[1].memory = m_IndexBuffer.memory;
	range[1].size = VK_WHOLE_SIZE;

	check_vk_result(vkFlushMappedMemoryRanges(device, 2, range));
	vkUnmapMemory(device, m_VertexBuffer.memory);
	vkUnmapMemory(device, m_IndexBuffer.memory);
}

void Renderer::InitDescriptors()
{
	VkDevice device = Engine::GetDevice();

	VkDescriptorSetLayoutBinding binding = {};
	binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	info.bindingCount = 1;
	info.pBindings = &binding;
	check_vk_result(vkCreateDescriptorSetLayout(device, &info, nullptr, &m_DescriptorSetLayout));
	m_DescriptorSet = Engine::AllocateSecriptorSet(m_DescriptorSetLayout);
	
	VkWriteDescriptorSet writeDS = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	writeDS.descriptorCount = 1;
	writeDS.dstSet = m_DescriptorSet;
	writeDS.descriptorType = binding.descriptorType;
	writeDS.dstBinding = 0;
	writeDS.pImageInfo = &m_Image->GetImageInfo();

	vkUpdateDescriptorSets(device, 1, &writeDS, 0, nullptr);
}

void Renderer::Shutdown()
{
	m_Image.reset();

	VkDevice device = Engine::GetDevice();
	vkDestroyPipeline(device, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(device, m_Layout, nullptr);

	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

	vkDestroyPipeline(device, m_PipelineCircle, nullptr);
	vkDestroyPipelineLayout(device, m_LayoutCircle, nullptr);

	vkDestroyBuffer(device, m_VertexBuffer.handle, nullptr);
	vkFreeMemory(device, m_VertexBuffer.memory, nullptr);

	vkDestroyBuffer(device, m_IndexBuffer.handle, nullptr);
	vkFreeMemory(device, m_IndexBuffer.memory, nullptr);
}

void Renderer::CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize)
{
	VkDevice device = Engine::GetDevice();

	if (buffer.handle != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, buffer.handle, nullptr);
	}
	if (buffer.handle != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, buffer.memory, nullptr);
	}

	VkBufferCreateInfo bufferCI = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferCI.size = newSize;
	bufferCI.usage = buffer.usage;
	bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	check_vk_result(vkCreateBuffer(device, &bufferCI, nullptr, &buffer.handle));

	VkMemoryRequirements req;
	vkGetBufferMemoryRequirements(device, buffer.handle, &req);

	VkMemoryAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	alloc_info.allocationSize = req.size;
	alloc_info.memoryTypeIndex = GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
	check_vk_result(vkAllocateMemory(device, &alloc_info, nullptr, &buffer.memory));

	check_vk_result(vkBindBufferMemory(device, buffer.handle, buffer.memory, 0));
	buffer.size = req.size;
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

void Renderer::BeginScene(const Camera& camera)
{
	auto windowData = GetWindowData();
	float viewportWidth = static_cast<float>(windowData->Width);
	float viewportHeight = static_cast<float>(windowData->Height);
	
	m_PushConstants.viewProjection = camera.GetViewProjection();

	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
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

	BeginScene(scene.GetCamera());

	RenderRectangle();
	RenderCircle();

	// get entities from Scene? or get Sprites from Scene?
	// what about drawing debug primitives? then get all entities to pass this* to them?
	// for rendering image quads just need some data from a Sprite
	// and also need Entity's tranform for position and scale
}

void Renderer::RenderCircle()
{
	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineCircle);

	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.handle, &offset);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.handle, offset, VK_INDEX_TYPE_UINT16);

	//m_PushConstantsCircle.transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_QuadPosition, 0.f))
	//	* glm::eulerAngleZ(m_QuadAngle)
	// * glm::scale(glm::mat4(1.f), glm::vec3(m_QuadScale.x, m_QuadScale.y, 1.f));
	vkCmdPushConstants(commandBuffer, m_LayoutCircle, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstantsCircle);

	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}

void Renderer::RenderRectangle()
{
	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.handle, &offset);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.handle, offset, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, 0, 1, &m_DescriptorSet, 0, nullptr);

	//m_QuadPosition.x += 0.0025f;
	//m_QuadAngle += 0.05f;
	//m_QuadScale.x += 0.005f;
	m_PushConstants.transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_QuadPosition, 0.f))
							* glm::eulerAngleZ(m_QuadAngle)
							* glm::scale(glm::mat4(1.f), glm::vec3(m_QuadScale.x, m_QuadScale.y, 1.f));
	
	vkCmdPushConstants(commandBuffer, m_Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstants);
	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}
