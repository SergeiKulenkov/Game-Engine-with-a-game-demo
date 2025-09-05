#include "Entity.h"
#include "Scene.h"

////////////////////

Entity::~Entity()
{
	m_Scene = nullptr;
}

size_t Entity::RegisterCollider(Collider& collider)
{
	assert(m_Scene && "This Entity's reference to the Scene is null");
	return m_Scene->RegisterCollider(collider);
}

void Entity::Init(const size_t id, Scene* scene)
{
	m_Id = id;
	m_Scene = scene;
	OnInit();
}