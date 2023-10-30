#pragma once

#include "object.h"

class Points;
class Cells;

class Geometry : public Object
{
public:
    static Geometry* New();

    void SetPoints(Points*);
    void SetCells(Cells*);

    Points* GetPoints() const noexcept;
    Cells* GetCells() const noexcept;

    size_t GetNumberOfPoints() const noexcept;
    size_t GetNumberOfCells() const noexcept;

protected:
    Geometry();
    ~Geometry() override;

private:
    Points* m_points { nullptr };
    Cells* m_cells { nullptr };
};