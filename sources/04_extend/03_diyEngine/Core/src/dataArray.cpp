#include "dataArray.h"
#include "objectFactory.h"

StandardNewMacro(DataArray);

DataArray::DataArray()
{
    LogDebug("");
}

DataArray::~DataArray()
{
    LogDebug("");
}

void DataArray::SetNumberOfComponents(uint8_t number)
{
    m_numberOfComponents = number;
}

uint8_t DataArray::GetNumberOfComponents() const noexcept
{
    return m_numberOfComponents;
}

void DataArray::SetName(std::string_view name)
{
    m_name = name;
}

std::string_view DataArray::GetName() const noexcept
{
    return m_name;
}

std::vector<float> DataArray::GetData() const noexcept
{
    return m_data;
}

float DataArray::GetValue(size_t index) const noexcept
{
    return m_data.at(index);
}

std::array<float, 2> DataArray::GetTuple2(size_t index) const noexcept
{
    return { m_data.at(index * 2), m_data.at(index * 2 + 1) };
}

std::array<float, 3> DataArray::GetTuple3(size_t index) const noexcept
{
    return { m_data.at(index * 3), m_data.at(index * 3 + 1), m_data.at(index * 3 + 2) };
}

void DataArray::SetNumberOfValues(size_t num)
{
    m_data.resize(num);
}

size_t DataArray::GetNumberOfValues() const noexcept
{
    return m_data.size();
}

void DataArray::SetNumberOfTuples(size_t num)
{
    m_data.resize(m_numberOfComponents * num);
}

size_t DataArray::GetNumberOfTuples() const noexcept
{
    return m_data.size() / static_cast<size_t>(m_numberOfComponents);
}
