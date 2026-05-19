#pragma once
#include <memory>

#include "Component.h"
#include "Transform.h"

////////////////////

struct Force
{
	void Reset()
	{
		amount = glm::vec2(0.f, 0.f);
		isImpulse = false;
	}

	glm::vec2 amount = glm::vec2(0.f, 0.f);
	bool isImpulse = false;
};

////////////////////

class Rigidbody : public Component
{
public:
	Rigidbody(const float mass = 1.0f, const float linearDamping = 0.f, const float restitution = 0.f);
	~Rigidbody() {}

	size_t GetId() const { return m_Id; }

	void SetMass(const float mass) { m_Mass = mass; }
	float GetMass() const { return m_Mass; }
	float& GetMass() { return m_Mass; }
	float GetInverseMass() const { return m_InverseMass; }

	void SetRestitution(const float restitution) { m_Restitution = restitution; }
	float GetRestitution() const { return m_Restitution; }
	float& GetRestitution() { return m_Restitution; }

	void SetLinearVelocity(const glm::vec2& velocity) { m_LinearVelocity = velocity; }
	glm::vec2 GetLinearVelocity() const { return m_LinearVelocity; }
	glm::vec2& GetLinearVelocity() { return m_LinearVelocity; }

	void SetAngularVelocity(const float velocity) { m_AngularVelocity = velocity; }
	float GetAngularVelocity() const { return m_AngularVelocity; }
	float& GeAngularVelocity() { return m_AngularVelocity; }

	void SetLinearDamping(const float damping) { m_LinearDamping = damping; }
	float GetLinearDamping() const { return m_LinearDamping; }
	float& GetLinearDamping() { return m_LinearDamping; }

	void SetAngularDamping(const float damping) { m_AngularDamping = damping; }
	float GetAngularDamping() const { return m_AngularDamping; }
	float& GetAngularDamping() { return m_AngularDamping; }

	void Update(float deltaTime);
	void MoveEntity(const glm::vec2& amount);
	void RotateEntity(const float amount);

	void ApplyForce(const Force& amount) { m_Force = amount; }
	void ApplyTorque(const float torque) { m_Torque += torque; }

protected:
	virtual void OnInit() override;
	virtual void OnRemove() override;

private:
	size_t m_Id = 0;

	glm::vec2 m_LinearVelocity = glm::vec2(0.f, 0.f);
	float m_LinearDamping = 0.f;
	Force m_Force;

	float m_AngularVelocity = 0.f;
	float m_AngularDamping = 0.f;
	float m_Torque = 0.f;

	float m_Mass = 1.f;
	float m_InverseMass = 0.f;
	float m_Restitution = 0.f;
	float m_Inertia = 1.f;
	float m_InverseInertia = 0.f;

	std::shared_ptr<TransformData> m_TransformData;
};