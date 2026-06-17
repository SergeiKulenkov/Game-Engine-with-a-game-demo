#pragma once
#include <array>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "../Utility.h"

////////////////////

static constexpr uint16_t maxDepth = 8;
static constexpr uint16_t quadCount = 4;
static constexpr uint16_t defaultElementCount = 10;

////////////////////

// quad rectangle
struct Area
{
	constexpr bool Contains(const Area& area) const
	{
		return (area.Position.x >= Position.x) && ((area.Position.x + area.Size.x) < (Position.x + Size.x)) &&
				(area.Position.y >= Position.y) && ((area.Position.y + area.Size.y) < (Position.y + Size.y));
	}

	constexpr bool Overlaps(const Area& area) const
	{
		return (Position.x < (area.Position.x + area.Size.x) && (Position.x + Size.x) >= area.Position.x &&
				Position.y < (area.Position.y + area.Size.y) && (Position.y + Size.y) >= area.Position.y);
	}

	////////////////////

	glm::vec2 Position = glm::vec2(0.f, 0.f);
	glm::vec2 Size = glm::vec2(1.f, 1.f);
};

////////////////////

// only store indexes from another container here because the tree is used for searching
template<IntegralType IndexType>
class QuadTree
{
public:
	// depth is needed when creating sub trees
	QuadTree(const glm::vec2& position, const glm::vec2& area, const uint16_t depth = 0)
		: m_Area(Area(position, area)), m_Depth(depth)
	{
		Init();
	}

	QuadTree(const Area& area, const uint16_t depth = 0)
		: m_Area(area), m_Depth(depth)
	{
		Init();
	}

	void Init() { m_Elements.reserve(defaultElementCount); }

	void Clear()
	{
		m_Elements.clear();

		for (uint16_t i = 0; i < quadCount; i++)
		{
			if (m_Children[i] != nullptr)
			{
				m_Children[i]->Clear();
			}

			m_Children[i].reset();
		}
	}

	void Insert(const IndexType element, const Area& area)
	{
		bool inserted = false;
		if (m_Depth + 1 < maxDepth)
		{
			for (uint16_t i = 0; i < quadCount; i++)
			{
				const Area childArea = GetChildArea(i);
				if (childArea.Contains(area))
				{
					if (m_Children[i] == nullptr)
					{
						m_Children[i] = std::make_unique<QuadTree<IndexType>>(childArea, m_Depth + 1);
					}

					m_Children[i]->Insert(element, area);
					inserted = true;
					break;
				}
			}
		}

		// new element doesn't fit in any of the children
		if (!inserted) m_Elements.push_back({ area, element });
	}

	void Query(const Area& area, std::vector<IndexType>& result, const int32_t selfId = -1) const
	{
		result.clear();
		QueryInternal(area, result, selfId);
	}

	bool Remove(const IndexType data, const Area& area)
	{
		bool found = false;
		uint16_t index = 0;
		for (const auto& [elementArea, id] : m_Elements)
		{
			if (area.Overlaps(elementArea) &&
				(id == data))
			{
				if (index != m_Elements.size() - 1)
				{
					m_Elements[index] = m_Elements[m_Elements.size() - 1];
				}

				m_Elements.pop_back();
				found = true;
				break;
			}

			index++;
		}

		if (!found)
		{
			for (uint16_t i = 0; i < quadCount; i++)
			{
				if (m_Children[i] != nullptr)
				{
					found = m_Children[i]->Remove(data, area);
					if (found) break;
				}
			}
		}

		return found;
	}
	
	// find an element with data and set it to newData
	bool UpdateId(const IndexType data, const Area& area, const IndexType newData)
	{
		bool found = false;
		for (auto& [elementArea, id] : m_Elements)
		{
			if (area.Overlaps(elementArea) &&
				(id == data))
			{
				id = newData;
				found = true;
				break;
			}
		}

		if (!found)
		{
			for (uint16_t i = 0; i < quadCount; i++)
			{
				if (m_Children[i] != nullptr)
				{
					found = m_Children[i]->UpdateId(data, area, newData);
					if (found) break;
				}
			}
		}

		return found;
	}

private:
	void QueryInternal(const Area& area, std::vector<IndexType>& result, const int32_t selfId = -1) const
	{
		for (const auto& [elementArea, id] : m_Elements)
		{
			if (id != selfId &&
				area.Overlaps(elementArea))
			{
				if (result.capacity() == 0) result.reserve(defaultElementCount);
				result.push_back(id);
			}
		}

		for (uint16_t i = 0; i < quadCount; i++)
		{
			if (m_Children[i] != nullptr)
			{
				const Area childArea = GetChildArea(i);
				if (area.Contains(childArea))
				{
					m_Children[i]->GetElements(result);
				}
				else if (childArea.Overlaps(area))
				{
					m_Children[i]->QueryInternal(area, result, selfId);
				}
			}
		}
	}

	void GetElements(std::vector<IndexType>& result) const
	{
		for (const auto& [elementArea, id] : m_Elements)
		{
			result.push_back(id);
		}

		for (uint16_t i = 0; i < quadCount; i++)
		{
			if (m_Children[i] != nullptr)
			{
				m_Children[i]->GetElements(result);
			}
		}
	}

	Area GetChildArea(const uint16_t index) const
	{
		Area childArea{ m_Area.Position, m_Area.Size / 2.0f };
		if (index == 1) childArea.Position.x += childArea.Size.x;
		if (index == 2) childArea.Position.y += childArea.Size.y;
		if (index == 3) childArea.Position += childArea.Size;
		return childArea;
	}

	////////////////////

	// depth of this sub tree
	uint16_t m_Depth = 0;

	Area m_Area;

	std::array<std::unique_ptr<QuadTree<IndexType>>, quadCount> m_Children{};

	std::vector<std::pair<Area, IndexType>> m_Elements;
};