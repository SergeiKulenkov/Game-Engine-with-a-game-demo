#pragma once
#include "glm/glm.hpp"
#include "KeyCodes.h"

struct GLFWwindow;

////////////////////

class InputManager
{
public:
	void Init(GLFWwindow* window) { m_Window = window; }

	static bool IsKeyDown(const KeyCode keycode);

	static bool IsMouseButtonDown(const MouseButton button);

	static bool IsMouseButtonUp(const MouseButton button);

	static glm::vec2 GetMousePosition();

private:
	inline static GLFWwindow* m_Window = nullptr;
};