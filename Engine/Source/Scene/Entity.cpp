#include "Entity.h"
#include "Scene.h"
#include "Component/Rigidbody.h"

////////////////////

void Entity::OnDestroy()
{
	for (auto it = m_Components.begin(); it != m_Components.end();)
	{
		it->second->OnRemove();
		it = m_Components.erase(it);
	}
}

size_t Entity::RegisterCollider(const size_t colliderType)
{
	ASSERT_SCENE_NULLPTR(m_Scene);
	return m_Scene->RegisterCollider(std::dynamic_pointer_cast<Collider>(m_Components[colliderType]));
}

void Entity::UnregisterCollider(const size_t id)
{
	ASSERT_SCENE_NULLPTR(m_Scene);
	m_Scene->UnRegisterCollider(id);
}

size_t Entity::RegisterRigidbody()
{
	ASSERT_SCENE_NULLPTR(m_Scene);
	return m_Scene->RegisterRigidbody(std::dynamic_pointer_cast<Rigidbody>(m_Components[typeid(Rigidbody).hash_code()]));
}

void Entity::UnregisterRigidbody(const size_t id)
{
	ASSERT_SCENE_NULLPTR(m_Scene);
	m_Scene->UnRegisterRigidbody(id);
}

void Entity::Init(const size_t id, Scene* scene)
{
	m_Id = id;
	m_Scene = scene;
	OnInit();
}