#include "Entity.h"
#include "Scene.h"

////////////////////

size_t Entity::RegisterCollider(const size_t colliderType)
{
	const std::shared_ptr sharedScene = m_Scene.lock();
	ASSERT_SCENE_SHARED_PTR(sharedScene);
	return sharedScene->RegisterCollider(std::dynamic_pointer_cast<Collider>(m_Components[colliderType]));
}

size_t Entity::RegisterRigidbody()
{
	const std::shared_ptr sharedScene = m_Scene.lock();
	ASSERT_SCENE_SHARED_PTR(sharedScene);
	return sharedScene->RegisterRigidbody(std::dynamic_pointer_cast<Rigidbody>(m_Components[typeid(Rigidbody).hash_code()]));
}

void Entity::Init(const size_t id, const std::shared_ptr<Scene>& scene)
{
	m_Id = id;
	m_Scene = scene;
	OnInit();
}