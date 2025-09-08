#include "Player.h"

#include <Input/InputManager.h>
#include <Scene/Component/Tag.h>
#include <Scene/Component/Sprite.h>
#include <Scene/Component/Collider.h>
#include <Scene/Scene.h>
#include <Scene/Physics.h>
#include <Utility/Utility.h>

////////////////////

void Player::OnInit()
{
	m_Transform = AddComponent<Transform>(defaultPosition)->GetTransformData();
	AddComponent<Tag>(defaultTag.data());
	AddComponent<Sprite>(spritePath);
	
	AddComponent<CircleCollider>(14.f, true);
	//AddComponent<BoxCollider>(glm::vec2(25.f, 25.f), true);

	const std::shared_ptr<Scene> sharedScene = m_Scene.lock();
	assert(sharedScene && "This Entity's reference to the Scene is null");
	sharedScene->RegisterDebugWindowField("Player's Max Speed", &m_MaxSpeed, true);
	sharedScene->RegisterDebugWindowField("Player's Current Speed", &m_Speed);
}

void Player::Update(float deltaTime)
{
	const glm::vec2 input = GetMovementInput();
	if (input.y == 1)
	{
		m_Speed += acceleration * deltaTime;
	}
	else if (input.y == -1)
	{
		m_Speed += deceleration * deltaTime * (-1);
	}
	else if (input.y == 0)
	{
		m_Speed += linearDrag * deltaTime * (-1);
	}
	
	if (input.x != 0)
	{
		m_Transform->rotation = Vector::Rotate(m_Transform->rotation, rotationRate * deltaTime * input.x);
	}

	m_Speed = glm::clamp(m_Speed, 0.f, m_MaxSpeed);
	m_Transform->position += m_Transform->rotation * m_Speed;
}

//void Player::OnCollision(const std::shared_ptr<Collision>& other)
//{
//}

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
	// testing raycast
	//std::shared_ptr<RaycastHit> hitResult = std::make_shared<RaycastHit>();
	//const glm::vec2 origin = m_Transform->position + m_Transform->rotation * 20.f;
	//const float length = 80.f;
	//rendererDebug.DrawLine(origin, origin + m_Transform->rotation * length, Colour::green);
	//if (m_Scene->Raycast(origin, m_Transform->rotation, length, hitResult))
	//{
	//	rendererDebug.DrawCircle(hitResult->contactPoint, 10.f, Colour::pink);
	//}

	// draw box collider
	//const std::array<glm::vec2, 4> vertices = GetComponent<BoxCollider>()->GetVertices();
	//for (size_t i = 0; i < vertices.size(); i++)
	//{
	//	rendererDebug.DrawLine(vertices[i], vertices[(i + 1) % vertices.size()], Colour::green);
	//}

	// draw circle collider
	rendererDebug.DrawCircle(m_Transform->position, GetComponent<CircleCollider>()->GetRadius(), Colour::green);

	// draw player's direction
	//rendererDebug.DrawLine(m_Transform->position, m_Transform->position + m_Transform->rotation * 30.f, Colour::green);
}