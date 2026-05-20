#include "Wall.h"

#include <Scene/Component/Collider.h>

////////////////////

void Wall::Setup(const glm::vec2& position, glm::vec2& size)
{
	AddComponent<Transform>(position);

	if (size.x == 0) size.x = boundaryThickness;
	else size.y = boundaryThickness;
	AddComponent<BoxCollider>(size);
}

void Wall::DrawDebug(const RendererDebug& rendererDebug)
{
	const AABB boundingBox = GetComponent<BoxCollider>()->GetAABB();
	rendererDebug.DrawRectangle(boundingBox.min, boundingBox.max, m_Colour);
}
