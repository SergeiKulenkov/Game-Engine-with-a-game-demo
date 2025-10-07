#pragma once
#include <memory>
#include <assert.h>

#define ASSERT_ENTITY_SHARED_PTR(entity) assert(entity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");
#define ASSERRT_HAS_TRANSFORM(hasTransform) assert(hasTransform && "Tranform Component is not present.");

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
		ASSERT_ENTITY_SHARED_PTR(sharedEntity);
		return sharedEntity;
	}

protected:
	virtual void OnInit() {}
	virtual void OnRemove() {}

	std::weak_ptr<Entity> m_Entity;

private:
	friend class Entity;
};