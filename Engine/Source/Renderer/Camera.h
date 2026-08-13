#pragma once
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

class Scene;

////////////////////

class Camera
{
public:
	~Camera() { m_Scene = nullptr; }

	const glm::vec3& GetPosition() const { return m_Position; }
	glm::vec3& GetPosition() { return m_Position; }
	void SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		UpdateViewProjection();
	}

	float GetRotationAngle() const { return m_RotationAngle; }
	float& GetRotationAngle() { return m_RotationAngle; }
	void SetRotationAngle(const float angle)
	{
		m_RotationAngle = angle;
		UpdateViewProjection();
	}

	const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
	void SetOrthographicSize(const float size)
	{
		m_OrthoSize = size;
		UpdateViewProjection();
	}

	float GetFOVAngle() const { return m_FOVAngle; }
	float GetNearClipPlane() const { return m_NearClipPlane; }
	float GetFarClipPlane() const { return m_FarClipPlane; }

	// in orthographic mode just modifies the orthographic size by the zoom value
	void Zoom(const float zoom)
	{
		if ((m_OrthoSize + zoom >= minOrthoSize) &&
			(m_OrthoSize + zoom <= maxOrthoSize))
		{
			m_OrthoSize += zoom;
			UpdateViewProjection();
		}
	}

	void UpdateAspectRatio();

private:
	Camera() {}

	void Init(Scene* scene);

	void UpdateViewProjection();

	////////////////////
	
	static constexpr float aspectRatio169 = 1.78f;
	static constexpr float minOrthoSize = 0.25f;
	static constexpr float maxOrthoSize = 2.f;

	Scene* m_Scene = nullptr;

	// TODO: test the Z position
	glm::vec3 m_Position = glm::vec3(0.f, 0.f, 0.f);
	float m_RotationAngle = 0.f;
	glm::mat4 m_ViewProjection = glm::mat4();
	glm::u16vec2 m_AspectRatio = glm::u16vec2(16, 9);
	//float m_ZoomLevel = 1.f;

	// orthographic
	float m_OrthoSize = 1.f;

	// perspective
	float m_FOVAngle = 45.f;
	float m_NearClipPlane = 0.1f;
	float m_FarClipPlane = 1000.f;

	friend class Scene;
};