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
	ASSERT_ENTITY_NULLPTR(m_Entity);
	m_Id = m_Entity->RegisterRigidbody();

	ASSERRT_HAS_TRANSFORM(m_Entity->HasComponent<Transform>());
	m_TransformData = m_Entity->GetComponent<Transform>()->GetTransformData();

	std::shared_ptr<Collider> collider = GetCollider();
	collider->SetRigidbodyId(m_Id);
	collider->SetDynamic(true);
}

void Rigidbody::OnRemove()
{
	GetCollider()->SetDynamic(false);
}

void Rigidbody::ResetId(const size_t id)
{
	m_Id = id;
	GetCollider()->SetRigidbodyId(m_Id);
}

std::shared_ptr<Collider> Rigidbody::GetCollider()
{
	std::shared_ptr<Collider> collider;
	ASSERT_ENTITY_NULLPTR(m_Entity);

	if (m_Entity->HasComponent<BoxCollider>())
	{
		collider = m_Entity->GetComponent<BoxCollider>();
	}
	else if (m_Entity->HasComponent<CircleCollider>())
	{
		collider = m_Entity->GetComponent<CircleCollider>();
	}

	return collider;
}
