#pragma once
#include <array>
#include <vector>
#include <list>
#include <memory>

#include <glm/glm.hpp>

#include "../Utility.h"

constexpr uint16_t MAX_DEPTH = 8;
constexpr uint16_t QUAD_COUNT = 4;

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
	Node(const Area& size, const uint16_t depth = 0) : m_Area(size), m_Depth(depth) {}

	void clear()
	{
		m_Elements.clear();

		for (int i = 0; i < QUAD_COUNT; i++)
		{
			if (m_Children[i] != nullptr)
			{
				m_Children[i]->clear();
			}

			m_Children[i].reset();
		}
	}

	uint16_t size() const
	{
		uint16_t count = m_Elements.size();

		for (int i = 0; i < QUAD_COUNT; i++)
		{
			if (m_Children[i] != nullptr)
			{
				count += m_Children[i]->size();
			}
		}

		return count;
	}

	void insert(const IteratorType element, const Area& size)
	{
		if (m_Depth + 1 < MAX_DEPTH)
		{
			for (int i = 0; i < QUAD_COUNT; i++)
			{
				Area childArea{ m_Area.Position, m_Area.Size / 2.0f };
				if (i == 1) childArea.Position.x += childArea.Size.x;
				if (i == 2) childArea.Position.y += childArea.Size.y;
				if (i == 3) childArea.Position += childArea.Size;

				if (childArea.Contains(size))
				{
						if (m_Children[i] == nullptr)
						{
							m_Children[i] = std::make_shared<Node<IteratorType>>(childArea, m_Depth + 1);
						}

						m_Children[i]->insert(element, size);
						return;
				}
			}
		}

		// new element doesn't fit in any of the children
		m_Elements.push_back({ size, element });
	}

	std::list<IteratorType> search(const Area& area) const
	{
		std::list<IteratorType> result;
		search(area, result);
		return result;
	}

	void search(const Area& area, std::list<IteratorType>& result) const
	{
		for (const auto& [elementArea, iterator] : m_Elements)
		{
			if (area.Overlaps(elementArea))
			{
				result.push_back(iterator);
			}
		}

		for (int i = 0; i < QUAD_COUNT; i++)
		{
			if (m_Children[i] != nullptr)
			{
				Area childArea{ m_Area.Position, m_Area.Size / 2.0f };
				if (i == 1) childArea.Position.x += childArea.Size.x;
				if (i == 2) childArea.Position.y += childArea.Size.y;
				if (i == 3) childArea.Position += childArea.Size;

				if (area.Contains(childArea))
				{
					m_Children[i]->elements(result);
				}
				else if (childArea.Overlaps(area))
				{
					m_Children[i]->search(area, result);
				}
			}
		}
	}

	void elements(std::list<IteratorType>& result) const
	{
		for (const auto& [elementArea, iterator] : m_Elements)
		{
			result.push_back(iterator);
		}

		for (int i = 0; i < QUAD_COUNT; i++)
		{
			if (m_Children[i] != nullptr)
			{
				m_Children[i]->elements(result);
			}
		}
	}

	std::list<IteratorType> elements() const
	{
		std::list<IteratorType> result;
		elements(result);
		return result;
	}

private:
	// depth of this Node
	uint16_t m_Depth = 0;

	Area m_Area;

	// somehow the frame time is the same when storing the children areas in every Node
	//std::array<Area, QUAD_COUNT> m_ChildrenAreas{};

	std::array<std::shared_ptr<Node<IteratorType>>, QUAD_COUNT> m_Children{};

	std::vector<std::pair<Area, IteratorType>> m_Elements;
};

////////////////////

// only store indexes from another container here because the tree is used for searching
template<Integral IndexType>
class QuadTree
{
	// list so the iterators inside Node will always be valid
	using ContainerType = std::list<IndexType>;
public:
	QuadTree(const glm::vec2& position, const glm::vec2& size) : m_Root(Area(position, size)) {}

	size_t size() const { return m_Elements.size(); }

	void clear()
	{
		m_Root.clear();
		m_Elements.clear();
	}

	void insert(const IndexType element, const Area& elementSize)
	{
		m_Elements.push_back(element);
		m_Root.insert(std::prev(m_Elements.end()), elementSize);
	}

	// returns a list of iterators (pointing to indexes) to the elements of this structure
	std::list<typename ContainerType::iterator> search(const Area& area) const
	{
		std::list<typename ContainerType::iterator> result;
		m_Root.search(area, result);
		return result;
	}

private:
	ContainerType m_Elements;

	Node<typename ContainerType::iterator> m_Root;
};