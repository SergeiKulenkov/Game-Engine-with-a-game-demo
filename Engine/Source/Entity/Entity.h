#pragma once
#include <unordered_map>
#include <memory>

#include "../Component/Component.h"

////////////////////

class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void Update(float deltaTime) {}

	template<typename T>
	T* AddComponent()
	{
		T* addedComponent = nullptr;
		if (GetComponent<T>() == nullptr)
		{
			const size_t id = typeid(T).hash_code();
			addedComponent = new T(id, this);

			std::unique_ptr<Component> newComponent = std::unique_ptr<Component>(addedComponent);
			m_Components.emplace(id, std::move(newComponent));
		}

		return addedComponent;
	}

	template<typename T>
	T* GetComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be of type Component");

		T* component = nullptr;
		const auto iterator = m_Components.find(typeid(T).hash_code());

		if (iterator != m_Components.end())
		{
			component = static_cast<T*>(iterator->second.get());
		}

		return component;
	}

	template<typename T>
	void RemoveComponent()
	{
		const size_t id = typeid(T).hash_code();
		const auto iterator = m_Components.find(id);

		if (iterator != m_Components.end())
		{
			m_Components.erase(id);
		}
	}

protected:
	std::unordered_map<size_t, std::unique_ptr<Component>> m_Components;

private:

};