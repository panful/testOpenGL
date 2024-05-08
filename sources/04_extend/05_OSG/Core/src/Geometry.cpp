#include "Geometry.h"
#include "Log.h"
#include "Visitor.h"

#include <glad/glad.h>
#include <numeric>

void Geometry::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void Geometry::SetData(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, std::initializer_list<uint32_t>&& attributes)
{
    m_vertices   = vertices;
    m_indices    = indices;
    m_attributes = attributes;
}

void Geometry::SetVertices(const std::vector<float>& vertices)
{
    m_vertices = vertices;
}

void Geometry::SetIndices(const std::vector<uint32_t>& indices)
{
    m_indices = indices;
}

void Geometry::SetAttributes(std::initializer_list<uint32_t>&& attributes)
{
    m_attributes = attributes;
}

void Geometry::Compile()
{
    Log::GetInstance()->Trace();

    glGenVertexArrays(1, &m_VAO);

    // TODO release resources
    uint32_t VBO { 0 }, EBO { 0 };
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

    GLuint stride = std::accumulate(m_attributes.begin(), m_attributes.end(), 0);
    GLuint index { 0 }, offset { 0 };
    for (auto& attribute : m_attributes)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, attribute, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(sizeof(GLfloat) * offset));
        offset += attribute;
        index++;
    }

    glBindVertexArray(0);
}

void Geometry::Draw()
{
    Log::GetInstance()->Trace();

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
