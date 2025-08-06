#include "Player.h"

#include <Input/InputManager.h>
#include <Scene/Component/Tag.h>
#include <Scene/Component/Sprite.h>
#include <Scene/Scene.h>

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
	m_Transform->position += speed * deltaTime * m_Transform->rotation;
}

void Player::DrawDebug(const RendererDebug& rendererDebug)
{
	rendererDebug.DrawCircle(m_Transform->position, 28.f, Colour::purple);
	rendererDebug.DrawLine(m_Transform->position, m_Transform->position + m_Transform->rotation * 40.f, Colour::green);
}