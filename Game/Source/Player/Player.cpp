#include "Player.h"

#include <Scene/Component/Tag.h>

////////////////////

Player::Player()
{
	m_Transform = AddComponent<Transform>(defaultPosition).GetTransformData();
	AddComponent<Tag>(defaultTag.data());
}

void Player::OnInit()
{
	// access Scene to create new Entity
}

void Player::Update(float deltaTime)
{
	m_Transform->position.x += speed * deltaTime;
}