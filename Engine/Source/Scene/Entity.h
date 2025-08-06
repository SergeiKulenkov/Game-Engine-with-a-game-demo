#pragma once
#include <unordered_map>
#include <memory>

#include "Component/Component.h"

////////////////////

class Scene;
struct RendererDebug;

class Entity
{
public:
	virtual ~Entity()
	{
		m_Scene = nullptr;
	}

	virtual void Update(float deltaTime) {}

	// used for drawing debug primitives
	// use Colour struct to select a colour or convert to uint32_t
	virtual void DrawDebug(const RendererDebug& rendererDebug) {}

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be of type Component.");
		assert(!HasComponent<T>() && "This Component is already present.");

		const size_t id = typeid(T).hash_code();
		m_Components.emplace(id, std::make_unique<T>(this, std::forward<Args>(args)...));

		return *(static_cast<T*>(m_Components[id].get()));
	}

	template<typename T>
	T& GetComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be of type Component");

		const auto iterator = m_Components.find(typeid(T).hash_code());
		assert((iterator != m_Components.end()) && "This Component is not present.");

		return *(static_cast<T*>(iterator->second.get()));
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

protected:
	Entity() = default;

	// called after setting the Scene pointer
	// can be used to access Scene methods for the first time
	virtual void OnInit() {}

	size_t m_Id = 0;

	Scene* m_Scene = nullptr;

	std::unordered_map<size_t, std::unique_ptr<Component>> m_Components;

private:
	void Init(const size_t id, Scene* scene)
	{
		m_Id = id;
		m_Scene = scene;
		OnInit();
	}

	////////////////////

	friend class Scene;
};