#include "Component.h"
#include "../Entity.h"

////////////////////

std::weak_ptr<Entity> Component::GetEntity() const
{
	ASSERT_ENTITY_NULLPTR(m_Entity);
	return m_Entity->GetWeakPointer();
}
