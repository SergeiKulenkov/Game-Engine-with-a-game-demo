#pragma once

////////////////////

class Entity;

class Component
{
public:
	Component(const size_t id, Entity* entity)
		: m_Id(id), m_Entity(entity)
	{
	}

	virtual ~Component();

protected:
	size_t m_Id = 0;
	Entity* m_Entity = nullptr;

private:

};