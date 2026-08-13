#pragma once
#include <string_view>
#include <array>

#include <glm/glm.hpp>

#include <Scene/Entity.h>

class Scene;
class Rigidbody;
struct TransformData;

////////////////////

enum DrawDebugChoice : uint8_t
{
	None = 0,
	Circle,
	Raycast,
	Count,
};

////////////////////

class Player : public Entity
{
public:
	Player() {}
	~Player() {}

protected:
	virtual void OnInit() override;

	virtual void Update(float deltaTime) override;

	virtual void DrawDebug(const RendererDebug& rendererDebug) override;

	virtual void OnCollision(Collision& other) override;

private:
	glm::vec2 GetMovementInput() const;

	////////////////////

	static constexpr std::string_view spritePath = "../Assets/bee.png";
	static constexpr glm::vec2 defaultPosition = glm::vec2(500.f, 300.f);

	static constexpr float acceleration = 2.75f;
	static constexpr float deceleration = 2.15f;
	static constexpr float linearDamping = 0.25f;
	static constexpr float restitution = 0.5f;

	static constexpr float defaultSpeedLimit = 150.f;
	static constexpr float maxSpeedLimit = 200.f;
	static constexpr float rotationRate = 3.5f;

	// debug things
	static constexpr float raycastOffset = 25.f;
	static constexpr float raycastLength = 100.f;
	static constexpr std::array<std::string_view, static_cast<uint8_t>(DrawDebugChoice::Count)> labels = {"None", "Circle", "Raycast"};
	static constexpr std::string_view maxSpeedField = "Player's Max Speed";
	static constexpr std::string_view currentSpeedField = "Player's Current Speed";
	static constexpr std::string_view allowInputField = "Allow input";
	static constexpr std::string_view debugChoiceField = "Player debug choice";

	////////////////////

	std::shared_ptr<TransformData> m_Transform;

	std::shared_ptr<Rigidbody> m_Rigidbody;

	float m_Speed = 0.f;
	float m_MaxSpeed = defaultSpeedLimit;
	bool m_AllowInput = true;
	DrawDebugChoice m_DebugChoice = DrawDebugChoice::Circle;

	bool m_ZoomKeyPressed = false;
};