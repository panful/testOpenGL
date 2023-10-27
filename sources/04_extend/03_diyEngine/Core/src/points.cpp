#include "points.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(Points);

Points::Points()
{
    LogDebug("");
}

Points::~Points()
{
    LogDebug("");
}

void Points::Allocate(size_t size)
{
    m_points.reserve(size * 3);
}

void Points::InsertNextPoint(float x, float y, float z)
{
    m_points.emplace_back(x);
    m_points.emplace_back(y);
    m_points.emplace_back(z);
}

std::vector<float> Points::GetPoints() const
{
    return m_points;
}