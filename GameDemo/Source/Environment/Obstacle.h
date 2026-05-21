#pragma once
#include <string_view>

#include <Scene/Entity.h>
#include <Scene/Scene.h>
#include <Scene/Component/Collider.h>

////////////////////

class Obstacle : public Entity
{
public:
	Obstacle() {}
	virtual ~Obstacle() {}

	void ChangeColour(const uint32_t colour) { m_Colour = colour; }

	void Setup(const ShapeType type, const glm::vec2& screenSize);

protected:
	virtual void Update(float deltaTime) override {}

	virtual void DrawDebug(const RendererDebug& rendererDebug) override;

private:
	static constexpr std::string_view spritePath = "../Assets/beePurple.png";
	static constexpr float screenOffset = 50.f;
	static constexpr float minSize = 22.f;
	static constexpr float maxSize = 30.f;

	static constexpr float minVelocity = 55.f;
	static constexpr float maxVelocity = 75.f;
	static constexpr float mass = 0.25f;
	static constexpr float linearDamping = 0.1f;
	static constexpr float restitution = 0.9f;

	uint32_t m_Colour = Colour::green;
};