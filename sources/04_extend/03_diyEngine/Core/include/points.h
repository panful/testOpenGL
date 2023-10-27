#pragma once

#include "object.h"
#include <vector>

class Points : public Object
{
public:
    static Points* New();

    void Allocate(size_t);
    void InsertNextPoint(float x, float y, float z);
    std::vector<float> GetPoints() const;

protected:
    Points();
    ~Points();

private:
    std::vector<float> m_points;
};