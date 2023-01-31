#pragma once
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

struct Vertex
{
    //顶点坐标
    glm::vec3 Position;
    //顶点颜色
    glm::vec3 Color;
    //顶点法向量
    //glm::vec3 Normal;
    ////顶点纹理坐标
    //glm::vec2 TexCoords;
    ////切线
    //glm::vec3 Tangent;
    ////双切线
    //glm::vec3 Bitangent;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};


class Mesh
{
public:
    Mesh(std::vector<Vertex>, std::vector<Texture> = std::vector<Texture>{}, std::vector<unsigned int> indices = std::vector<unsigned int>{});
    ~Mesh();

    void draw();

private:


public:
    bool m_bDisplay;
    unsigned int m_VAO, m_VBO, m_EBO;

    std::vector<Vertex>       m_vertices;   //顶点
    std::vector<Texture>      m_textures;   //纹理
    std::vector<unsigned int> m_indices;    //索引
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices)
    :m_vertices(vertices),
    m_textures(textures),
    m_indices(indices),
    m_VAO(0),
    m_VBO(0),
    m_EBO(0),
    m_bDisplay(true)
{
    if (indices.size() > 0)  //使用glDrawElements绘制
    {
        //创建
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        //绑定
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        //填充
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
        //配置：顶点坐标
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        //配置：顶点颜色
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        //配置：顶点法线
        //glEnableVertexAttribArray(1);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        ////配置：顶点纹理坐标
        //glEnableVertexAttribArray(2);
        //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        ////配置：切线
        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        ////配置：双切线
        //glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        //释放VAO
        glBindVertexArray(0);
    }
    else //使用glDrawArrays绘制
    {
        //创建
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        //绑定
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        //填充
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
        //配置：顶点坐标
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        //配置：顶点颜色
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        //配置：顶点法线
        //glEnableVertexAttribArray(1);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        ////配置：顶点纹理坐标
        //glEnableVertexAttribArray(2);
        //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        ////配置：切线
        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        ////配置：双切线
        //glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        //释放VAO
        glBindVertexArray(0);
        
    }
}

Mesh::~Mesh()
{
}

void Mesh::draw()
{
    if (!m_bDisplay)
        return;

    if (m_indices.size() > 0)
    {
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    else
    {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
        glBindVertexArray(0);
    }

}