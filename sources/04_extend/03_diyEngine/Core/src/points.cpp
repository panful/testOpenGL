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

/// @brief 申请size个顶点的内存
/// @param size 
void Points::Allocate(size_t size)
{
    m_points.reserve(size * 3);
}

std::vector<float> Points::GetPoints() const noexcept
{
    return m_points;
}

void Points::SetNumberofPoints(size_t num)
{
    m_points.resize(num);
}

size_t Points::GetNumberOfPoints() const noexcept
{
    return m_points.size() / 3;
}