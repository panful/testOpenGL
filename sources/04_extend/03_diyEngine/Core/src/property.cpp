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

void Property::SetColor(const std::array<double, 3>& c)
{
    m_color = c;
}

std::array<double, 3> Property::GetColor()  const noexcept
{
    return m_color;
}

void Property::SetEdgeColor(const std::array<double, 3>& c)
{
    m_edgeColor = c;
}

std::array<double, 3> Property::GetEdgeColor()  const noexcept
{
    return m_edgeColor;
}

void Property::SetPointSize(double s)
{
    m_pointSize = s;
}

double Property::GetPointSize()  const noexcept
{
    return m_pointSize;
}

void Property::SetLineWidth(double w)
{
    m_lineWidth = w;
}

double Property::GetLineWidth()  const noexcept
{
    return m_lineWidth;
}

void Property::SetReresentation(Representation r)
{
    m_representation = r;
}

Representation Property::GetReresentation()  const noexcept
{
    return m_representation;
}

void Property::SetEdgeVisibility(bool v)
{
    m_edgeVisibility = v;
}

bool Property::GetEdgeVisibility(bool v)  const noexcept
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