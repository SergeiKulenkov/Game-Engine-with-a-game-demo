#pragma once

////////////////////

class Entity;

class Component
{
public:
	Component(Entity* entity)
		: m_Entity(entity)
	{}

	virtual ~Component() { m_Entity = nullptr; }

	Entity* GetEntity() const { return m_Entity; }

protected:
	Entity* m_Entity = nullptr;

private:
	friend class Entity;
};