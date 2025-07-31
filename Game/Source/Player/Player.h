#pragma once
#include "Entity/Entity.h"

#include <glm/glm.hpp>

////////////////////

class Player : public Entity
{
public:
	Player();

	void Update(float deltaTime) override;

private:

};