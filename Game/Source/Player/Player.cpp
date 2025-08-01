#include "Player.h"

#include <Component/Tag.h>

////////////////////

Player::Player()
{
	m_Transform = AddComponent<Transform>(defaultPosition).GetTransformData();
	AddComponent<Tag>(defaultTag.data());
}

void Player::Update(float deltaTime)
{
	m_Transform->position.x += speed * deltaTime;
}
