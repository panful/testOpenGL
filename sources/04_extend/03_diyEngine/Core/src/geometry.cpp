#include "geometry.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(Geometry);

Geometry::Geometry()
{
    LogDebug("");
}

Geometry::~Geometry()
{
    LogDebug("");
}

void Geometry::SetPoints(Points* p)
{
    m_points = p;
}

void Geometry::SetCells(Cells* c)
{
    m_cells = c;
}

Points* Geometry::GetPoints() const
{
    return m_points;
}

Cells* Geometry::GetCells() const
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