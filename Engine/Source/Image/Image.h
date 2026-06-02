#pragma once
#include <string>

#include "vulkan/vulkan.h"

////////////////////

enum class ImageFormat
{
	None = 0,
	RGBA,
	RGBA32F
};

////////////////////

class Image
{
public:
	Image(const std::string_view path);
	Image(const uint32_t width, const uint32_t height, const ImageFormat format, const void* data = nullptr);
	~Image();

	void SetData(const void* data);

	VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

	void Resize(const uint32_t width, const uint32_t height);

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

private:
	void AllocateMemory();
	void Release();

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;

	VkImage m_Image = nullptr;
	VkImageView m_ImageView = nullptr;
	VkDeviceMemory m_Memory = nullptr;
	VkSampler m_Sampler = nullptr;

	ImageFormat m_Format = ImageFormat::None;

	VkBuffer m_StagingBuffer = nullptr;
	VkDeviceMemory m_StagingBufferMemory = nullptr;

	size_t m_AlignedSize = 0;

	VkDescriptorSet m_DescriptorSet = nullptr;

	std::string m_Filepath;
};