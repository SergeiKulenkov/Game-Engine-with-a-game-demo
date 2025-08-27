#include "Entity.h"
#include "Scene.h"

////////////////////

Entity::~Entity()
{
	m_Scene = nullptr;
}

void Entity::Destroy(size_t id)
{
	m_Scene->RemoveEntity(id);
}

void Entity::RegisterCollider(Collider& collider)
{
	assert(m_Scene && "This Entity's reference to the Scene is null");
	m_Scene->RegisterCollider(collider);
}

void Entity::Init(const size_t id, Scene* scene)
{
	m_Id = id;
	m_Scene = scene;
	OnInit();
}