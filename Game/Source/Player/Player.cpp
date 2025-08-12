#include "Player.h"

#include <Input/InputManager.h>
#include <Scene/Component/Tag.h>
#include <Scene/Component/Sprite.h>
#include <Scene/Scene.h>
#include <Utility/Utility.h>

////////////////////

Player::Player()
{
	m_Transform = AddComponent<Transform>(defaultPosition).GetTransformData();
	AddComponent<Tag>(defaultTag.data());
	AddComponent<Sprite>(spritePath);
}

void Player::OnInit()
{
	// access Scene to create new Entity
}

void Player::Update(float deltaTime)
{
	const glm::vec2 input = GetMovementInput();
	if (input.y == 1)
	{
		speed += acceleration * deltaTime;
	}
	else if (input.y == -1)
	{
		speed += deceleration * deltaTime * (-1);
	}
	else if (input.y == 0)
	{
		speed += linearDrag * deltaTime * (-1);
	}
	
	if (input.x != 0)
	{
		m_Transform->rotation = Vector::Rotate(m_Transform->rotation, rotationRate * deltaTime * input.x);
	}

	speed = glm::clamp(speed, 0.f, maxSpeed);
	m_Transform->position += m_Transform->rotation * speed;
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

//void Player::DrawDebug(const RendererDebug& rendererDebug)
//{
//	rendererDebug.DrawCircle(m_Transform->position, 28.f, Colour::purple);
//	rendererDebug.DrawLine(m_Transform->position, m_Transform->position + m_Transform->rotation * 40.f, Colour::green);
//}