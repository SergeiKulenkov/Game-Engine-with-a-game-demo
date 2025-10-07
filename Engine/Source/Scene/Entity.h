#pragma once
#include <unordered_map>
#include <memory>
#include <assert.h>

#include "Component/Component.h"

#define ASSERT_SCENE_SHARED_PTR(scene) assert(scene && "This Entity's reference to the Scene is null");
#define ASSERT_COMPONENT_PRESENT(present) assert(present && "This Component is already present.");
#define ASSERT_COMPONENT_NOT_PRESENT(notPresent) assert(notPresent && "This Component is not present.");
#define ASSERT_DERIVED_FROM_COMPONENT(isDerived) static_assert(isDerived, "T must be of type Component.");

////////////////////

class Scene;
struct RendererDebug;
class Collider;
struct Collision;
class Rigidbody;

class Entity : public std::enable_shared_from_this<Entity>
{
public:
	Entity() {}

	virtual ~Entity() {}

	virtual void Update(float deltaTime) {}

	// used for drawing debug primitives
	// use Colour struct to select a colour or convert to uint32_t
	virtual void DrawDebug(const RendererDebug& rendererDebug) {}

	virtual void OnCollision(const std::shared_ptr<Collision>& other) {}

	template<typename T, typename... Args>
	std::shared_ptr<T> AddComponent(Args&&... args)
	{
		ASSERT_DERIVED_FROM_COMPONENT((std::is_base_of_v<Component, T>));
		ASSERT_COMPONENT_PRESENT(!HasComponent<T>());

		const size_t id = typeid(T).hash_code();
		m_Components.emplace(id, std::make_shared<T>(std::forward<Args>(args)...));
		m_Components.at(id)->m_Entity = shared_from_this();
		m_Components.at(id)->OnInit();

		return std::dynamic_pointer_cast<T>(m_Components.at(id));
	}

	template<typename T>
	std::shared_ptr<T> GetComponent()
	{
		ASSERT_DERIVED_FROM_COMPONENT((std::is_base_of_v<Component, T>));
		const auto iterator = m_Components.find(typeid(T).hash_code());
		ASSERT_COMPONENT_NOT_PRESENT((iterator != m_Components.end()));

		return std::dynamic_pointer_cast<T>(iterator->second);
	}

	template<typename T>
	bool HasComponent()
	{
		ASSERT_DERIVED_FROM_COMPONENT((std::is_base_of_v<Component, T>));
		return m_Components.find(typeid(T).hash_code()) != m_Components.end();
	}

	template<typename T>
	void RemoveComponent()
	{
		ASSERT_COMPONENT_NOT_PRESENT(HasComponent<T>());
		const size_t id = typeid(T).hash_code();
		m_Components.at(id)->OnRemove();
		m_Components.erase(id);
	}

	size_t RegisterCollider(const size_t colliderType);
	size_t RegisterRigidbody();

	size_t GetId() const { return m_Id; }

protected:
	// called after setting the Scene pointer
	// can be used to access Scene methods for the first time
	// and to call AddComponent() because it requires an initialized shared_ptr to this Entity
	virtual void OnInit() {}

	size_t m_Id = 0;

	std::weak_ptr<Scene> m_Scene;

	std::unordered_map<size_t, std::shared_ptr<Component>> m_Components;

private:
	void Init(const size_t id, const std::shared_ptr<Scene>& scene);

	////////////////////

	friend class Scene;
};