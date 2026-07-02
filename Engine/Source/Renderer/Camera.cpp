#include "Camera.h"
#include "../Scene/Scene.h"

////////////////////


void Camera::Init(Scene* scene)
{
	m_Scene = scene;
	UpdateAspectRatio();
	UpdateViewProjection();
}

void Camera::UpdateAspectRatio()
{
	const glm::vec2 screenSize = m_Scene->GetScreenSize();

	// reduce floating point precision here
	// support other aspect ratios
	if (screenSize.x / screenSize.y == aspectRatio169)
	{
		m_AspectRatio = glm::u16vec2(16, 9);
	}
}

void Camera::UpdateViewProjection()
{
	const glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), m_Position)
									* glm::rotate(glm::mat4(1.f), glm::radians(m_RotationAngle), glm::vec3(0.f, 0.f, 1.f));

	// same calculations as in Unity
	//float height = m_OrthoSize * 2.f;
	//float width = height * (m_AspectRatio.x / m_AspectRatio.y);
	//height /= 2.f;
	//width /= 2.f;

	m_ViewProjection = glm::ortho(-m_OrthoSize, m_OrthoSize, -m_OrthoSize, m_OrthoSize, -1.f, 1.f)
					* glm::inverse(cameraTransform);

	// perspective
	//const glm::vec2 screenSize = m_Scene->GetScreenSize();
	//m_ViewProjection = glm::perspectiveFov(glm::radians(m_FOVAngle), screenSize.x, screenSize.y, m_NearClipPlane, m_FarClipPlane)
	//				* glm::inverse(cameraTransform);
}
