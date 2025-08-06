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

	virtual void DrawDebug(const RendererDebug& rendererDebug) override;

private:
	static constexpr std::string_view defaultTag = "Player";
	static constexpr glm::vec2 defaultPosition = glm::vec2(100, 100);
	static constexpr float speed = 15.5f;
	static constexpr std::string_view spritePath = "../Assets/bee.png";

	std::shared_ptr<TransformData> m_Transform = std::make_shared<TransformData>();

	glm::vec2 m_Velocity = glm::vec2(0, 0);
	glm::vec2 m_Acceleration = glm::vec2(0, 0);
};