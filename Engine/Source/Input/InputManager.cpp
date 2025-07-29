#include "InputManager.h"
#include "glfw3.h"

////////////////////

bool InputManager::IsKeyDown(const KeyCode keycode)
{
	assert(m_Window);
	const int state = glfwGetKey(m_Window, (int)keycode);
	return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

bool InputManager::IsMouseButtonDown(const MouseButton button)
{
	assert(m_Window);
	const int state = glfwGetMouseButton(m_Window, (int)button);
	return (state == GLFW_PRESS);
}

bool InputManager::IsMouseButtonUp(const MouseButton button)
{
	assert(m_Window);
	const int state = glfwGetMouseButton(m_Window, (int)button);
	return state == GLFW_RELEASE;
}

glm::vec2 InputManager::GetMousePosition()
{
	assert(m_Window);
	double x;
	double y;
	glfwGetCursorPos(m_Window, &x, &y);
	return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}
