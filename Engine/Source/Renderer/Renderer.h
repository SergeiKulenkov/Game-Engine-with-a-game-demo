#pragma once

#include "vulkan/vulkan.h"

class Scene;

////////////////////

class Renderer
{
public:
	// move drawing primitives and images here
	// leave UI windows for imgui
	void DrawImageQuad();

private:
	Renderer() {}

	void Init();
	void Render();

	////////////////////

	VkPipeline m_Pipeline = nullptr;
	VkPipelineLayout m_Layout = nullptr;

	// maybe need to store all sprites here just like colliders in physics??
	// or Renderer can be created by and stored in the Engine
	// then all the vulkan code is moved to the Renderer
	// and it gets a pointer to Scene at the right time to access Sprites

	friend class Scene;
};