#pragma once

#include "log.h"
#include "object.h"
#include <string>
#include <string_view>
#include <vector>
#include <array>

class DataArray : public Object
{
public:
    static DataArray* New();

    void SetNumberOfComponents(uint8_t);
    uint8_t GetNumberOfComponents() const noexcept;

    void SetNumberOfValues(size_t);
    size_t GetNumberOfValues() const noexcept;

    void SetNumberOfTuples(size_t);
    size_t GetNumberOfTuples() const noexcept;

    template <typename T>
    void SetData(const std::vector<T>&);

    std::vector<float> GetData() const noexcept;
    float GetValue(size_t) const noexcept;
    std::array<float, 2> GetTuple2(size_t) const noexcept;
    std::array<float, 3> GetTuple3(size_t) const noexcept;

    template <typename T>
    void InsertNextValue(T);

    template <typename T>
    void InsertNextTuple2(T, T);

    template <typename T>
    void InsertNextTuple3(T, T, T);

    void SetName(std::string_view);
    std::string_view GetName() const noexcept;

protected:
    DataArray();
    ~DataArray() override;

private:
    uint8_t m_numberOfComponents { 1 };
    std::string m_name { "" };
    std::vector<float> m_data {};
};

template <typename T>
void DataArray::SetData(const std::vector<T>& data)
{
    m_data.assign(data.begin(), data.end());
}

template <typename T>
void DataArray::InsertNextValue(T value)
{
    m_data.emplace_back(static_cast<float>(value));
}

template <typename T>
void DataArray::InsertNextTuple2(T v0, T v1)
{
    if (2 != m_numberOfComponents)
    {
        LogWarn("The number of components do not match the number requested");
    }
    m_data.insert(m_data.cend(), { static_cast<float>(v0), static_cast<float>(v1) });
}

template <typename T>
void DataArray::InsertNextTuple3(T v0, T v1, T v2)
{
    if (3 != m_numberOfComponents)
    {
        LogWarn("The number of components do not match the number requested");
    }
    m_data.insert(m_data.cend(), { static_cast<float>(v0), static_cast<float>(v1), static_cast<float>(v2) });
}