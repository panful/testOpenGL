#include "geometry.h"
#include "cells.h"
#include "dataArray.h"
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

    for (auto [name, data] : m_pointData)
    {
        DestructObjectMemberMacro(data);
    }
    for (auto [name, data] : m_cellData)
    {
        DestructObjectMemberMacro(data);
    }
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
    return m_points->GetNumberOfPoints();
}

size_t Geometry::GetNumberOfCells() const noexcept
{
    return 0;
}

void Geometry::AddPointArray(DataArray* data)
{
    if (m_pointData.contains(data->GetName()))
    {
        LogWarn("DataArray with the same name already exists");
        m_pointData[data->GetName()]->UnRegister(this);
    }

    m_pointData[data->GetName()] = data;
    m_pointData[data->GetName()]->Register(this);
}

DataArray* Geometry::GetPointArray(std::string_view arrayName) const noexcept
{
    return m_pointData.contains(arrayName) ? m_pointData.at(arrayName) : nullptr;
}

void Geometry::AddCellArray(DataArray* data)
{
    if (m_cellData.contains(data->GetName()))
    {
        LogWarn("DataArray with the same name already exists");
        m_cellData[data->GetName()]->UnRegister(this);
    }

    m_cellData[data->GetName()] = data;
    m_cellData[data->GetName()]->Register(this);
}

DataArray* Geometry::GetCellArray(std::string_view arrayName) const noexcept
{
    return m_cellData.contains(arrayName) ? m_cellData.at(arrayName) : nullptr;
}