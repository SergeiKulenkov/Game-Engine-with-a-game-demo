#pragma once
#include "Entity.h"

////////////////////

class Scene
{
public:
	Scene() {}

	~Scene() {}

	void Update(float deltaTime)
	{
		for (auto& [id, entity] : m_Entities)
		{
			entity->Update(deltaTime);
		}
	}

	void Render()
	{
		// call Render() or Draw() for Sprite components
		// also call RenderDebug() for Entities for drawing debug primitives

		//for (auto& [id, entity] : m_Entities)
		//{
		//	
		//}
	}

	template<typename T>
	T& CreateEntity()
	{
		static_assert(std::is_base_of_v<Entity, T>, "T must be of type Entity.");

		const size_t id = m_Entities.size() + 1;
		m_Entities.emplace(id, std::make_unique<T>());

		T& newEntity = *(static_cast<T*>(m_Entities[id].get()));
		newEntity.Init(id, this);

		return newEntity;
	}

private:
	std::unordered_map<size_t, std::unique_ptr<Entity>> m_Entities;
};