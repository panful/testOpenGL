#pragma once

#include "object.h"
#include <vector>

enum class CellType
{
    Point,
    Line,
    Triangle,
};

class Cells : public Object
{
public:
    static Cells* New();

    void Allocate(size_t);
    void InsertNextCell(std::initializer_list<uint32_t>&&);
    void InsertCells(CellType, const std::vector<uint32_t>&);

    std::vector<uint32_t> GetVertexIndices() const;
    std::vector<uint32_t> GetLineIndices() const;
    std::vector<uint32_t> GetTriangleIndices() const;

protected:
    Cells();
    ~Cells();

private:
    std::vector<uint32_t> m_vertex;
    std::vector<uint32_t> m_line;
    std::vector<uint32_t> m_triangle;
};