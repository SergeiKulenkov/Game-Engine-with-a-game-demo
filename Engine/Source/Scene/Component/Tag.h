#pragma once
#include <string>

#include "Component.h"

////////////////////

class Tag : public Component
{
public:
	Tag(Entity* entity)
		: Component(entity)
	{}

	Tag(Entity* entity, const std::string& tag)
		: Component(entity), m_Tag(tag)
	{}

	virtual ~Tag() override {}

	std::string GetTag() const { return m_Tag; }
	std::string& GetTag() { return m_Tag; }
	void SetTag(const std::string& tag) { m_Tag = tag; }

private:
	std::string m_Tag = "";
};