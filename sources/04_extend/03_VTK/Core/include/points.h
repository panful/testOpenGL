#pragma once

#include "object.h"
#include <vector>

class Points : public Object
{
public:
    static Points* New();

    void Allocate(size_t);

    template <typename T>
    void InsertNextPoint(T, T, T);

    template <typename T>
    void SetPoint(size_t, T, T, T);

    std::vector<float> GetPoints() const noexcept;

    void SetNumberofPoints(size_t);
    size_t GetNumberOfPoints() const noexcept;

protected:
    Points();
    ~Points() override;

private:
    std::vector<float> m_points {};
};

template <typename T>
void Points::InsertNextPoint(T x, T y, T z)
{
    m_points.insert(m_points.cend(), { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) });
}

template <typename T>
void Points::SetPoint(size_t index, T x, T y, T z)
{
    m_points[index * 3]     = static_cast<float>(x);
    m_points[index * 3 + 1] = static_cast<float>(y);
    m_points[index * 3 + 2] = static_cast<float>(z);
}