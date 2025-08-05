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

	void OnInit() override;
	void Update(float deltaTime) override;

private:
	static constexpr std::string_view defaultTag = "Player";
	static constexpr glm::vec2 defaultPosition = glm::vec2(100, 100);
	static constexpr float speed = 7.5f;

	std::shared_ptr<TransformData> m_Transform = std::make_shared<TransformData>();
};