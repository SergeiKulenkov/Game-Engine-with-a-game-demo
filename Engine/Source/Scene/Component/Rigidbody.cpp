#include "Rigidbody.h"

#include "Collider.h"
#include "../../Utility/Utility.h"

////////////////////

Rigidbody::Rigidbody(const float mass, const float linearDamping, const float restitution)
	: m_Mass(mass), m_LinearDamping(linearDamping), m_Restitution(restitution)
{
	// for now for simplicity inertia is the same as mass
	m_Inertia = m_Mass;
	m_InverseMass = 1.f / m_Mass;
	m_InverseInertia = 1.f / m_Inertia;
}

void Rigidbody::Update(float deltaTime)
{
	if (!m_Force.isImpulse)
	{
		m_LinearVelocity += m_InverseMass * deltaTime * m_Force.amount;
	}
	else m_LinearVelocity += m_Force.amount * m_InverseMass;

	m_LinearVelocity *= 1.0f / (1.0f + deltaTime * m_LinearDamping);
	MoveEntity(m_LinearVelocity * deltaTime);

	m_AngularVelocity += m_InverseInertia * m_Torque * deltaTime;
	m_AngularVelocity *= 1.0f / (1.0f + deltaTime * m_AngularDamping);
	RotateEntity(m_AngularVelocity * deltaTime);

	m_Force.Reset();
	m_Torque = 0.f;
}

void Rigidbody::MoveEntity(const glm::vec2& amount)
{
	m_TransformData->position += amount;
}

void Rigidbody::RotateEntity(const float amount)
{
	m_TransformData->rotation = Vector::Rotate(m_TransformData->rotation, amount);
}

void Rigidbody::OnInit()
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

void Rigidbody::OnRemove()
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