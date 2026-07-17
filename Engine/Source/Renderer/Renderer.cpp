#include "Renderer.h"

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

	// TODO: how to init descriptors for every image? and change image creation to remove imgui function call?
	InitDescriptors();

	//  TODO: implement a pipeline builder for this
	//InitPipeline(&m_Pipeline, &m_Layout, ObjectType::TEXTURED, 2, rectangleVertexShaderPath, rectangleFragmentShaderPath);
	InitPipeline(&m_PipelineCircle, &m_LayoutCircle, ObjectType::PRIMITIVE_CIRCLE, 3, circleVertexShaderPath, circleFragmentShaderPath);

	m_VerticesCircleBase = new VertexCircle[maxCircles];
	m_Indices = new uint16_t[maxIndices];

	uint16_t offset = 0;
	for (uint16_t i = 0; i < maxIndices; i += 6)
	{
		m_Indices[i]     = offset;
		m_Indices[i + 1] = offset + 2;
		m_Indices[i + 2] = offset + 1;

		m_Indices[i + 3] = offset;
		m_Indices[i + 4] = offset + 3;
		m_Indices[i + 5] = offset + 2;

		offset += 4;
	}
}

void Renderer::InitPipeline(VkPipeline* pipeline, VkPipelineLayout* layout, const ObjectType type, const uint16_t count, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	VkDevice device = Engine::GetDevice();
	VkRenderPass renderPass = GetWindowData()->RenderPass;

	VkPushConstantRange pushConstantRange;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(glm::mat4);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutInfo.pPushConstantRanges = &pushConstantRange;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &m_DescriptorSetLayout;
	check_vk_result(vkCreatePipelineLayout(device, &layoutInfo, nullptr, layout));

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkVertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> attribute_descriptions(count);
	switch (type)
	{
		case ObjectType::TEXTURED:
			bindingDescription.stride = sizeof(Vertex);

			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].binding = bindingDescription.binding;
			attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[0].offset = 0;

			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].binding = bindingDescription.binding;
			attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[1].offset = static_cast<uint32_t>(offsetof(Vertex, textureCoord));
			break;
		case ObjectType::PRIMITIVE_CIRCLE:
			bindingDescription.stride = sizeof(VertexCircle);

			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].binding = bindingDescription.binding;
			attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[0].offset = 0;

			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].binding = bindingDescription.binding;
			attribute_descriptions[1].format = VK_FORMAT_R32_SFLOAT;
			attribute_descriptions[1].offset = static_cast<uint32_t>(offsetof(VertexCircle, thickness));

			attribute_descriptions[2].location = 2;
			attribute_descriptions[2].binding = bindingDescription.binding;
			attribute_descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attribute_descriptions[2].offset = static_cast<uint32_t>(offsetof(VertexCircle, colour));
			break;
		case ObjectType::PRIMITIVE_RECTANGLE:
			break;
		default:
			break;
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = count;
	vertexInputInfo.pVertexAttributeDescriptions = attribute_descriptions.data();

	VkPipelineRasterizationStateCreateInfo raster{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	raster.cullMode = VK_CULL_MODE_BACK_BIT;
	raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	raster.lineWidth = 1.0f;

	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo blend{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	blend.attachmentCount = 1;
	blend.pAttachments = &blendAttachment;

	VkPipelineViewportStateCreateInfo viewport{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewport.viewportCount = 1;
	viewport.scissorCount = 1;

	VkPipelineDepthStencilStateCreateInfo depthStencil{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.stencilTestEnable = VK_FALSE;

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
	pipe.pVertexInputState = &vertexInputInfo;
	pipe.pInputAssemblyState = &inputAssembly;
	pipe.pViewportState = &viewport;
	pipe.pRasterizationState = &raster;
	pipe.pMultisampleState = &multisample;
	pipe.pDepthStencilState = &depthStencil;
	pipe.pColorBlendState = &blend;
	pipe.pDynamicState = &dynamic;
	pipe.layout = *layout;
	pipe.renderPass = renderPass;

	check_vk_result(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, pipeline));
	vkDestroyShaderModule(device, shader_stages[0].module, nullptr);
	vkDestroyShaderModule(device, shader_stages[1].module, nullptr);
}

void Renderer::InitBuffers(Buffer& vertexBuffer, VertexCircle* vertices, VkDeviceSize vertexMemory, Buffer& indexBuffer, uint16_t* indices, VkDeviceSize indexMemory)
{
	VkDevice device = Engine::GetDevice();

	vertexBuffer.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	CreateOrResizeBuffer(vertexBuffer, vertexMemory);

	indexBuffer.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	CreateOrResizeBuffer(indexBuffer, indexMemory);

	void* vbMemory;
	check_vk_result(vkMapMemory(device, vertexBuffer.memory, 0, vertexMemory, 0, &vbMemory));
	memcpy(vbMemory, vertices, static_cast<size_t>(vertexMemory));

	uint16_t* ibMemory;
	check_vk_result(vkMapMemory(device, indexBuffer.memory, 0, indexMemory, 0, (void**)&ibMemory));
	memcpy(ibMemory, indices, static_cast<size_t>(indexMemory));

	VkMappedMemoryRange range[2] = {};
	range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range[0].memory = vertexBuffer.memory;
	range[0].size = VK_WHOLE_SIZE;
	range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range[1].memory = indexBuffer.memory;
	range[1].size = VK_WHOLE_SIZE;

	check_vk_result(vkFlushMappedMemoryRanges(device, 2, range));
	vkUnmapMemory(device, vertexBuffer.memory);
	vkUnmapMemory(device, indexBuffer.memory);
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
	// vulkan debug message
	// binding 0 was created with VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER but VkDescriptorPool was not created with any VkDescriptorPoolSize::type with VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
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

	for (Buffer& buffer : m_VertexBufferCircle)
	{
		vkDestroyBuffer(device, buffer.handle, nullptr);
		vkFreeMemory(device, buffer.memory, nullptr);
	}

	for (Buffer& buffer : m_IndexBuffer)
	{
		vkDestroyBuffer(device, buffer.handle, nullptr);
		vkFreeMemory(device, buffer.memory, nullptr);
	}
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
	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	auto windowData = GetWindowData();
	float viewportWidth = static_cast<float>(windowData->Width);
	float viewportHeight = static_cast<float>(windowData->Height);

	m_CirclesIndexCount = 0;
	m_CirclesVertexCount = 0;
	m_VerticesCirclePtr = m_VerticesCircleBase;

	// also setup scale and translation like in imgui?
	const glm::mat4 viewProjection = camera.GetViewProjection();
	vkCmdPushConstants(commandBuffer, m_LayoutCircle, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &viewProjection);

	VkViewport vp{};
	vp.width = viewportWidth;
	vp.height = viewportHeight;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &vp);

	VkRect2D scissor{};
	scissor.extent.width = windowData->Width;
	scissor.extent.height = windowData->Height;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::EndScene()
{
	const uint32_t frameIndex = Engine::GetFrameIndex();

	VkDeviceSize verticesMemory = m_CirclesVertexCount * sizeof(VertexCircle);
	VkDeviceSize indicesMemory = m_CirclesIndexCount * sizeof(uint16_t);
	InitBuffers(m_VertexBufferCircle[frameIndex], m_VerticesCircleBase, verticesMemory, m_IndexBuffer[frameIndex], m_Indices, indicesMemory);

	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineCircle);

	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBufferCircle[frameIndex].handle, &offset);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer[frameIndex].handle, offset, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(commandBuffer, m_CirclesIndexCount, 1, 0, 0, 0);
}

void Renderer::Render(const Scene& scene)
{
	BeginScene(scene.GetCamera());

	//m_QuadPosition.x += 0.0025f;
	//m_QuadAngle += 0.05f;
	//m_QuadScale.x += 0.005f;

	RenderCircle(glm::vec2(-0.7f, -0.4f), glm::vec2(1.f, 1.f));
	RenderCircle(glm::vec2(0.7f, 0.4f), glm::vec2(1.f, 1.f));

	// get entities from Scene? or get Sprites from Scene?
	// what about drawing debug primitives? then get all entities to pass this* to them?
	// for rendering image quads just need some data from a Sprite
	// and also need Entity's tranform for position and scale

	EndScene();
}

void Renderer::RenderCircle(const glm::vec2& quadPosition, const glm::vec2& quadScale, const float quadAngle)
{	
	// TODO: transoform to apply angle
}

void Renderer::RenderCircle(const glm::vec2& quadPosition, const glm::vec2& quadScale)
{
	// using the same coordinate system as in imgui - start at the top left corner
	// TODO: use position and scale
	m_VerticesCirclePtr->position = glm::vec2(-0.5f, -0.5f);
	m_VerticesCirclePtr->thickness = 0.05f;
	m_VerticesCirclePtr->colour = glm::vec3(161.f / 255.f, 80.f / 255.f, 230.f / 255.f);
	m_VerticesCirclePtr++;

	m_VerticesCirclePtr->position = glm::vec2(0.5f, -0.5f);
	m_VerticesCirclePtr->thickness = 0.05f;
	m_VerticesCirclePtr->colour = glm::vec3(252.f / 255.f, 195.f / 255.f, 40.f / 255.f);
	m_VerticesCirclePtr++;

	m_VerticesCirclePtr->position = glm::vec2(0.5f, 0.5f);
	m_VerticesCirclePtr->thickness = 0.05f;
	m_VerticesCirclePtr->colour = glm::vec3(45.f / 255.f, 115.f / 255.f, 225.f / 255.f);
	m_VerticesCirclePtr++;

	m_VerticesCirclePtr->position = glm::vec2(-0.5f, 0.5f);
	m_VerticesCirclePtr->thickness = 0.05f;
	m_VerticesCirclePtr->colour = glm::vec3(0.f / 255.f, 255.f / 255.f, 0.f / 255.f);
	m_VerticesCirclePtr++;

	m_CirclesVertexCount += 4;
	m_CirclesIndexCount += 6;
}

void Renderer::RenderRectangle(const glm::vec2& quadPosition, const glm::vec2& quadScale, const float quadAngle)
{
	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.handle, &offset);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer[0].handle, offset, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, 0, 1, &m_DescriptorSet, 0, nullptr);

	// TODO: apply Sprite's layer as Z position? or just sort them by position?
	//m_PushConstants.transform = glm::translate(glm::mat4(1.0f), glm::vec3(quadPosition, 0.f))
	//							* glm::eulerAngleZ(quadAngle)
	//							* glm::scale(glm::mat4(1.f), glm::vec3(quadScale.x, quadScale.y, 1.f));

	//vkCmdPushConstants(commandBuffer, m_Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstants);
	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}
