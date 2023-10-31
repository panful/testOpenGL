#include "property.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(Property);

Property::Property()
{
    LogDebug("");
}

Property::~Property()
{
    LogDebug("");
}

void Property::Render()
{
}

void Property::SetColor(const std::array<double, 3>& c)
{
    m_color = c;
}

std::array<double, 3> Property::GetColor() const noexcept
{
    return m_color;
}

void Property::SetEdgeColor(const std::array<double, 3>& c)
{
    m_edgeColor = c;
}

std::array<double, 3> Property::GetEdgeColor() const noexcept
{
    return m_edgeColor;
}

void Property::SetOpacity(double opacity)
{
    if (opacity > 1.0 || opacity < 0.0)
    {
        LogError("Opacity cannot be less than 0.0 or greater than 1.0");
        return;
    }
    m_opacity = opacity;
}

double Property::GetOpacity() const noexcept
{
    return m_opacity;
}

void Property::SetPointSize(double size)
{
    m_pointSize = size;
}

double Property::GetPointSize() const noexcept
{
    return m_pointSize;
}

void Property::SetLineWidth(double width)
{
    m_lineWidth = width;
}

double Property::GetLineWidth() const noexcept
{
    return m_lineWidth;
}

void Property::SetReresentation(Representation representation)
{
    m_representation = representation;
}

Representation Property::GetReresentation() const noexcept
{
    return m_representation;
}

void Property::SetEdgeVisibility(bool visibility)
{
    m_edgeVisibility = visibility;
}

bool Property::GetEdgeVisibility() const noexcept
{
    return m_edgeVisibility;
}

void Property::EdgeVisibilityOn()
{
    m_edgeVisibility = true;
}

void Property::EdgeVisibilityOff()
{
    m_edgeVisibility = false;
}