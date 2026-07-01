#pragma once
#include <glm/glm.hpp>

////////////////////

class Camera
{
public:
	Camera() {}

	void SetPosition(const glm::vec3& position) {}
	glm::vec3 GetPosition() const { return m_CameraPosition; }

	float GetAngle() const { return m_Angle; }
	float GetNearClipPlane() const { return m_NearClipPlane; }
	float GetFarClipPlane() const { return m_FarClipPlane; }

	void Zoom() {}

private:
	glm::vec3 m_CameraPosition = glm::vec3(0, 0, 3.f);
	float m_Angle = 45.f;
	float m_NearClipPlane = 0.1f;
	float m_FarClipPlane = 1000.f;
};