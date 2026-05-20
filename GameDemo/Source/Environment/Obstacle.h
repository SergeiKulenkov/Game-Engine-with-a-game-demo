#pragma once
#include "Scene/Entity.h"
#include <Scene/Scene.h>
#include <Scene/Component/Collider.h>

////////////////////

class Obstacle : public Entity
{
public:
	Obstacle() {}	
	~Obstacle() {}

	void ChangeColour(const uint32_t colour) { m_Colour = colour; }

	void Setup(const ShapeType type);

protected:
	virtual void Update(float deltaTime) override;

	virtual void DrawDebug(const RendererDebug& rendererDebug) override;

private:
	static constexpr float minObstaclePosition = 100.f;
	static constexpr float maxObstaclePosition = 950.f;
	static constexpr float minObstacleSize = 18.f;
	static constexpr float maxObstacleSize = 30.f;

	static constexpr float minObstacleVelocity = 10.f;
	static constexpr float maxObstacleVelocity = 12.f;
	static constexpr float obstacleMass = 0.25f;
	static constexpr float obstacleLinearDamping = 0.1f;
	static constexpr float obstacleRestitution = 0.9f;

	uint32_t m_Colour = Colour::green;
};