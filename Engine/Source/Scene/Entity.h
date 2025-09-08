#pragma once
#include <unordered_map>
#include <memory>
#include <assert.h>

#include "Component/Component.h"

////////////////////

class Scene;
struct RendererDebug;
class Collider;
struct Collision;

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
		static_assert(std::is_base_of_v<Component, T>, "T must be of type Component.");
		assert(!HasComponent<T>() && "This Component is already present.");

		const size_t id = typeid(T).hash_code();
		m_Components.emplace(id, std::make_shared<T>(std::forward<Args>(args)...));
		m_Components[id]->m_Entity = shared_from_this();
		m_Components[id]->OnInit();

		return std::dynamic_pointer_cast<T>(m_Components[id]);
	}

	template<typename T>
	std::shared_ptr<T> GetComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be of type Component");

		const auto iterator = m_Components.find(typeid(T).hash_code());
		assert((iterator != m_Components.end()) && "This Component is not present.");

		return std::dynamic_pointer_cast<T>(iterator->second);
	}

	template<typename T>
	bool HasComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be of type Component");
		return m_Components.find(typeid(T).hash_code()) != m_Components.end();
	}

	template<typename T>
	void RemoveComponent()
	{
		assert(HasComponent<T>() && "This Component is not present.");
		m_Components.erase(typeid(T).hash_code());
	}

	size_t RegisterCollider(const size_t colliderType);

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