#pragma once
#include "QuadTreeStatic.h"

template<IntegralType IndexType>
class QuadTreeDynamic;

////////////////////

// so there's QuadTreeDynamic<T> that holds a std::list of Elements
// Element holds the data of type T (in this case Id) and its ElementLocation (inside the tree structure)
// so ElementLocation holds the address of the std::list inside some Node that holds this Element
// and the iterator to this specific Element in that std::list
// so there are two lists, each list stores iterators from another list
template<typename IteratorType>
struct ElementLocation
{
	// address of the list in some Node that holds this Element
	typename std::list<std::pair<Area, IteratorType>>* container = nullptr;
	// iterator in that container pointing to this Element
	typename std::list<std::pair<Area, IteratorType>>::iterator iterator;
};

////////////////////

template<IntegralType IndexType>
class Element
{
public:
	IndexType data;

private:
	// iterator in the list in the QuadTreeDynamic
	// see above for the whole structure
	ElementLocation<typename std::list<Element<IndexType>>::iterator> location;

	friend class QuadTreeDynamic<IndexType>;
};

////////////////////

// Node stores items as their Area and an iterator in the QuadTree
template<typename IteratorType>
class NodeDynamic
{
public:
	NodeDynamic(const Area& area, const uint16_t depth = 0)
		: m_Area(area), m_Depth(depth)
	{}

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

	ElementLocation<IteratorType> Insert(const IteratorType element, const Area& area)
	{
		ElementLocation<IteratorType> insertedLocation;
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
						m_Children[i] = std::make_shared<NodeDynamic<IteratorType>>(childArea, m_Depth + 1);
					}

					insertedLocation = m_Children[i]->Insert(element, area);
					inserted = true;
					break;
				}
			}
		}

		// new element doesn't fit in any of the children
		if (!inserted)
		{
			m_Elements.push_back({ area, element });
			insertedLocation = { &m_Elements, std::prev(m_Elements.end()) };
		}

		return insertedLocation;
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

	template<IntegralType IndexType>
	bool Query(const IndexType id, const Area& area, IteratorType& element)
	{
		bool found = false;
		for (const auto& [elementArea, iterator] : m_Elements)
		{
			if (area.Overlaps(elementArea) &&
				(iterator->data == id))
			{
				element = iterator;
				found = true;
				break;
			}
		}

		if (!found)
		{
			std::list<IteratorType> containedElements;
			for (uint16_t i = 0; i < quadCount; i++)
			{
				if (m_Children[i] != nullptr)
				{
					const Area childArea = GetChildArea(i);
					if (area.Contains(childArea))
					{
						m_Children[i]->GetElements(containedElements);
					}
					else if (childArea.Overlaps(area))
					{
						found = m_Children[i]->Query(id, area, element);
						if (found) break;
					}
				}
			}

			if (!found)
			{
				for (const auto& it : containedElements)
				{
					if (it->data == id)
					{
						element = it;
						found = true;
						break;
					}
				}
			}
		}

		return found;
	}

private:
	uint16_t Size() const
	{
		uint16_t count = m_Elements.size();

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

	std::array<std::shared_ptr<NodeDynamic<IteratorType>>, quadCount> m_Children{};

	// elements in this Node, list because iterators are really useful here
	std::list<std::pair<Area, IteratorType>> m_Elements;
};

////////////////////

// only store indexes from another container here because the tree is used for searching
template<IntegralType IndexType>
class QuadTreeDynamic
{
	// list so the iterators inside Node will always be valid
	using ContainerType = std::list<Element<IndexType>>;

public:
	QuadTreeDynamic(const glm::vec2& position, const glm::vec2& area)
		: m_Root(Area(position, area))
	{}

	size_t Size() const { return m_Elements.size(); }

	void Clear()
	{
		m_Root.Clear();
		m_Elements.clear();
	}

	void Insert(const IndexType id, const Area& area)
	{
		Element<IndexType> newElement;
		newElement.data = id;

		m_Elements.push_back(newElement);
		m_Elements.back().location = m_Root.Insert(std::prev(m_Elements.end()), area);
	}

	void Query(const Area& area, std::vector<IndexType>& result) const
	{
		std::list<typename ContainerType::iterator> resultIterators;
		m_Root.Query(area, resultIterators);

		result.clear();
		result.reserve(resultIterators.size());

		for (const auto& it : resultIterators)
		{
			result.push_back(it->data);
		}
	}

	void Update(const IndexType id, const Area& area)
	{
		typename ContainerType::iterator it;
		if (m_Root.Query<IndexType>(id, area, it))
		{
			Update(it, area);
		}
	}

	void Remove(const IndexType id, const Area& area)
	{
		typename ContainerType::iterator it;
		if (m_Root.Query<IndexType>(id, area, it))
		{
			Remove(it);
		}
	}

private:
	std::list<typename ContainerType::iterator> Query(const Area& area) const
	{
		std::list<typename ContainerType::iterator> resultIterators;
		m_Root.Query(area, resultIterators);
		return resultIterators;
	}

	void Update(typename ContainerType::iterator& element, const Area& area)
	{
		element->location.container->erase(element->location.iterator);
		element->location = m_Root.Insert(element, area);
	}

	void Remove(typename ContainerType::iterator& element)
	{
		element->location.container->erase(element->location.iterator);
		m_Elements.erase(element);
	}

	////////////////////

	ContainerType m_Elements;

	NodeDynamic<typename ContainerType::iterator> m_Root;
};