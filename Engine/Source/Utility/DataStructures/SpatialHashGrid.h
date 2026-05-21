#pragma once
#include <unordered_set>
#include <vector>
#include <array>
#include <assert.h>

#include <glm/glm.hpp>

#define ASSERT_GRID_INDEX(index, gridSize) assert(index < gridSize && "Index out of range.");

////////////////////

struct Cell
{
	bool operator==(const Cell& other) const { return (x == other.x) && (y == other.y); }

	// upper left corner
	uint16_t x = 0;
	uint16_t y = 0;
};

////////////////////

struct CellElement
{
	bool operator==(const CellElement& other) const { return id == other.id; }

	// id from an outside storage
	size_t id;

	std::array<Cell, 2> cellsRange = { Cell(), Cell() };
};

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

		m_Grid.resize(m_GridSizeX * m_GridSizeY);
		uint16_t rowIndex = 0;
		for (uint16_t row = 0; row < m_GridSizeY; row++)
		{
			rowIndex = row * m_GridSizeX;
			for (uint16_t column = 0; column < m_GridSizeX; column++)
			{
				ASSERT_GRID_INDEX(rowIndex + column, m_Grid.size());
				m_Grid[rowIndex + column].reserve(defaultElementsCount);
			}
		}
	}

	void Insert(const AABB& boundingBox, const size_t Id)
	{
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);
		Insert(cellA, cellB, Id);
	}

	void Insert(const Cell cellA, const Cell cellB, const size_t Id)
	{
		CellElement element(Id);
		element.cellsRange = { cellA, cellB };

		uint16_t rowIndex = 0;
		for (uint16_t row = cellA.y; row <= cellB.y; row++)
		{
			rowIndex = row * m_GridSizeX;
			for (uint16_t column = cellA.x; column <= cellB.x; column++)
			{
				ASSERT_GRID_INDEX(rowIndex + column, m_Grid.size());
				m_Grid[rowIndex + column].emplace_back(element);
			}
		}
	}

	// returning Ids
	void Query(const glm::vec2& position, const glm::vec2& areaSize, std::unordered_set<size_t>& result)
	{
		const glm::vec2 min = glm::vec2(position.x - areaSize.x / 2.f, position.y - areaSize.y / 2.f);
		const glm::vec2 max = glm::vec2(position.x + areaSize.x / 2.f, position.y + areaSize.y / 2.f);
		Query(AABB(min, max), result);
	}

	// returning Ids
	void Query(const AABB& boundingBox, std::unordered_set<size_t>& result)
	{
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);

		uint16_t rowIndex = 0;
		for (uint16_t row = cellA.y; row <= cellB.y; row++)
		{
			rowIndex = row * m_GridSizeX;
			for (uint16_t column = cellA.x; column <= cellB.x; column++)
			{
				ASSERT_GRID_INDEX(rowIndex + column, m_Grid.size());
				for (const CellElement& cellElement : m_Grid[rowIndex + column])
				{
					result.insert(cellElement.id);
				}
			}
		}
	}

	void Update(const AABB& boundingBox, const size_t Id)
	{
		// first check if the element still occupies the same cells
		// if not, then remove and insert it
		bool sameCells = false;
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);

		uint16_t rowIndex = 0;
		for (uint16_t row = cellA.y; row <= cellB.y; row++)
		{
			rowIndex = row * m_GridSizeX;
			for (uint16_t column = cellA.x; column <= cellB.x; column++)
			{
				ASSERT_GRID_INDEX(rowIndex + column, m_Grid.size());
				const std::vector<CellElement>& vector = m_Grid[rowIndex + column];
				auto elementPosition = std::find(vector.begin(), vector.end(), CellElement(Id));

				if (elementPosition != vector.end())
				{
					sameCells = (elementPosition->cellsRange[0] == cellA) && (elementPosition->cellsRange[1] == cellB);
					break;
				}
			}

			if (sameCells) break;
		}

		if (!sameCells)
		{
			Remove(cellA, cellB, Id);
			Insert(cellA, cellB, Id);
		}
	}

	void Remove(const AABB& boundingBox, const size_t Id)
	{
		const Cell cellA = GetCell(boundingBox.min);
		const Cell cellB = GetCell(boundingBox.max);
		Remove(cellA, cellB, Id);
	}

	void Remove(const Cell cellA, const Cell cellB, const size_t Id)
	{
		uint16_t rowIndex = 0;
		for (uint16_t row = cellA.y; row <= cellB.y; row++)
		{
			rowIndex = row * m_GridSizeX;
			for (uint16_t column = cellA.x; column <= cellB.x; column++)
			{
				ASSERT_GRID_INDEX(rowIndex + column, m_Grid.size());
				std::vector<CellElement>& vector = m_Grid[rowIndex + column];
				auto elementPosition = std::find(vector.begin(), vector.end(), CellElement(Id));

				if (elementPosition != vector.end())
				{
					*elementPosition = vector[vector.size() - 1];
					vector.pop_back();
				}
			}
		}
	}

private:
	Cell GetCell(const glm::vec2& position) const
	{
		uint16_t x = static_cast<uint16_t>(std::floor(position.x / m_CellSize));
		// allowing the maximum edges of the grid
		if (x == m_GridSizeX) x--;

		uint16_t y = static_cast<uint16_t>(std::floor(position.y / m_CellSize));
		if (y == m_GridSizeY) y--;

		return Cell(x, y);
	}

	////////////////////

	static constexpr uint16_t defaultElementsCount = 8;

	uint16_t m_GridSizeX = 1;
	uint16_t m_GridSizeY = 1;

	// 30x30 at 1080p resolution, cells bigger than 40 seem to make performance worse
	uint16_t m_CellSize = 1;

	std::vector<std::vector<CellElement>> m_Grid;
};