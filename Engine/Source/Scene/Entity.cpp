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
	const std::shared_ptr sharedScene = m_Scene.lock();
	ASSERT_SCENE_SHARED_PTR(sharedScene);
	return sharedScene->RegisterCollider(std::dynamic_pointer_cast<Collider>(m_Components[colliderType]));
}

void Entity::UnregisterCollider(const size_t id)
{
	const std::shared_ptr sharedScene = m_Scene.lock();
	ASSERT_SCENE_SHARED_PTR(sharedScene);
	sharedScene->UnRegisterCollider(id);
}

size_t Entity::RegisterRigidbody()
{
	const std::shared_ptr sharedScene = m_Scene.lock();
	ASSERT_SCENE_SHARED_PTR(sharedScene);
	return sharedScene->RegisterRigidbody(std::dynamic_pointer_cast<Rigidbody>(m_Components[typeid(Rigidbody).hash_code()]));
}

void Entity::UnregisterRigidbody(const size_t id)
{
	const std::shared_ptr sharedScene = m_Scene.lock();
	ASSERT_SCENE_SHARED_PTR(sharedScene);
	sharedScene->UnRegisterRigidbody(id);
}

void Entity::Init(const size_t id, const std::shared_ptr<Scene>& scene)
{
	m_Id = id;
	m_Scene = scene;
	OnInit();
}