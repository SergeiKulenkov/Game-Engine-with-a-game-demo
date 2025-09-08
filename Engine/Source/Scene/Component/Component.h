#pragma once
#include <memory>

////////////////////

class Entity;

class Component
{
public:
	Component() {}

	virtual ~Component() {}

	std::shared_ptr<Entity> GetEntity() const
	{
		const std::shared_ptr<Entity> sharedEntity = m_Entity.lock();
		assert(sharedEntity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");
		return sharedEntity;
	}

protected:
	virtual void OnInit() {}

	std::weak_ptr<Entity> m_Entity;

private:
	friend class Entity;
};