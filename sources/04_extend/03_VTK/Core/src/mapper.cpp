#include "mapper.h"
#include "geometry.h"
#include "log.h"
#include "lookupTable.h"
#include "objectFactory.h"

ObjectFactoryNewMacro(Mapper);

Mapper::Mapper()
{
    LogDebug("");
}

Mapper::~Mapper()
{
    LogDebug("");
    DestructObjectMemberMacro(m_geometry);
    DestructObjectMemberMacro(m_lookupTable);
}

void Mapper::SetGeometry(Geometry* geometry)
{
    SetObjectBodyMacro(m_geometry, geometry);
}

void Mapper::Render(Renderer*, Actor*)
{
}

void Mapper::SetColorVisibility(bool colorVisibility)
{
    m_colorVisibility = colorVisibility;
}

bool Mapper::GetColorVisibility() const noexcept
{
    return m_colorVisibility;
}

void Mapper::SetColorArray(std::string_view colorArray)
{
    m_colorArray = colorArray;
}

std::string_view Mapper::GetColorArray() const noexcept
{
    return m_colorArray;
}

void Mapper::SetUseLookupTable(bool useLookupTable)
{
    m_useLookupTable = useLookupTable;
}

bool Mapper::GetUseLookupTable() const noexcept
{
    return m_useLookupTable;
}

void Mapper::SetLookupTable(LookupTable* lookupTable)
{
    SetObjectBodyMacro(m_lookupTable, lookupTable);
}

LookupTable* Mapper::GetLookupTable() const noexcept
{
    return m_lookupTable;
}

void Mapper::SetColorMode(ColorMode colorMode)
{
    m_colorMode = colorMode;
}

void Mapper::SetColorModeToPointData()
{
    m_colorMode = ColorMode::Point;
}

void Mapper::SetColorModeToCellData()
{
    m_colorMode = ColorMode::Cell;
}

/// @brief
/// @param value true:给每个顶点设置颜色着色器自动插值 false:纹理映射颜色
void Mapper::SetInterpolateColorBeforeMapping(bool value)
{
    m_interpolateColorBeforeMapping = value;
}

bool Mapper::GetInterpolateColorBeforeMapping() const noexcept
{
    return m_interpolateColorBeforeMapping;
}

void Mapper::InterpolateColorBeforeMappingOn()
{
    m_interpolateColorBeforeMapping = true;
}

void Mapper::InterpolateColorBeforeMappingOff()
{
    m_interpolateColorBeforeMapping = false;
}