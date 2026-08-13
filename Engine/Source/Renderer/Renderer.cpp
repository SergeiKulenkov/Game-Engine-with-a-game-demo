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
	m_WhiteTexture = std::make_shared<Image>(1, 1, ImageFormat::RGBA);
	const uint32_t white = 0xffffffff;
	m_WhiteTexture->SetData(&white);

	// TODO: how to init descriptors for every image? and change image creation to remove imgui function call?
	InitDescriptors();

	//  TODO: implement a pipeline builder for this
	InitPipeline(&m_Pipeline, &m_Layout, ObjectType::TEXTURED, 3, texturedVertexShaderPath, texturedFragmentShaderPath);
	InitPipeline(&m_PipelineCircle, &m_LayoutCircle, ObjectType::PRIMITIVE_CIRCLE, 4, circleVertexShaderPath, circleFragmentShaderPath);
	InitPipeline(&m_PipelineLine, &m_LayoutLine, ObjectType::LINE, 2, lineVertexShaderPath, lineFragmentShaderPath);

	m_CircleVerticesBase = new VertexCircle[maxVertices];
	m_QuadVerticesBase = new Vertex[maxVertices];
	m_LineVerticesBase = new VertexLine[maxVertices];
	m_Indices = new uint16_t[maxIndices];

	uint16_t offset = 0;
	for (uint16_t i = 0; i < maxIndices; i += 6)
	{
		m_Indices[i]     = offset;
		m_Indices[i + 1] = offset + 1;
		m_Indices[i + 2] = offset + 2;

		m_Indices[i + 3] = offset;
		m_Indices[i + 4] = offset + 2;
		m_Indices[i + 5] = offset + 3;

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
	if (type == ObjectType::TEXTURED)
	{
		layoutInfo.setLayoutCount = 1;
		layoutInfo.pSetLayouts = &m_DescriptorSetLayout;
	}
	check_vk_result(vkCreatePipelineLayout(device, &layoutInfo, nullptr, layout));

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	if (type != ObjectType::LINE)
	{
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
	else inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

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

			attribute_descriptions[2].location = 2;
			attribute_descriptions[2].binding = bindingDescription.binding;
			attribute_descriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attribute_descriptions[2].offset = static_cast<uint32_t>(offsetof(Vertex, colour));
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
			attribute_descriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attribute_descriptions[2].offset = static_cast<uint32_t>(offsetof(VertexCircle, colour));

			attribute_descriptions[3].location = 3;
			attribute_descriptions[3].binding = bindingDescription.binding;
			attribute_descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[3].offset = static_cast<uint32_t>(offsetof(VertexCircle, vertexPosition));
			break;
		case ObjectType::LINE:
			bindingDescription.stride = sizeof(VertexLine);

			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].binding = bindingDescription.binding;
			attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[0].offset = 0;

			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].binding = bindingDescription.binding;
			attribute_descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attribute_descriptions[1].offset = static_cast<uint32_t>(offsetof(VertexLine, colour));
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
	raster.cullMode = VK_CULL_MODE_NONE;
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

void Renderer::InitBuffers(Buffer& vertexBuffer, void* vertices, VkDeviceSize vertexMemory, Buffer& indexBuffer, uint16_t* indices, VkDeviceSize indexMemory)
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
	binding.descriptorCount = 1; // number of textures??
	binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	info.bindingCount = 1;
	info.pBindings = &binding;
	check_vk_result(vkCreateDescriptorSetLayout(device, &info, nullptr, &m_DescriptorSetLayout));
	// vulkan debug message
	// binding 0 was created with VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER but VkDescriptorPool was not created with any VkDescriptorPoolSize::type with VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	m_DescriptorSet = Engine::AllocateSecriptorSet(m_DescriptorSetLayout);
	
	VkWriteDescriptorSet writeDS = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	writeDS.descriptorCount = binding.descriptorCount;
	writeDS.dstSet = m_DescriptorSet;
	writeDS.descriptorType = binding.descriptorType;
	writeDS.dstBinding = 0;
	writeDS.pImageInfo = &m_Image->GetImageInfo();

	vkUpdateDescriptorSets(device, 1, &writeDS, 0, nullptr);
}

void Renderer::Shutdown()
{
	m_Image.reset();
	m_WhiteTexture.reset();

	VkDevice device = Engine::GetDevice();
	vkDestroyPipeline(device, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(device, m_Layout, nullptr);

	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

	vkDestroyPipeline(device, m_PipelineCircle, nullptr);
	vkDestroyPipelineLayout(device, m_LayoutCircle, nullptr);

	vkDestroyPipeline(device, m_PipelineLine, nullptr);
	vkDestroyPipelineLayout(device, m_LayoutLine, nullptr);

	for (Buffer& buffer : m_QuadVertexBuffer)
	{
		vkDestroyBuffer(device, buffer.handle, nullptr);
		vkFreeMemory(device, buffer.memory, nullptr);
	}

	for (Buffer& buffer : m_CircleVertexBuffer)
	{
		vkDestroyBuffer(device, buffer.handle, nullptr);
		vkFreeMemory(device, buffer.memory, nullptr);
	}

	for (Buffer& buffer : m_IndexBuffer)
	{
		vkDestroyBuffer(device, buffer.handle, nullptr);
		vkFreeMemory(device, buffer.memory, nullptr);
	}

	for (Buffer& buffer : m_LineVertexBuffer)
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
	m_CircleVerticesPtr = m_CircleVerticesBase;

	m_QuadIndexCount = 0;
	m_QuadVertexCount = 0;
	m_QuadVerticesPtr = m_QuadVerticesBase;

	m_LineVertexCount = 0;
	m_LineVerticesPtr = m_LineVerticesBase;

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

	// add this as a push constant and multiply vertex position by this scale in shader?
	//constants[0] = 2.0f / draw_data->DisplaySize.x; // Scale
	//constants[1] = 2.0f / draw_data->DisplaySize.y;
	vkCmdPushConstants(commandBuffer, m_LayoutCircle, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &camera.GetViewProjection());
	vkCmdPushConstants(commandBuffer, m_Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &camera.GetViewProjection());
	vkCmdPushConstants(commandBuffer, m_LayoutLine, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &camera.GetViewProjection());
}

void Renderer::EndScene()
{
	const uint32_t frameIndex = Engine::GetFrameIndex();
	if (m_CirclesIndexCount != 0)
	{
		VkDeviceSize verticesMemory = m_CirclesVertexCount * sizeof(VertexCircle);
		VkDeviceSize indicesMemory = m_CirclesIndexCount * sizeof(uint16_t);
		InitBuffers(m_CircleVertexBuffer[frameIndex], m_CircleVerticesBase, verticesMemory, m_IndexBuffer[frameIndex], m_Indices, indicesMemory);

		VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineCircle);

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_CircleVertexBuffer[frameIndex].handle, &offset);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer[frameIndex].handle, offset, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(commandBuffer, m_CirclesIndexCount, m_CirclesIndexCount / 6, 0, 0, 0);
	}
	if (m_QuadIndexCount != 0)
	{
		VkDeviceSize verticesMemory = m_QuadVertexCount * sizeof(Vertex);
		VkDeviceSize indicesMemory = m_QuadIndexCount * sizeof(uint16_t);

		// TODO: refactor to init index buffer separately
		VkDevice device = Engine::GetDevice();
		m_QuadVertexBuffer[frameIndex].usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		CreateOrResizeBuffer(m_QuadVertexBuffer[frameIndex], verticesMemory);

		void* vbMemory;
		check_vk_result(vkMapMemory(device, m_QuadVertexBuffer[frameIndex].memory, 0, verticesMemory, 0, &vbMemory));
		memcpy(vbMemory, m_QuadVerticesBase, static_cast<size_t>(verticesMemory));

		VkMappedMemoryRange range[1] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = m_QuadVertexBuffer[frameIndex].memory;
		range[0].size = VK_WHOLE_SIZE;

		check_vk_result(vkFlushMappedMemoryRanges(device, 1, range));
		vkUnmapMemory(device, m_QuadVertexBuffer[frameIndex].memory);

		// bind and issue the draw command
		VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_QuadVertexBuffer[frameIndex].handle, &offset);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer[frameIndex].handle, offset, VK_INDEX_TYPE_UINT16);

		// bind all textures, access them by index in the shader
		//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, 0, 1, &textures, 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, m_QuadIndexCount, m_QuadIndexCount / 6, 0, 0, 0);
	}
	if (m_LineVertexCount != 0)
	{
		// TODO: refactor to init index buffer separately
		VkDevice device = Engine::GetDevice();
		VkDeviceSize verticesMemory = m_LineVertexCount * sizeof(VertexLine);
		m_LineVertexBuffer[frameIndex].usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		CreateOrResizeBuffer(m_LineVertexBuffer[frameIndex], verticesMemory);

		void* vbMemory;
		check_vk_result(vkMapMemory(device, m_LineVertexBuffer[frameIndex].memory, 0, verticesMemory, 0, &vbMemory));
		memcpy(vbMemory, m_LineVerticesBase, static_cast<size_t>(verticesMemory));

		VkMappedMemoryRange range[1] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = m_LineVertexBuffer[frameIndex].memory;
		range[0].size = VK_WHOLE_SIZE;

		check_vk_result(vkFlushMappedMemoryRanges(device, 1, range));
		vkUnmapMemory(device, m_LineVertexBuffer[frameIndex].memory);

		// bind and issue the draw command
		VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLine);

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_LineVertexBuffer[frameIndex].handle, &offset);

		vkCmdDraw(commandBuffer, m_LineVertexCount, m_LineVertexCount / 2, 0, 0);
	}
}

void Renderer::Render(const Scene& scene)
{
	BeginScene(scene.GetCamera());

	const std::array<glm::vec4, 4 > colours = { glm::vec4(161.f / 255.f, 80.f / 255.f, 230.f / 255.f, 1.0f),
												glm::vec4(252.f / 255.f, 195.f / 255.f, 40.f / 255.f, 1.0f),
												glm::vec4(45.f / 255.f, 115.f / 255.f, 225.f / 255.f, 1.0f),
												glm::vec4(0.f / 255.f, 255.f / 255.f, 0.f / 255.f, 1.0f) };

	
	AddCircle(glm::vec2(0.75f, 0.3f), glm::vec2(0.75f, 0.75f), colours, 0.05f);
	AddCircle(glm::vec2(0.75f, 0.3f), glm::vec2(1.f, 1.f), glm::vec4(0.75f, 0.75f, 0.0f, 1.0f));

	AddImageQuad(glm::vec2(-0.7f, 0.0f), glm::vec2(0.5f, 0.5f), glm::radians(25.f), m_DescriptorSet);

	AddLine(glm::vec2(-0.2f, 0.5f), glm::vec2(0.75f, 0.3f), glm::vec4(1.f, 0.f, 0.0f, 1.0f));
	AddLine(glm::vec2(0.75f, 0.3f), glm::vec2(0.75f, 0.9f), glm::vec4(1.f, 0.f, 0.0f, 1.0f));

	//AddFilledRectangle(glm::vec2(-0.5f, -0.5f), glm::vec2(0.5f, 0.5f), glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), glm::radians(-25.f));

	AddRectangle(glm::vec2(-0.5f, 0.35f), glm::vec2(0.3f, 0.8f), glm::vec4(0.f, 1.f, 0.0f, 1.0f));

	// get entities from Scene? or get Sprites from Scene?
	// what about drawing debug primitives? then get all entities to pass this* to them?
	// for rendering image quads just need some data from a Sprite
	// and also need Entity's tranform for position and scale

	EndScene();
}

void Renderer::AddCircle(const glm::vec2& position, const glm::vec2& scale, const glm::vec4 colour, const float thickness)
{
	const std::array<glm::vec4, 4>& colours = { colour, colour, colour, colour };
	AddCircle(position, scale, colours, thickness);
}

void Renderer::AddCircle(const glm::vec2& position, const glm::vec2& scale, const std::array<glm::vec4, 4>& colours, const float thickness)
{
	const glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(position, 0.f))
								* glm::scale(glm::mat4(1.f), glm::vec3(scale.x, scale.y, 1.f));

	// maybe need to multiply thickness by some scaling value??
	for (uint16_t i = 0; i < vertexNumberForQuad; i++)
	{
		m_CircleVerticesPtr->worldPosition = transform * quadVertexPositions[i];
		m_CircleVerticesPtr->thickness = thickness;
		m_CircleVerticesPtr->colour = colours[i];
		m_CircleVerticesPtr->vertexPosition = quadVertexPositions[i] * 2.f;
		m_CircleVerticesPtr++;
	}

	m_CirclesVertexCount += 4;
	m_CirclesIndexCount += 6;
}

void Renderer::AddFilledRectangle(const glm::vec2& position, const glm::vec2& scale, const glm::vec4 colour, const float angle)
{
	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.f))
								* glm::eulerAngleZ(angle)
								* glm::scale(glm::mat4(1.f), glm::vec3(scale.x, scale.y, 1.f));

	for (uint16_t i = 0; i < vertexNumberForQuad; i++)
	{
		m_QuadVerticesPtr->position = transform * quadVertexPositions[i];
		m_QuadVerticesPtr->textureCoord = textureCoordinates[i];
		m_QuadVerticesPtr->colour = colour;
		m_QuadVerticesPtr++;
	}

	m_QuadVertexCount += 4;
	m_QuadIndexCount += 6;
}

void Renderer::AddImageQuad(const glm::vec2& position, const glm::vec2& scale, const float angle, VkDescriptorSet textureId, const glm::vec4 tintColour)
{
	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.f))
								* glm::eulerAngleZ(angle)
								* glm::scale(glm::mat4(1.f), glm::vec3(scale.x, scale.y, 1.f));

	// TODO: find the texture in the texture list and add its index to the vertices 
	for (uint16_t i = 0; i < vertexNumberForQuad; i++)
	{
		m_QuadVerticesPtr->position = transform * quadVertexPositions[i];
		m_QuadVerticesPtr->textureCoord = textureCoordinates[i];
		m_QuadVerticesPtr->colour = tintColour;
		m_QuadVerticesPtr++;
	}

	m_QuadVertexCount += 4;
	m_QuadIndexCount += 6;
	
	VkCommandBuffer commandBuffer = Engine::GetActiveCommandBuffer();
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, 0, 1, &textureId, 0, nullptr);
}

void Renderer::AddRectangle(const glm::vec2& positionA, const glm::vec2& positionB, const glm::vec4 colour)
{
	AddLine(positionA, glm::vec2(positionB.x, positionA.y), colour);
	AddLine(glm::vec2(positionB.x, positionA.y), positionB, colour);
	AddLine(positionB, glm::vec2(positionA.x , positionB.y), colour);
	AddLine(glm::vec2(positionA.x, positionB.y), positionA, colour);
}

void Renderer::AddLine(const glm::vec2& positionA, const glm::vec2& positionB, const glm::vec4 colour)
{
	m_LineVerticesPtr->position = positionA;
	m_LineVerticesPtr->colour = colour;
	m_LineVerticesPtr++;
	m_LineVerticesPtr->position = positionB;
	m_LineVerticesPtr->colour = colour;
	m_LineVerticesPtr++;

	m_LineVertexCount += 2;
}