#pragma once
#include <string_view>

#include <glm/glm.hpp>

#include <Scene/Entity.h>

////////////////////

class Scene;
class Rigidbody;
struct TransformData;

class Player : public Entity
{
public:
	Player() {}

	~Player() {}

	virtual void OnInit() override;

	virtual void Update(float deltaTime) override;

	virtual void DrawDebug(const RendererDebug& rendererDebug) override;

	virtual void OnCollision(const std::shared_ptr<Collision>& other) override;

private:
	glm::vec2 GetMovementInput() const;

	////////////////////

	static constexpr std::string_view defaultTag = "Player";
	static constexpr std::string_view spritePath = "../Assets/bee.png";
	static constexpr glm::vec2 defaultPosition = glm::vec2(100.f, 100.f);

	static constexpr float acceleration = 2.75f;
	static constexpr float deceleration = 2.15f;
	static constexpr float linearDamping = 0.25f;
	static constexpr float defaultMaxSpeed = 150.5f;
	static constexpr float rotationRate = 3.2f;

	////////////////////

	std::shared_ptr<TransformData> m_Transform;

	std::shared_ptr<Rigidbody> m_Rigidbody;

	float m_Speed = 0.f;
	float m_MaxSpeed = defaultMaxSpeed;
};