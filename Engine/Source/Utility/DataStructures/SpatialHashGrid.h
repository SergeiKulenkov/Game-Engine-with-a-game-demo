#pragma once
#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>

#include <glm/glm.hpp>

////////////////////

struct Cell
{
	bool operator==(const Cell& other) const { return (x == other.x) && (y == other.y); }

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

struct CellElement
{
	bool operator==(const CellElement& other) const { return id == other.id; }

	// id from an outside storage
	size_t id;
	int16_t queryId = -1;
	std::array<Cell, 2> cellsRange = { Cell(), Cell() };
};

template<>
struct std::hash<CellElement>
{
	size_t operator()(const CellElement& element) const { return element.id; }
};

////////////////////

class SpatialHashGrid
{
public:
	SpatialHashGrid(const glm::vec2& areaSize, const uint16_t gridSizeX = 64, const uint16_t gridSizeY = 36)
		: m_GridSizeX(gridSizeX), m_GridSizeY(gridSizeY)
	{
		m_Map.reserve(gridSizeX * gridSizeY);

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

		CellElement element(Id);
		element.cellsRange = {cellA, cellB};

		for (uint16_t x = cellA.x; x <= cellB.x; x++)
		{
			for (uint16_t y = cellA.y; y <= cellB.y; y++)
			{
				// operator [] adds a new key if it's not present which is fine here
				m_Map[Cell(x, y)].emplace_back(element);
			}
		}
	}

	std::vector<size_t> Query(const glm::vec2& position, const glm::vec2& areaSize)
	{
		const glm::vec2 min = glm::vec2(position.x - areaSize.x / 2.f, position.y - areaSize.y / 2.f);
		const glm::vec2 max = glm::vec2(position.x + areaSize.x / 2.f, position.y + areaSize.y / 2.f);
		return Query(AABB(min, max));
	}

	// returning Ids
	std::vector<size_t> Query(const AABB& boundingBox)
	{
		std::vector<size_t> result;
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);
		Cell currentCell;

		m_QueryId++;
		for (uint16_t x = cellA.x; x <= cellB.x; x++)
		{
			for (uint16_t y = cellA.y; y <= cellB.y; y++)
			{
				currentCell = Cell(x, y);
				if (m_Map.find(currentCell) != m_Map.end())
				{
					for (CellElement& cellElement : m_Map.at(currentCell))
					{
						if (cellElement.queryId != m_QueryId)
						{
							// if the same element is found in another cell during this query, it won't be added
							cellElement.queryId = m_QueryId;
							result.emplace_back(cellElement.id);
						}
					}
				}
			}
		}

		return result;
	}

	void Update(const AABB& boundingBox, const size_t Id)
	{
		// first check if the element still occupies the same cells
		// if not, then remove and insert it
		bool sameCells = false;
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);
		
		for (uint16_t x = cellA.x; x <= cellB.x; x++)
		{
			for (uint16_t y = cellA.y; y <= cellB.y; y++)
			{
				const Cell currentCell = Cell(x, y);
				if (m_Map.find(currentCell) != m_Map.end())
				{
					std::vector<CellElement>& vector = m_Map.at(currentCell);
					auto elementPosition = std::find_if(vector.begin(), vector.end(), [Id](CellElement& element) { return element.id == Id; });
					if (elementPosition != vector.end())
					{
						sameCells = (elementPosition->cellsRange[0] == cellA && elementPosition->cellsRange[1] == cellB);
						break;
					}
				}
			}

			if (sameCells) break;
		}

		if (!sameCells)
		{
			Remove(boundingBox, Id);
			Insert(boundingBox, Id);
		}
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
					std::vector<CellElement>& vector = m_Map.at(currentCell);
					auto elementPosition = std::find_if(vector.begin(), vector.end(), [Id](CellElement& element) { return element.id == Id; });
					if (elementPosition != vector.end())
					{
						*elementPosition = vector[vector.size() - 1];
						vector.pop_back();
					}
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

	uint16_t m_QueryId = 0;

	std::unordered_map<Cell, std::vector<CellElement>> m_Map;
};