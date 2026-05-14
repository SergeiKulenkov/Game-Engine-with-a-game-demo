#pragma once
#include <unordered_map>
#include <unordered_set>

#include <glm/glm.hpp>

#include "../../Scene/Component/Collider.h"

////////////////////

struct Cell
{
	bool operator==(const Cell& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	// upper left corner
	uint16_t x = 0;
	uint16_t y = 0;
};

template<>
struct std::hash<Cell>
{
	size_t operator()(const Cell& cell) const
	{
		return (std::hash<uint16_t>()(cell.x)) ^ (std::hash<uint16_t>()(cell.y));
	}
};

////////////////////

//struct CellElement
//{
//	// id from an outside storage
//	size_t id;
//	AABB boundingBox;
//	// range of cells
//	std::array<Cell, 2> cells{};
//};

////////////////////

class SpatialHashGrid
{
public:
	SpatialHashGrid(const glm::vec2& areaSize, const uint16_t gridSizeX = 64, const uint16_t gridSizeY = 36)
		: m_GridSizeX(gridSizeX), m_GridSizeY(gridSizeY)
	{
		const uint16_t cellSize = static_cast<uint16_t>(areaSize.x) / gridSizeX;
		if (cellSize == (areaSize.y / gridSizeY))
		{
			m_CellSize = cellSize;
		}
	}

	void Insert(const AABB& boundingBox, const size_t Id)
	{
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);
		//const CellElement element(Id, boundingBox, {cellA, cellB});

		for (uint16_t x = cellA.x; x <= cellB.x; x++)
		{
			for (uint16_t y = cellA.y; y <= cellB.y; y++)
			{
				// operator [] adds a new key if it's not present which is fine here
				m_Map[Cell(x, y)].insert(Id);
			}
		}
	}

	std::unordered_set<size_t> Query(const glm::vec2& position, const glm::vec2& areaSize)
	{
		const glm::vec2 min = glm::vec2(position.x - areaSize.x / 2.f, position.y - areaSize.y / 2.f);
		const glm::vec2 max = glm::vec2(position.x + areaSize.x / 2.f, position.y + areaSize.y / 2.f);
		return Query(AABB(min, max));
	}

	// returning a set of Ids
	std::unordered_set<size_t> Query(const AABB& boundingBox)
	{
		std::unordered_set<size_t> result;
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);
		Cell currentCell;

		for (uint16_t x = cellA.x; x <= cellB.x; x++)
		{
			for (uint16_t y = cellA.y; y <= cellB.y; y++)
			{
				currentCell = Cell(x, y);
				if (m_Map.find(currentCell) != m_Map.end())
				{
					for (const size_t id : m_Map.at(currentCell))
					{
						result.emplace(id);
					}
				}
			}
		}

		return result;
	}

	void Update(const AABB& boundingBox, const size_t Id)
	{
		Remove(boundingBox, Id);
		Insert(boundingBox, Id);
	}

	void Remove(const AABB& boundingBox, const size_t Id)
	{
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);
		Cell currentCell;

		for (uint16_t x = cellA.x; x <= cellB.x; x++)
		{
			for (uint16_t y = cellA.y; y <= cellB.y; y++)
			{
				currentCell = Cell(x, y);
				if (m_Map.find(currentCell) != m_Map.end())
				{
					m_Map.at(currentCell).erase(Id);

					//for (const size_t id : m_Map.at(currentCell))
					//{
					//	if (id == Id)
					//	{
					//		std::swap(m_Map.at(currentCell)[m_Map.at(currentCell).size() - 1], Id);
					//		m_Map.at(currentCell).pop_back();
					//		break;
					//	}
					//}
				}
			}
		}
	}

private:
	Cell GetCell(const glm::vec2& position) const
	{
		const uint16_t x = static_cast<uint16_t>(std::floor(position.x / m_CellSize));
		const uint16_t y = static_cast<uint16_t>(std::floor(position.y / m_CellSize));
		return Cell(x, y);
	}

	////////////////////

	uint16_t m_GridSizeX = 1;
	uint16_t m_GridSizeY = 1;

	// 30x30 at 1080p resolution
	uint16_t m_CellSize = 1;

	//std::unordered_map<Cell, std::vector<const size_t>> m_Map;
	//std::unordered_map<Cell, std::vector<CellElement>> m_Map;
	std::unordered_map<Cell, std::unordered_set<size_t>> m_Map;
};