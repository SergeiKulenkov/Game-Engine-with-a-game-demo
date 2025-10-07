#pragma once
#include <memory>

#include "Component.h"
#include "Collider.h"
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
	Rigidbody(const float mass = 1.0f, const float linearDamping = 0.f, const float restitution = 0.f)
		: m_Mass(mass), m_LinearDamping(linearDamping), m_Restitution(restitution)
	{
		// for now for simplicity inertia is the same as mass
		m_Inertia = m_Mass;
		m_InverseMass = 1.f / m_Mass;
		m_InverseInertia = 1.f / m_Inertia;
	}

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

	void Update(float deltaTime)
	{
		if (!m_Force.isImpulse)
		{
			m_LinearVelocity += m_InverseMass * deltaTime * m_Force.amount;
		}
		else
		{
			m_LinearVelocity += m_Force.amount * m_InverseMass;
		}

		m_LinearVelocity *= 1.0f / (1.0f + deltaTime * m_LinearDamping);
		MoveEntity(m_LinearVelocity * deltaTime);

		m_AngularVelocity += m_InverseInertia * m_Torque * deltaTime;
		m_AngularVelocity *= 1.0f / (1.0f + deltaTime * m_AngularDamping);
		RotateEntity(m_AngularVelocity * deltaTime);

		m_Force.Reset();
		m_Torque = 0.f;
	}

	void MoveEntity(const glm::vec2& amount)
	{
		m_TransformData->position += amount;
	}

	void RotateEntity(const float amount)
	{
		m_TransformData->rotation += amount;
		// rotate transform
	}

	void ApplyForce(const Force& amount) { m_Force = amount; }
	void ApplyTorque(const float torque) { m_Torque += torque; }

protected:
	virtual void OnInit() override
	{
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		ASSERT_ENTITY_SHARED_PTR(sharedEntity);
		m_Id = sharedEntity->RegisterRigidbody();

		ASSERRT_HAS_TRANSFORM(sharedEntity->HasComponent<Transform>());
		m_TransformData = sharedEntity->GetComponent<Transform>()->GetTransformData();

		if (sharedEntity->HasComponent<BoxCollider>())
		{
			sharedEntity->GetComponent<BoxCollider>()->SetDynamic(true, m_Id);
		}
		else if (sharedEntity->HasComponent<CircleCollider>())
		{
			sharedEntity->GetComponent<CircleCollider>()->SetDynamic(true, m_Id);
		}
	}

	virtual void OnRemove() override
	{
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		ASSERT_ENTITY_SHARED_PTR(sharedEntity);

		// set collider to dynamic
		if (sharedEntity->HasComponent<BoxCollider>())
		{
			sharedEntity->GetComponent<BoxCollider>()->SetDynamic(false, 0);
		}
		else if (sharedEntity->HasComponent<CircleCollider>())
		{
			sharedEntity->GetComponent<CircleCollider>()->SetDynamic(false, 0);
		}
	}

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