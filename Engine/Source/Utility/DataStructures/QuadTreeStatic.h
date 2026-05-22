#pragma once
#include <array>
#include <vector>
#include <list>
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

// Node stores items as their Area and an iterator in the QuadTree
template<typename IteratorType>
class Node
{
public:
	Node(const Area& area, const uint16_t depth = 0) : m_Area(area), m_Depth(depth)
	{
		m_Elements.reserve(defaultElementCount);
	}

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

	void Insert(const IteratorType element, const Area& area)
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
						m_Children[i] = std::make_shared<Node<IteratorType>>(childArea, m_Depth + 1);
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

	void Query(const Area& area, std::list<IteratorType>& result) const
	{
		for (const auto& [elementArea, iterator] : m_Elements)
		{
			if (area.Overlaps(elementArea))
			{
				result.push_back(iterator);
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
					m_Children[i]->Query(area, result);
				}
			}
		}
	}

private:
	uint16_t Size() const
	{
		uint16_t count = m_Elements.area();

		for (uint16_t i = 0; i < quadCount; i++)
		{
			if (m_Children[i] != nullptr)
			{
				count += m_Children[i]->Size();
			}
		}

		return count;
	}

	void GetElements(std::list<IteratorType>& result) const
	{
		for (const auto& [elementArea, iterator] : m_Elements)
		{
			result.push_back(iterator);
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

	// depth of this Node
	uint16_t m_Depth = 0;

	Area m_Area;

	// somehow the frame time is the same when storing the children areas in every Node
	//std::array<Area, QUAD_COUNT> m_ChildrenAreas{};

	std::array<std::shared_ptr<Node<IteratorType>>, quadCount> m_Children{};

	std::vector<std::pair<Area, IteratorType>> m_Elements;
};

////////////////////

// only store indexes from another container here because the tree is used for searching
template<IntegralType IndexType>
class QuadTreeStatic
{
	// list so the iterators inside Node will always be valid
	// this statement is useful to use iterator type and to try other containers
	using ContainerType = std::list<IndexType>;

public:
	QuadTreeStatic(const glm::vec2& position, const glm::vec2& area)
		: m_Root(Area(position, area))
	{}

	size_t Size() const { return m_Elements.size(); }

	void Clear()
	{
		m_Root.Clear();
		m_Elements.clear();
	}

	void Insert(const IndexType element, const Area& elementSize)
	{
		m_Elements.push_back(element);
		m_Root.Insert(std::prev(m_Elements.end()), elementSize);
	}

	void Query(const Area& area, std::vector<IndexType>& result) const
	{
		std::list<typename ContainerType::iterator> resultIterators;
		m_Root.Query(area, resultIterators);

		result.clear();
		result.reserve(resultIterators.size());

		for (const auto& it : resultIterators)
		{
			result.push_back(*it);
		}
	}

private:
	ContainerType m_Elements;

	Node<typename ContainerType::iterator> m_Root;
};