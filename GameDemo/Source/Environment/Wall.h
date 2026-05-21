#pragma once
#include <Scene/Entity.h>
#include <Scene/Scene.h>
#include <Scene/Component/Collider.h>

////////////////////

class Wall : public Entity
{
public:
	Wall() {}
	virtual ~Wall() {}

	void ChangeColour(const uint32_t colour) { m_Colour = colour; }

	// parameters need to be adjusted with the config values
	void Setup(glm::vec2 position, glm::vec2 size);

protected:
	virtual void Update(float deltaTime) override {}

	virtual void DrawDebug(const RendererDebug& rendererDebug) override;

private:
	static constexpr float thickness = 5.f;

	uint32_t m_Colour = Colour::green;
};