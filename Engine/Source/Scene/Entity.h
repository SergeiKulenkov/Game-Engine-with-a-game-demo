#pragma once
#include <unordered_map>
#include <memory>

#include "Component/Component.h"

////////////////////

class Scene;

class Entity
{
public:
	virtual ~Entity()
	{
		m_Scene = nullptr;
	}

	virtual void OnInit() {}

	virtual void Update(float deltaTime) {}

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