#pragma once
#include <string_view>

#include <glm/glm.hpp>

#include <Entity/Entity.h>
#include <Component/Transform.h>

////////////////////

class Player : public Entity
{
public:
	Player();

	void Update(float deltaTime) override;

private:
	static constexpr std::string_view defaultTag = "Player";
	static constexpr glm::vec2 defaultPosition = glm::vec2(100, 100);
	static constexpr float speed = 7.5f;

	std::shared_ptr<TransformData> m_Transform = std::make_shared<TransformData>();
};