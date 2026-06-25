#pragma once

#include "vulkan/vulkan.h"

class Engine;
class Scene;

////////////////////

class Renderer
{
public:
	void Render(const Scene& scene);

	// move drawing primitives and images here
	// leave UI windows for imgui
	void DrawImageQuad();

private:
	Renderer() {}

	void Init();
	void InitPipeline();

	////////////////////

	VkPipeline m_Pipeline = nullptr;
	VkPipelineLayout m_Layout = nullptr;

	friend class Engine;
};