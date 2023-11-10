#pragma once

#include "object.h"
#include <string_view>
#include <unordered_map>

class Points;
class Cells;
class DataArray;

class Geometry : public Object
{
public:
    static Geometry* New();

    void SetPoints(Points*);
    void SetCells(Cells*);

    // void SetScalars();
    // void SetNormals();
    // void SetVectors();
    // void SetTcoords();
    // void SetTangents();
    // void SetTensors();

    void AddPointArray(DataArray*);
    DataArray* GetPointArray(std::string_view) const noexcept;

    void AddCellArray(DataArray*);
    DataArray* GetCellArray(std::string_view) const noexcept;

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
    std::unordered_map<std::string_view, DataArray*> m_pointData {};
    std::unordered_map<std::string_view, DataArray*> m_cellData {};
};