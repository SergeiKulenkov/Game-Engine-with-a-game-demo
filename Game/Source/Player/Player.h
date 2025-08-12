#pragma once
#include <string_view>

#include <glm/glm.hpp>

#include <Scene/Entity.h>
#include <Scene/Component/Transform.h>

////////////////////

class Scene;

class Player : public Entity
{
public:
	Player();

	~Player() {}

	virtual void OnInit() override;

	virtual void Update(float deltaTime) override;

	//virtual void DrawDebug(const RendererDebug& rendererDebug) override;

private:
	glm::vec2 GetMovementInput() const;

	static constexpr std::string_view defaultTag = "Player";
	static constexpr std::string_view spritePath = "../Assets/bee.png";
	static constexpr glm::vec2 defaultPosition = glm::vec2(100, 100);

	static constexpr float acceleration = 1.55f;
	static constexpr float deceleration = 1.1f;
	static constexpr float linearDrag = 0.8f;
	static constexpr float maxSpeed = 7.5f;
	static constexpr float rotationRate = 3.2f;

	std::shared_ptr<TransformData> m_Transform = std::make_shared<TransformData>();

	glm::vec2 m_Velocity = glm::vec2(0, 0);
	glm::vec2 m_Acceleration = glm::vec2(0, 0);
	float speed = 0.f;
};