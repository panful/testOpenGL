#pragma once

#include "Node.h"
#include <initializer_list>
#include <vector>

class Geometry : public Node
{
public:
    void Accept(Visitor& visitor) override;

    void SetData(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, std::initializer_list<uint32_t>&& attributes);
    void SetVertices(const std::vector<float>& vertices);
    void SetIndices(const std::vector<uint32_t>& indices);
    void SetAttributes(std::initializer_list<uint32_t>&& attributes);

    void Compile();
    void Draw();

private:
    std::vector<float> m_vertices {};
    std::vector<uint32_t> m_indices {};
    std::vector<uint32_t> m_attributes {};

    uint32_t m_VAO { 0 };
};
