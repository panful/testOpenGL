#include "cells.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(Cells);

Cells::Cells()
{
    LogDebug("");
}

Cells::~Cells()
{
    LogDebug("");
}

void Cells::Allocate(size_t point_size)
{
}

void Cells::InsertNextCell(std::initializer_list<uint32_t>&& cell)
{
    switch (cell.size())
    {
    case 1:
        m_vertex.insert(std::cend(m_vertex), cell);
        break;
    case 2:
        m_line.insert(std::cend(m_line), cell);
        break;
    case 3:
        m_triangle.insert(std::cend(m_triangle), cell);
        break;
    default:
        LogError("Wrong number of cell :" + std::to_string(cell.size()));
        break;
    }
}

void Cells::InsertCells(CellType type, const std::vector<uint32_t>& cells)
{
    switch (type)
    {
    case CellType::Point:
        m_vertex.insert(std::cend(m_vertex), std::begin(cells), std::end(cells));
        break;
    case CellType::Line:
        if (0 == cells.size() % 2)
        {
            m_line.insert(std::cend(m_vertex), std::begin(cells), std::end(cells));
        }
        break;
    case CellType::Triangle:
        if (0 == cells.size() % 3)
        {
            m_triangle.insert(std::cend(m_vertex), std::begin(cells), std::end(cells));
        }
        break;
    default:
        break;
    }
}

std::vector<uint32_t> Cells::GetVertexIndices() const noexcept
{
    return m_vertex;
}

std::vector<uint32_t> Cells::GetLineIndices() const noexcept
{
    return m_line;
}

std::vector<uint32_t> Cells::GetTriangleIndices() const noexcept
{
    return m_triangle;
}

size_t Cells::GetNumberOfVertices() const noexcept
{
    return m_vertex.size();
}

size_t Cells::GetNumberOfLines() const noexcept
{
    return m_line.size() / 2;
}

size_t Cells::GetNumberOfTriangles() const noexcept
{
    return m_triangle.size() / 3;
}