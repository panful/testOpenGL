#include "geometry.h"
#include "cells.h"
#include "log.h"
#include "objectFactory.h"
#include "points.h"

StandardNewMacro(Geometry);

Geometry::Geometry()
{
    LogDebug("");
}

Geometry::~Geometry()
{
    LogDebug("");
    DestructObjectMemberMacro(m_points);
    DestructObjectMemberMacro(m_cells);
}

void Geometry::SetPoints(Points* points)
{
    SetObjectBodyMacro(m_points, points);
}

void Geometry::SetCells(Cells* cells)
{
    SetObjectBodyMacro(m_cells, cells);
}

Points* Geometry::GetPoints() const noexcept
{
    return m_points;
}

Cells* Geometry::GetCells() const noexcept
{
    return m_cells;
}

size_t Geometry::GetNumberOfPoints() const noexcept
{
    return 0;
}

size_t Geometry::GetNumberOfCells() const noexcept
{
    return 0;
}