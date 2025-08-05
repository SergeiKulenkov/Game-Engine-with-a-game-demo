#pragma once

////////////////////

class Entity;

class Component
{
public:
	Component() {}

	Component(Entity* entity)
		: m_Entity(entity)
	{}

	virtual ~Component() { m_Entity = nullptr; }

protected:
	Entity* m_Entity = nullptr;

private:

	friend class Entity;
};