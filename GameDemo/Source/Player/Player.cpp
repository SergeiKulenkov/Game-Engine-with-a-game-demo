#include "Player.h"
#include <vector>

#include <Input/InputManager.h>
#include <Renderer/Camera.h>
#include <Scene/Scene.h>
#include <Scene/Physics.h>
#include <Scene/Component/Transform.h>
#include <Scene/Component/Tag.h>
#include <Scene/Component/Sprite.h>
#include <Scene/Component/Collider.h>
#include <Scene/Component/Rigidbody.h>
#include <Utility/Utility.h>

#include "../Environment/Wall.h"

////////////////////

void Player::OnInit()
{
	m_Transform = AddComponent<Transform>(defaultPosition)->GetTransformData();
	const glm::vec2 size = AddComponent<Sprite>(spritePath)->GetSize();
	
	const float radius = glm::min(size.x, size.y) / 2.f;
	AddComponent<CircleCollider>(radius);
	m_Rigidbody = AddComponent<Rigidbody>(1.f, linearDamping, restitution);

	// testing debug window
	ASSERT_SCENE_NULLPTR(m_Scene);
	m_Scene->RegisterEditableDebugFieldFloat(maxSpeedField.data(), &m_MaxSpeed, maxSpeedLimit, 0.f);
	m_Scene->RegisterDebugFieldFloat(currentSpeedField.data(), &m_Speed);
	m_Scene->RegisterCheckbox(allowInputField, &m_AllowInput);
	m_Scene->RegisterRadioButton(debugChoiceField, (uint8_t*)(&m_DebugChoice), std::vector(labels.begin(), labels.end()));
}

void Player::Update(float deltaTime)
{
	glm::vec2 input = glm::vec2(0.f, 0.f);
	if (m_AllowInput) input = GetMovementInput();

	if (input.x != 0)
	{
		m_Transform->rotation = Vector::Rotate(m_Transform->rotation, rotationRate * deltaTime * input.x);
	}

	if (input.y != 0)
	{
		if (input.y == 1)
		{
			m_Speed += acceleration;
		}
		else if (input.y == -1)
		{
			m_Speed += deceleration * (-1);
		}

		m_Speed = glm::clamp(m_Speed, 0.f, m_MaxSpeed);
		m_Rigidbody->ApplyForce(Force(m_Transform->rotation * m_Speed, false));
	}
	else if (m_Speed > 0)
	{
		m_Speed -= linearDamping;
		if (m_Speed < 0.f) m_Speed = 0.f;
	}

	// camera test
	if (input != glm::vec2(0.f, 0.f))
	{
		const float cameraSpeed = 4.f;
		Camera& camera = m_Scene->GetCamera();
		camera.GetPosition() += glm::vec3(input.x * cameraSpeed * deltaTime, -input.y * cameraSpeed * deltaTime, 0.f);
	}
	else
	{
		if (!m_ZoomKeyPressed)
		{
			float zoomAmount = 0.f;
			if (InputManager::IsKeyPressed(KeyCode::Q))
				zoomAmount += 0.25f;
			else if (InputManager::IsKeyPressed(KeyCode::E))
				zoomAmount -= 0.25f;

			if (zoomAmount != 0.f)
			{
				m_Scene->ZoomCamera(zoomAmount);
				m_ZoomKeyPressed = true;
			}
		}
		else
		{
			if (InputManager::IsKeyUp(KeyCode::Q) && InputManager::IsKeyUp(KeyCode::E))
				m_ZoomKeyPressed = false;
		}
	}
}

void Player::OnCollision(Collision& other)
{
	Debug::Log("x = {}, y = {}", other.contact.x, other.contact.y);
	m_Speed = 0.f;

	if (!other.entity.expired())
	{
		const std::shared_ptr<Entity> sharedEntity = other.entity.lock();
		if (std::dynamic_pointer_cast<Wall>(sharedEntity) == nullptr)
		{
			m_Scene->DestroyEntity(sharedEntity->GetId());
		}
	}

	// here also access the entity inside Collision and call its functions
	// for example it can be an Obstacle, so call ChangeColour
}

glm::vec2 Player::GetMovementInput() const
{
	glm::vec2 input = glm::vec2(0, 0);
	if (InputManager::IsKeyDown(KeyCode::W))
		input.y = 1;
	else if (InputManager::IsKeyDown(KeyCode::S))
		input.y = -1;
	if (InputManager::IsKeyDown(KeyCode::A))
		input.x = -1;
	else if (InputManager::IsKeyDown(KeyCode::D))
		input.x = 1;

	return input;
}

void Player::DrawDebug(const RendererDebug& rendererDebug)
{
	switch (m_DebugChoice)
	{
		case DrawDebugChoice::Circle:
		{
			if (HasComponent<CircleCollider>())
			{
				rendererDebug.DrawCircle(m_Transform->position, GetComponent<CircleCollider>()->GetRadius(), Colour::green);
			}
			break;
		}
		case DrawDebugChoice::Raycast:
		{
			RaycastHit hitResult;
			const glm::vec2 origin = m_Transform->position + m_Transform->rotation * raycastOffset;

			for (int i = 0; i < 9; i++)
			{
				glm::vec2 dir = m_Transform->rotation;
				dir.y -= 0.2f * i;
				rendererDebug.DrawLine(origin, origin + dir * raycastLength, Colour::green);

				if (m_Scene->Raycast(origin, dir, raycastLength, hitResult))
				{
					rendererDebug.DrawCircle(hitResult.contactPoint, 10.f, Colour::pink);
				}
			}
			break;
		}
		case DrawDebugChoice::None:
		default:
			break;
	}
}