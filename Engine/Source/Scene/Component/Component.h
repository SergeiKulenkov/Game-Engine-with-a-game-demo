#pragma once
#include <memory>
#include <assert.h>

class Entity;

#define ASSERT_ENTITY_NULLPTR(entity) assert(entity && "Entity's pointer is nillptr.");
#define ASSERT_ENTITY_SHARED_PTR(entity) assert(entity && "Can't get Entity's shared pointer for this Component because it's no longer valid.");
#define ASSERRT_HAS_TRANSFORM(hasTransform) assert(hasTransform && "Tranform Component is not present.");
#define ASSERT_TRANSFORM_SHARED_PTR(transform) assert(transform && "Can't get Transform's shared pointer because it's no longer valid.");

////////////////////

class Component
{
public:
	Component() {}
	virtual ~Component() { m_Entity = nullptr; }

	// for example used for collisions
	// so it may be used to store an entity for a long time, so need a way to check validity
	std::weak_ptr<Entity> GetEntity() const;

protected:
	virtual void OnInit() {}
	virtual void OnRemove() {}

	Entity* m_Entity = nullptr;

private:
	friend class Entity;
};